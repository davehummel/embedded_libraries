
#ifndef DH_CONTROLLEDNAV_H__
#define DH_CONTROLLEDNAV_H__
#include "dh_controller.h"
 #include <ControlledPanTilt.h>
 #include <ControlledMotor.h>
 #include <ControlledI2CXL.h>
 #include <ControlledBN055.h>
 #include <ControlledLidar.h>
#include "quadfit.h"



#define ABS_CONF  1.2
#define HIGH_CONF 3
#define LOW_CONF 10

#define NAV_ID 200
#define ECHO_ID 210
#define LIDAR_ID 216
#define LIA_ID 220
#define GYRO_ID 222
#define MOTOR_ID 230
#define SERVO_ID 240

#define MIN_DIST 25
#define STOP_DIST 30

#define BASE_DELAY 100
#define BASE_INTERVAL 10

#define ACCEL_MULT 1
#define ACCEL_DELAY 5

#define RANGE_MULT 10
#define RANGE_EVAL_MULT 5

#define ECHO_MULT 10
#define ECHO_DELAY 1
#define ECHO_MIN 24
#define ECHO_MAX 400
#define ECHO_AB_DELAY 40

#define LIDAR_MULT 2
#define LIDAR_DELAY 0
#define LIDAR_MIN 40
#define LIDAR_MAX 2000

#define GYRO_MULT 1
#define GYRO_DELAY 2

#define MIN_MOTOR_DIF 512;



#define DISCOVERWAITSTILL 1
#define DISCOVERMINROTM0 2
#define DISCOVERMINROTM1 3
#define DISCOVERMOTORRESPONSEM0 4
#define DISCOVERMOTORRESPONSEM1 5
#define DISCOVERSMALLESTTURNM0 6
#define DISCOVERSMALLESTTURNM1 7
#define ACTION_FAILED 10
#define DECIDE_ACTION 11
#define CORRECTION_STOPPED_PULSE 12
#define CORRECTION_FINAL_APPROACH 13
#define HARD_APPROACH 15
#define MOVING_CORRECTION 18

class HeadingResponse {
public:

	HeadingResponse(uint16_t cycleTimeMS){
		cycleTime = cycleTimeMS;
	}

	void startSelfDiscovery(){
		state = DISCOVERWAITSTILL;
		prevState = 0;
		stateCycle = 0;
		minM0CyclePulse = 5;
		minM1CyclePulse = 5;
	}

	void turn(){
		stateCycle = 0;
		state = DECIDE_ACTION;
	}

	void processCycle(double headingDelta, double headingVel, double headingAccel, double avgAccel, int16_t& throttle, int16_t& m0, int16_t& m1, int16_t& differential, bool& breaks){
		breaks = false;
		if (state != lastSeenState){
			prevState = lastSeenState;
			lastSeenState = state;
		}
		switch (state){
			case 0:
				breaks = true;
				return;
			case DISCOVERWAITSTILL:
				if (throttle == 0 && headingVel == 0)
					stateCycle++;
				if (stateCycle == 50){
					if (prevState == 0){
						state = DISCOVERMINROTM0;
						m0 = 1;
					}
					else if (prevState == DISCOVERMINROTM0){
						state = DISCOVERMINROTM1;
						m1 = 1;
					}
					else if (prevState == DISCOVERMINROTM1){
						minThrottle = max(minM0Throttle, minM1Throttle);
						Serial.print("Min Throttle:");
						Serial.print(minM0Throttle);
						Serial.print(',');
						Serial.println(minM1Throttle);
						delayCycleCount = 0;
						state = DISCOVERMOTORRESPONSEM0;
					}
					else if (prevState == DISCOVERMOTORRESPONSEM0){
						state = DISCOVERMOTORRESPONSEM1;
					}
					else if (prevState == DISCOVERMOTORRESPONSEM1){
						delayCycleCount /= 2;
						Serial.print("Delay Cycle Count:");
						Serial.println(delayCycleCount);
						state = DISCOVERSMALLESTTURNM0;

						stateCycle = 0;
					}
					else if (prevState == DISCOVERSMALLESTTURNM0){
						stateCycle = 0;
						state = DISCOVERSMALLESTTURNM1;
					}
					else if (prevState == DISCOVERSMALLESTTURNM1){
						Serial.print("MinPulse:");
						Serial.print(minM0CyclePulse);
						Serial.print(',');
						Serial.println(minM1CyclePulse);
						minCyclePulse = min(minM0CyclePulse, minM1CyclePulse);
						state = 0;
					}
					stateCycle = 0;
				}
				return;
			case DISCOVERMINROTM0:{
				testMinRot(headingVel, m0, minM0Throttle);
				return;
			}
			case DISCOVERMINROTM1:{
				testMinRot(headingVel, m1, minM1Throttle);
				return;
			}
			case DISCOVERMOTORRESPONSEM0:{
				testMotorResp(headingVel, m0, delayCycleCount);
				return;
			}
			case DISCOVERMOTORRESPONSEM1:{
				testMotorResp(headingVel, m1, delayCycleCount);
				return;
			}
			case DISCOVERSMALLESTTURNM0:{
				testMinTurn(headingDelta, m0,minM0CyclePulse);
				return;
			}
			case DISCOVERSMALLESTTURNM1:{
				testMinTurn(headingDelta, m1, minM1CyclePulse);
				return;
			}
			case ACTION_FAILED:{
				Serial.print("State ");
				Serial.print(prevState);
				Serial.println(" : Failed");
				Serial1.println("<0:Action FAILED");
				breaks = true;
				state = 0;
				stateCycle = 0;
				return;
			}

			case DECIDE_ACTION:{
				Serial.println("deciding...");
				if (throttle > 0){
					m0 = m1 = throttle;
					if (headingVel == 0){
						// probably not moving yet (too low throttle?)
						if (abs(headingDelta) > finalCorrectionRange){
							state = CORRECTION_STOPPED_PULSE;
							Serial.println("throttled in CORRECTION_STOPPED_PULSE");
							stateCycle = 0;
							startPulseAttempt = 0;
							differential = 0;
							return;
						}
						else{
							Serial.println("DOING NOTHING");
							// Do nothing!
							differential = 0;
							return;
						}
					}
					else {
						Serial.println("MOVING_CORRECTION");
						state = MOVING_CORRECTION;
						startPulseAttempt = 0;
						stateCycle = 0;
						return;
					}
					return;
				}
				double delta = abs(headingDelta);
				if (abs(headingVel) <= minValidVelocity){
					// IN a standing position
					
					if (delta < minMoveDist){
						state = 0;
						stateCycle = 0;
						return;
					}
					
					state = CORRECTION_STOPPED_PULSE;
					stateCycle = 0;
					startPulseAttempt = 0;
					return;
					
				}
				if (abs(headingDelta) < finalCorrectionRange){
					if ((headingDelta > 0 && headingVel > 0) || (headingDelta < 0 && headingVel < 0)){
						state = CORRECTION_FINAL_APPROACH;
						stateCycle = 0;
						return;
					}
					else{
						// close but moving the wrong direction
						state = HARD_APPROACH;
						stateCycle = 0;
						return;
					}
				}	else{
					state = HARD_APPROACH;
					stateCycle = 0;

					return;
				}
			
			}
			return;
			case MOVING_CORRECTION:{
				m0 = m1 = throttle;

				if (stateCycle == 0){
					double targetVel = solveRotVelCubic(headingDelta - solvePos(prevOffsetVelocity[0], 0, ((double)(delayCycleCount - 1) * BASE_INTERVAL * GYRO_MULT) / 1000.l));
					differential = calcNewOffset(targetVel);
				}
				if (stateCycle < delayCycleCount - 1){
					updateRotModelVel(headingVel);
				}
				if (stateCycle == delayCycleCount){
					stateCycle = 0;
					updateRotModelDif(differential);
					return;
				}
				
				
				stateCycle++;
				break;
			}

			case HARD_APPROACH:{
				bool turnLeft = headingDelta < 0;
				uint16_t boost = minThrottle + ((255.0l - minThrottle)) * (abs(headingDelta)/180.0l);
				if (stateCycle < minCyclePulse)
					boost = 255;
				m0 = m1 = throttle;
				if (!turnLeft){
					m0 += boost;
					m1 -= boost;
				}
				else{
					m0 -= boost;
					m1 += boost;
				}
				if (abs(headingDelta) < finalCorrectionRange){
					state = CORRECTION_FINAL_APPROACH;
					stateCycle = 0;
					m0 = m1 = throttle;
					breaks = true;
					return;
				}
				stateCycle++;
				break;
			}
			case CORRECTION_FINAL_APPROACH:{
				if (abs(headingVel) < minValidVelocity){
					stateCycle = 0;
					state = DECIDE_ACTION;
					return;
				}
				double posIn100ms = headingDelta - solvePos(headingVel, avgAccel,.1);
				bool turnLeft = headingDelta < 0;
				int8_t score = 0;
				if (posIn100ms>4)
					score = 3;
				else if (posIn100ms>2)
					score = 2;
				else if (posIn100ms > 1)
					score = 1;
				else if (posIn100ms > -1)
					score = 0;
				else if (posIn100ms > -2)
					score = -1;
				else if (posIn100ms > -4)
					score = -2;
				else
					score = -3;

				if (turnLeft)
					score = -score;

				if (abs(headingDelta) < minMoveDist && (score == 0 || score == 1 || score == -1) ){
					breaks = score != 1;
					stateCycle = 0;
					state = 0;
				}

				switch (score){
				case 3:
					m0 = m1 = throttle;
					if (!turnLeft){
						m0 += minThrottle;
						m1 -= minThrottle;
					}
					else{
						m0 -= minThrottle;
						m1 += minThrottle;
					}
					break;
				case 2:
					
				case 1:
					m0 = m1 = throttle;
					if (!turnLeft)
						m0 += minThrottle;
					else
						m1 += minThrottle;
					break;
				case 0:
					m0 = m1 = throttle;
					break;
				case -1:
					breaks = true;
					break;
				case -2:
					m0 = m1 = throttle;
					if (turnLeft)
						m0 += minThrottle/2;
					else
						m1 += minThrottle/2;
					break;
				case -3:
					m0 = m1 = throttle;
					if (turnLeft){
						m0 += minThrottle;
						m1 -= minThrottle;
					}
					else{
						m0 -= minThrottle;
						m1 += minThrottle;
					}
				}
				break;
			}
			case CORRECTION_STOPPED_PULSE:{
				bool turnLeft = headingDelta < 0;
	
				
				if (turnLeft && headingVel < -minValidVelocity){
					stateCycle = 0;
					Serial.print("Success Left!");
					state = DECIDE_ACTION;
					return;
				}
				else if (!turnLeft && headingVel > minValidVelocity){
					stateCycle = 0;
					Serial.print("Success Right!");
					state = DECIDE_ACTION;
					return;
				}
				switch (startPulseAttempt){
				case 0: // normal move
					if (stateCycle == 0){
						m0 = m1 = throttle;
						if (!turnLeft)
							m0 += minThrottle;
						else
							m1 += minThrottle;
					}
					else if (stateCycle == 3 * minCyclePulse){
						stateCycle = 0;
						m0 = m1 = throttle;
						startPulseAttempt++;
						return;
					}

					break;
				case 1: // normal move opposite side only
					if (stateCycle == 0){
						m0 = m1 = throttle;
						if (turnLeft)
							m0 -= minThrottle;
						else
							m1 -= minThrottle;
					}
					else if (stateCycle == 3 * minCyclePulse){
						stateCycle = 0;
						m0 = m1 = throttle;
						startPulseAttempt++;
						return;
					}
					break;
				case 2: // more power normal move
					if (stateCycle == 0){
						m0 = m1 = throttle;
						if (!turnLeft)
							m0 += minThrottle * 1.5;
						else
							m1 += minThrottle * 1.5;
					}
					else if (stateCycle == 3 * minCyclePulse){
						stateCycle = 0;
						m0 = m1 = throttle;
						startPulseAttempt++;
						return;
					}
					break;
				case 3: // more power opposite side only
					if (stateCycle == 0){
						m0 = m1 = throttle;
						if (turnLeft)
							m0 -= minThrottle * 1.5;
						else
							m1 -= minThrottle * 1.5;
					}
					else if (stateCycle == 3 * minCyclePulse){
						stateCycle = 0;
						m0 = m1 = throttle;
						startPulseAttempt++;
						return;
					}
					break;
				case 4: // both sides, normal power with gradual ramp to full power
					if (stateCycle % minCyclePulse == 0){
						m0 = m1 = throttle;
						uint8_t boost = stateCycle / minCyclePulse * 20;
						if (boost > 255){
							stateCycle = 0;
							m0 = m1 = throttle;
							startPulseAttempt++;
						}
						if (!turnLeft){
							m0 += boost;
							m1 -=  boost;
						}
						else{
							m0 -= boost;
							m1 +=  boost;
						}
					}
					break;
				default: // FAILURE!
					Serial.println("Failure");
					stateCycle = 0;
					state = ACTION_FAILED;
					break;
				}

				stateCycle++;
				break;
			}
			default:
			Serial.println("Unkown State1");
				breaks = true;
				state = 0;
				stateCycle = 0;
				return;
		}
	}

	void updateRotModelVel(double headingVel){
		prevOffsetVelSamples++;
		prevOffsetVelocity[0] += headingVel;
	}

	void updateRotModelDif(double dif){
		prevOffset[0] = dif;
		prevOffsetVelocity[0] /= (double)prevOffsetVelSamples;
		prevOffsetVelSamples = 0;
	}

	int16_t calcNewOffset(double targetVel){
	
		Serial.print("OV|OF:");

	
		double a, b;
		//if ((prevOffsetVelocity[3] == 0 && prevOffsetVelocity[4] == 0 && prevOffsetVelocity[5] == 0) || !linreg(10, prevOffset, prevOffsetVelocity, b, a)){
			a = 5;
			b = -20.;
		//}
		for (uint8_t i = 9; i >0; i--){
			Serial.print(prevOffsetVelocity[i-1]);
			Serial.print('|');
			Serial.print(prevOffset[i - 1]);
			Serial.print(" ");
			prevOffsetVelocity[i] = prevOffsetVelocity[i - 1];
			prevOffset[i] = prevOffset[i - 1];
		}
		prevOffsetVelocity[0] = 0;
		prevOffset[0] = 0;
		Serial.print(" a:");
		Serial.print(a);
		Serial.print(" b:");
		Serial.print(b);
		return a + b*targetVel;
	}

	void testMinTurn(double delta, int16_t& mX, uint8_t& pulse){
		if (stateCycle == 0){
			mX = 120;
			initDelta = delta;
		}
		if (stateCycle == 2){
			mX = 255;
			initDelta = delta;
		}
		if (stateCycle == pulse){
			mX = 0;
		}
		if (stateCycle == delayCycleCount * 2){
			if (abs(initDelta - delta) < .1){
				pulse++;
				if (pulse == 100){
					stateCycle = 0;
					mX = 0;
					state = ACTION_FAILED;
					return;
				}
			}
			else{
				state = DISCOVERWAITSTILL;
				mX = 0;
				stateCycle = 0;
				return;
			}
		}
		stateCycle++;
	}

	void testMotorResp(double headingVel, int16_t& mX, uint8_t& cycleCount){
		if (stateCycle == 0){
			mX = -120; // max(minM0Throttle, minM1Throttle);
		}
		else {
			cycleCount++;
		}
		if (stateCycle == 2){
			mX = -255; // max(minM0Throttle, minM1Throttle);
		}
		if (headingVel != 0){
			stateCycle = 0;
			mX = 0;
			state = DISCOVERWAITSTILL;
			return;
		}
		if (stateCycle > 100){
			stateCycle = 0;
			mX = 0;
			state = ACTION_FAILED;
			return;
		}
		if (stateCycle == 20){
			mX = 0;
		}
		stateCycle++;
	
	}

	void testMinRot(double headingVel, int16_t & mX, uint8_t & minMX){
		if (stateCycle == 20){
			// Success!
			minMX = mX;
			mX = 0;

			stateCycle = 0;
			state = DISCOVERWAITSTILL;
			return;
		}
		if (stateCycle >10){
			if ((headingVel <.1 && headingVel > -.1))
				stateCycle = 0;
		}
		if (stateCycle == 0)
			mX += 2;
		if (mX>255){
			stateCycle = 0;
			state = ACTION_FAILED;
			mX = 0; 
			return;
		}
		stateCycle++;
	}

	void abortPrev(){
		stateCycle = 0;
		state = 0;
	}

	uint8_t getState(){
		return state;
	}

	double solveRotVelCubic(double headingDelta){
		return headingDelta * responseCurve[1] + headingDelta*headingDelta*headingDelta*responseCurve[0];
	}

	double responseCurve[2] = { .02, 1 };

	uint8_t minM0Throttle= 90;
	uint8_t minM1Throttle = 90;
	uint8_t minThrottle = 90;
	uint8_t delayCycleCount = 8;
	uint8_t minM0CyclePulse = 8;
	uint8_t minM1CyclePulse = 8;
	uint8_t minCyclePulse = 8;
	double finalCorrectionRange = 2.5;
	double minMoveDist = .5;
	double minValidVelocity = 5;

	double initDelta;

private:
	uint16_t cycleTime = 10;
	uint8_t state = 0;
	uint8_t prevState = 0;
	uint8_t lastSeenState = 0;
	uint8_t stateCycle = 0;
	uint8_t stateCompleteCycle = 0;
	uint8_t startPulseAttempt = 0;
	double prevOffset[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };;
	uint8_t prevOffsetVelSamples = 0;
	double prevOffsetVelocity[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	double solvePos(double vel, double accel, double time){
		return vel * time + .5l*accel*time*time;
	}

};




class ControlledNavigator: public Controller::Controlled{

public:

	void setup(void){

		rangeFreq = 1000.l/(baseInterval*rangeMult);
		accelFreq = 1000.l/(baseInterval*accelMult);
		gyroFreq = 1000.l / (baseInterval*gyroMult);

		qfitA.setSampleCount(rangeEvalMult);
		qfitB.setSampleCount(rangeEvalMult);
		qfitL.setSampleCount(rangeEvalMult);
		if (qfitA.xData)
			delete[] qfitA.xData;
		qfitA.xData = new double[rangeEvalMult];
		if (qfitB.xData)
			delete[] qfitB.xData;
		qfitB.xData = new double[rangeEvalMult];
		if (qfitL.xData)
			delete[] qfitL.xData;
		qfitL.xData = new double[rangeEvalMult];
		if (qfitA.yData)
			delete[] qfitA.yData;
		qfitA.yData = new double[rangeEvalMult];
		if (qfitB.yData)
			delete[] qfitB.yData;
		qfitB.yData = new double[rangeEvalMult];
		if (qfitL.yData)
			delete[] qfitL.yData;
		qfitL.yData = new double[rangeEvalMult];

		if (qfitA.approxYVals)
			delete[] qfitA.approxYVals;
		qfitA.approxYVals = new double[rangeEvalMult];
		if (qfitB.approxYVals)
			delete[] qfitB.approxYVals;
		qfitB.approxYVals = new double[rangeEvalMult];
		if (qfitL.approxYVals)
			delete[] qfitL.approxYVals;
		qfitL.approxYVals = new double[rangeEvalMult];
		for (uint16_t i = 0; i < rangeEvalMult; i++){
			qfitA.xData[i] = i;
			qfitB.xData[i] = i;
			qfitL.xData[i] = i;
		}
		qfitA.processXData();
		qfitB.processXData();
		qfitL.processXData();

		if (prevErrors)
			delete[] prevErrors;
		prevErrors = new double[rangeEvalMult];
		if (prevEsts)
			delete[] prevEsts;
		prevEsts = new double[rangeEvalMult];
	}

	void begin(void){

		setup();

		echo = (ControlledI2CXL*) controller->getControlled('S');
		motor = (ControlledMotor*) controller->getControlled('M');
		panTilt = (ControlledPanTilt*) controller->getControlled('P');
		gyro = (ControlledBN055*) controller->getControlled('G');
		lidar = (ControlledLidar*) controller->getControlled('L');
		headingResponse = new HeadingResponse(baseInterval * gyroMult);

	}

	void execute(uint32_t time,uint32_t id,char command[], bool serializeOnComplete){
		
		uint16_t pointer = 0;
		switch (command[0]){
			case 'Z':{ // ZERO
				
				uint8_t sampleRate;
				if (command[4] == '\0'){
					lia_bias[0]+= gyro->accel[0];
					lia_bias[1]+= gyro->accel[1];
					lia_bias[2]+= gyro->accel[2];
					
					break;
				}

				pointer = 5;
				if (!Controller::parse_uint16(zeroSampleCount,pointer,command)){
					return;
				}

				pointer++;
				if (!Controller::parse_uint8(sampleRate,pointer,command)){
					return;
				}

					lia_bias[0]=0;
					lia_bias[1]=0;
					lia_bias[2]=0;

				controller->schedule(NAV_ID,sampleRate,sampleRate,false,zeroSampleCount,Controller::newString("ZERO"),'N',true);
				controller->schedule(LIA_ID,1,sampleRate,false,zeroSampleCount,Controller::newString("LIA"),'G',true);
		
				break;
			}
			case 'D':{ // DISCOVER
				if (command[1] == 'P'){
					break;
				}
				if (command[1] == '\0'){
					navEval(time);
					calcDirection();
					bool breaks = false;
		
					headingResponse->processCycle( headingDelta,  estHeadingVel, estHeadingAccel,avgHeadingAccel, currentThrottle,  m0,  m1,differential,breaks);
					if (!breaks){

						if (differential==0)
							motor->setMotorDirect(m0, m1);
						else
							motor->setMotor(currentThrottle,differential);

						motor->updateMotor();
					}
					else{
						motor->stopMotor();
					}
					if (headingResponse->getState() == 0){
						stop();
						controller->kill(NAV_ID);
					}
					break;
				}
				if (isMoving){
					stop();
				}
				maxAllowedTime = time + 100000;
				start(false);
				headingResponse->startSelfDiscovery();
				controller->schedule(NAV_ID,BASE_DELAY+baseInterval,baseInterval,false,0,Controller::newString("D"),'N',true);
				break;
			}
			case 'T':{ // TURN
				if (command[1] == '\0'){
					bool abort = navEval(time);
					if (abort){
						stop();
						return;
					}
					calcDirection();
					bool breaks = false;
					headingResponse->processCycle(headingDelta, estHeadingVel, estHeadingAccel, avgHeadingAccel, currentThrottle, m0, m1, differential, breaks);
					if (!breaks){
						if (differential==0)
							motor->setMotorDirect(m0, m1);
						else
							motor->setMotor(currentThrottle, differential);
						motor->updateMotor();
					}
					else{
						motor->stopMotor();
					}


					if (headingResponse->getState() == 0){
						stop();
						controller->kill(NAV_ID);
					}
					break;
				}

				pointer = 5;
				if (!Controller::parse_double(targetHeading,pointer,command)){
					return;
				}
				pointer++;

				uint32_t maxMoveTime;

				if (!Controller::parse_uint32(maxMoveTime, pointer, command)){
					return;
				}

				maxAllowedTime = maxMoveTime + time;

				if (headingResponse->getState() != 0){
					headingResponse->abortPrev();
					stop();
				}
				start(false);
				headingResponse->turn();
				controller->schedule(id, 20, 0, false, 1, Controller::newString("HEADING"), 'G', true);
				controller->schedule(NAV_ID, baseInterval * gyroMult * 2, baseInterval * gyroMult, false, 0, Controller::newString("T"), 'N', true);
				break;
			}
			case 'M':{ // MOVE
				if (command[1] == '\0'){
					moveEval(time);
					bool abort = navEval(time);
					if (abort){
						stop();
						return;
					}
					calcDirection();
					bool breaks = false;

					//TODO put in throttleResponse class;
					headingResponse->processCycle(headingDelta, estHeadingVel, estHeadingAccel, avgHeadingAccel, currentThrottle, m0, m1, differential, breaks);
					if (!breaks){

						if (differential==0)
							motor->setMotorDirect(m0, m1);
						else
							motor->setMotor(currentThrottle, differential);

						motor->updateMotor();
					}
					else{
						motor->stopMotor();
					}


					if (headingResponse->getState() == 0){
						stop();
						controller->kill(NAV_ID);
					}
					break;
				}
				pointer = 5;
				uint16_t finalTime = 0;
				while (true){
					int16_t throttle;
					uint16_t timeOffset;
					if (!Controller::parse_int16(throttle, pointer, command)){
						return;
					}
					pointer++;
					if (command[pointer] == 'B'){
						throttle = -256;
					} else 	if (!Controller::parse_uint16(timeOffset, pointer, command)){
						return;
					}
					if (timeOffset == 0){
						currentOffset = 0;
						offsetStartTime = time;
						finalTime = 0;
					}
					else if (timeOffset > finalTime){
						finalTime = timeOffset;
					}
					if (throttle > 255)
						throttle = 255;
					if (throttle < -256) // perserve break value
						throttle = -255
					addMovePoint(throttle, timeOffset);
				}


				targetHeading = 0;

				if (isMoving){
					stop();
				}

				maxAllowedTime = offsetStartTime+finalTime;

				if (panTilt->getHeight() != 512)
					controller->run(SERVO_ID,Controller::newString("MOVE 2 512"),'P',false);
				if (panTilt->getGoal() != 512)
					controller->run(SERVO_ID,Controller::newString("MOVE 1 512"),'P',false);


				start(true);
				headingResponse->turn();
				controller->schedule(id, 20, 0, false, 1, Controller::newString("HEADING"), 'G', true);
				controller->schedule(NAV_ID, baseInterval * gyroMult * 2, baseInterval * gyroMult, false, 0, Controller::newString("M"), 'N', true);
				break;
			}
		}
	}
	void serialize(Stream* output, uint32_t id, char command[]){
		if (printMovingUpdate == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->print('M');
			output->print(estDist);
			output->print('S');
			output->println(estSpeed);

			printMovingUpdate = false;

		}
		if (printZeroUpdate == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->print("ZERO");

			output->print(lia_bias[0]);
			output->print(',');
			output->print(lia_bias[1]);
			output->print(',');
			output->println(lia_bias[2]);

			printZeroUpdate = false;

		}	
		if (printDiscUpdate == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->print("DISC");
			output->print(headingResponse->minM0Throttle);
			output->print(",");
			output->print(headingResponse->minM1Throttle);
			output->print(";");
			output->println(headingResponse->delayCycleCount);
			printDiscUpdate = false;

		}
		if (printBlockUpdate == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->print("BLOCKED");

			output->println(nearestObj);

			printBlockUpdate = false;

		}	
		if (printStarted == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->println("Started");

			printStarted = false;

		}	
	}
	void startSchedule(char command[], uint32_t id){
	
	}
	void endSchedule(char command[], uint32_t id){
		switch (command[0]){
			case 'Z':{ // ZERO
				if (command[4] == '\0'){
					lia_bias[0]/=(double)zeroSampleCount;
					lia_bias[1]/=(double)zeroSampleCount;
					lia_bias[2]/=(double)zeroSampleCount;
					printZeroUpdate = true;
					controller->schedule(id, 20, 0, false, 1, Controller::newString("HEADING"), 'G', true);
					break;
				}
			}
			case 'D':{
				if (command[1] == '\0'){
					Serial1.println("QOAS");
					printDiscUpdate = true;
					controller->run(id, Controller::newString("DP"), 'N', true);
					break;
				}
			}
		}
	}


	uint16_t targetSpeed;
	uint16_t targetDist;
	double targetHeading;
	int16_t currentThrottle;
	int16_t m0=0, m1=0 , differential;
	uint16_t nearestObj;
	double estSpeed;
	double estDist;
	double estHeading;
	double estHeadingVel;
	double estHeadingAccel;
	double avgHeadingAccel;
	double lia_bias[3] = {0.,0.,0.};
	bool isMoving = false;
	
private:
	// DIRECT CONFIGURATIONS to SENSOR LOGIC
	uint16_t minDist = MIN_DIST;
	uint16_t stopDist = STOP_DIST;
	uint16_t baseInterval = BASE_INTERVAL;
	uint16_t accelMult = ACCEL_MULT;
	uint16_t accelDelay = ACCEL_DELAY;
	uint16_t rangeMult = RANGE_MULT;
	uint16_t rangeEvalMult = RANGE_EVAL_MULT;
	uint16_t echoMult = ECHO_MULT;
	uint16_t echoDelay = ECHO_DELAY;
	uint16_t echoMin = ECHO_MIN;
	uint16_t echoMax = ECHO_MAX;
	uint16_t echoABDelay = ECHO_AB_DELAY;
	uint16_t lidarMult = LIDAR_MULT;
	uint16_t lidarDelay = LIDAR_DELAY;
	uint16_t lidarMin = LIDAR_MIN;
	uint16_t lidarMax = LIDAR_MAX;
	uint16_t gyroMult = GYRO_MULT;
	uint16_t gyroDelay = GYRO_DELAY;
	uint16_t zeroSampleCount;
	// DERIVED CONFIGURATIONS to SENSOR LOGIC
	uint16_t cyclesPerRangeEval = 0;
	QuadFit<double> qfitA,qfitB,qfitL;
	
	uint8_t echoSubSamples;
	uint8_t lidarSubSamples;
	double accelFreq;
	double rangeFreq;
	double gyroFreq;
	double headingDelta;
	uint8_t headingOvershotResponse;
	double headingVelocity;
	double headingSamples[3] = { 0., 0., 0. };
	// // OPERATIONAL 
	HeadingResponse* headingResponse;
	uint16_t rangeSampleCount;
	uint32_t maxAllowedTime ;
	uint32_t cycleCount;
	int16_t turnFactor;
	int16_t minTurnThrottle;

	double prevEstSpeed;
	double* prevEsts;

	double* prevErrors;

	double prevAccelYData;



	bool printMovingUpdate = false;
	bool printZeroUpdate = false;
	bool printDiscUpdate = false;
	bool printBlockUpdate = false;
	bool printStarted = false;

	uint32_t printTime = 0;

	ControlledMotor* motor;
	ControlledI2CXL* echo;
	ControlledPanTilt* panTilt;
	ControlledBN055* gyro;
	ControlledLidar* lidar;

	bool navEval(uint32_t time){
		cycleCount++;
		printTime = time;
		if (time >= maxAllowedTime){
			Serial.println("TIME ABORT!");
			stop();
			return true;
		}
		return false;
	}


	void moveEval(uint32_t time){


		nearestObj = echo->readingA;
		if (echo->readingB < nearestObj)
			nearestObj = echo->readingB;

		if (nearestObj<=stopDist && nearestObj > 0){
			printBlockUpdate = true;
			currentThrottle = 0;
			stop();
			return;
		}

	
		calcSpeed();
		calcDirection();



		if (estSpeed*.1 + estDist >= targetDist){
			Serial.println("FINISHED!");
			stop();
			return;
		}

// TODO write slow down function here
		if (estSpeed*.3 + estDist >= targetDist){
				//	initThrottle = 40;
		}

		//updateMotor(headingDelta,estHeadingVel,estHeadingAccel,currentThrottle,m0,m1);

	}

	void calcDirection(){
		if (cycleCount%gyroMult==0) {
			processGyro();
		}
	}


	void calcSpeed(){
		// TODO try to collect regular data
		if (cycleCount%accelMult==0){
			processAccel();
		}

		if (cycleCount%echoMult==0){
			qfitA.yData[rangeEvalMult-1] += echo->readingA;
			qfitB.yData[rangeEvalMult-1] += echo->readingB;
			echoSubSamples++;
		}
		if (cycleCount%lidarMult==0){
			qfitL.yData[rangeEvalMult-1] += lidar->lastRead;
			lidarSubSamples++;
		}

	
		if (cycleCount%rangeMult == 0){	
			qfitA.yData[rangeEvalMult-1] /= (double) echoSubSamples;
			qfitB.yData[rangeEvalMult-1] /= (double) echoSubSamples;
			qfitL.yData[rangeEvalMult-1] /= (double) lidarSubSamples;
			echoSubSamples = 0;
			lidarSubSamples = 0;

			if (rangeMult == cycleCount){
				presample();
			}

			double confA=999,confB=999,confL=999;
		//	Serial.print("EchoA:");
			calcConf(confA,echoMin,echoMax,qfitA);
		//	Serial.print("EchoB:");
			calcConf(confB,echoMin,echoMax,qfitB); 

		//	Serial.print("Lidar:");
			calcConf(confL,echoMin,echoMax,qfitL);


			double finalConf = mergeRangeAndAccel(confA,confB,confL);


			shift(rangeEvalMult, qfitA);
			shift(rangeEvalMult, qfitB);
			shift(rangeEvalMult, qfitL);
			updatePrevEstimates(finalConf);

			printMovingUpdate = true;
		}

	}

	void updatePrevEstimates(double error){
		for (uint8_t i = 0; i < rangeEvalMult - 1; i++){
			prevEsts[i] = prevEsts[i + 1];
			prevErrors[i] = prevErrors[i + 1];
		}
		prevEsts[rangeEvalMult - 1] = estDist;
		prevErrors[rangeEvalMult - 1] = error;
	}

	void processAccel(){
		double prevSpeed = estSpeed;
		double curAccel =  lia_bias[1] -gyro->accel[1];
		estSpeed += ((prevAccelYData + curAccel)/ 2.0) / (accelFreq); 
		estDist += ((estSpeed + prevSpeed) / 2.0) / (accelFreq); 
		prevAccelYData = curAccel;


	}

	void processGyro(){
		bool firstUpdate = estHeading == -9999.;
		estHeading = gyro->heading[0];
		if (firstUpdate){
			targetHeading = estHeading + targetHeading;
		}
		
		headingDelta = solveHeadingDelta(estHeading,targetHeading);
		
		solveRotVelocity(firstUpdate);


		
		//motorDif = solveMotorDifferentialChange(headingDelta, estHeadingVel, estHeadingAccel,initThrottle, motorDif);

		//if (motorDif > 511)
		//	motorDif = 511;
		//else if (motorDif < -511)
		//	motorDif = -511;
		Serial.print("HD:");
		Serial.print(estHeading);
		Serial.print(" Tar:");
		Serial.print(targetHeading);
		Serial.print(" Dlt:");
		Serial.print(headingDelta);
		Serial.print(" Vel:");
		Serial.print(estHeadingVel);
		Serial.print(" Acc:");
		Serial.print(estHeadingAccel);	
		Serial.print(" , ");
		Serial.println(avgHeadingAccel);

	}

	double solveHeadingDelta(double source, double target){
		if (target > 180 || target < -180){
			target = target - 360 * (int)(target / 360);
		}
		if (source > 180 || source < -180){
			source = source - 360 * (int)(source / 360);
		}
		double delta = target - source;

		if (delta > 180.){
			delta =  delta - 360.;
		}else if (delta < -180.){
			delta = 360. + delta;
		}
		return delta;
	}

	void solveRotVelocity(bool firstUpdate){ 

		estHeadingAccel = estHeadingVel; // storing old value;

		if (firstUpdate == true){
			headingSamples[0] = headingDelta;
			headingSamples[1] = headingDelta;
			headingSamples[2] = headingDelta;
			estHeadingAccel = estHeadingVel = avgHeadingAccel = 0;
		//	estTTT = -1;
			return;
		}
		headingSamples[2] = headingSamples[1];
		headingSamples[1] = headingSamples[0];
		headingSamples[0] = headingDelta;

		estHeadingVel =  -((headingSamples[0] - headingSamples[1])*2.0l + (headingSamples[1] - headingSamples[2])) / 3.0l * gyroFreq;
	//	estHeadingVel = headingSamples[0] - headingSamples[1];
		estHeadingAccel = estHeadingVel - estHeadingAccel;
		avgHeadingAccel = avgHeadingAccel * .75l + (estHeadingAccel)*.25l;
		/*
		if (headingDelta == 0){
			estTTT = 0;
		}
		else if (estHeadingAccel == 0){
			estTTT = -headingDelta / estHeadingVel;
		} else {
			estTTT = estHeadingVel*estHeadingVel - 2.L * estHeadingAccel * headingDelta;
			if (estTTT < 0) {
				estTTT = -1;
			} else{
				estTTT = sqrt(estTTT);
				double alt = (-estHeadingVel + estTTT)/estHeadingAccel;
				estTTT = (-estHeadingVel - estTTT) / estHeadingAccel;
				if (estTTT< 0 || (alt>0 && alt<estTTT)){
					estTTT = alt;
				}

			}
		}
		if (estTTT < 0 || estTTT>100000)
			estTTT = -1;
		else
			estTTT *= 1000.L;
			*/
	}

	

	void calcConf(double &rangeConf,double min,double max, QuadFit<double> &qfit){

/*		Serial.print(qfit.yData[0]);
		Serial.print(',');
		Serial.print(qfit.yData[1]);
		Serial.print(',');
		Serial.print(qfit.yData[2]);
		Serial.print(',');
		Serial.print(qfit.yData[3]);
		Serial.print(',');
		Serial.print(qfit.yData[4]);
*/

		for (uint8_t i = 0 ; i < rangeEvalMult ; i++){
			if (qfit.yData[i]>=max && i > 0){
				Serial.println(" No Conf!!");
				rangeConf = 1000;
				return;
			}else if (qfit.yData[i]< min && i < rangeEvalMult){
				Serial.println(" No Conf!!");
				rangeConf = 1000;
				return;
			}
		}

		qfit.solve();

		qfit.calculate();

		rangeConf = 0;

		for (uint8_t i = 0 ; i <= rangeEvalMult ; i++){
			float temp = (qfit.approxYVals[i] - qfit.yData[i]);
			temp *= .2 + (float)i/3.0;
			rangeConf +=  temp>0? temp : -temp;
		}

//		Serial.print(" -> ");
//		Serial.println(rangeConf);
	
		return;

	}

	double mergeRangeAndAccel(double echoConfA,double echoConfB, double lidarConf ){
		if (echoConfA >= HIGH_CONF && echoConfB >= HIGH_CONF && lidarConf >= HIGH_CONF){
			Serial.println("Use Accel");
			return 999;// Just use accel data
		}

		float estSpeedEchoA,estSpeedEchoB,estSpeedLidar,estDistEchoA,estDistEchoB,estDistLidar;

		if (lidarConf < .01 ) {// zero check
			solveRangeMove(estSpeedLidar,estDistLidar,qfitL);
			estSpeed = estSpeedLidar;
			estDist = prevEsts[rangeEvalMult-1] + estDistLidar;
			Serial.println("Chose Lidar with perfect match");
			return lidarConf;
		}

		if (echoConfA < .01 ) {// zero check
			solveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
			estSpeed = estSpeedEchoA;
			estDist = prevEsts[rangeEvalMult - 1] + estDistEchoA;
			Serial.println("Chose Echo A with perfect match");
			return echoConfA;
		}

		if (echoConfB < .01 ) {// zero check
			solveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
			estSpeed = estSpeedEchoB;
			estDist = prevEsts[rangeEvalMult - 1] + estDistEchoB;
			Serial.println("Chose Echo B with perfect match");
			return echoConfB;
		}

		uint8_t trustLevelA, trustLevelB, trustLevelL;
		if (echoConfA < ABS_CONF)
			trustLevelA = 3;
		else if (echoConfA < HIGH_CONF)
			trustLevelA = 2;
		else if (echoConfA < LOW_CONF)
			trustLevelA = 1;
		else 
			trustLevelA = 0;

		if (echoConfB < ABS_CONF)
			trustLevelB = 3;
		else if (echoConfB < HIGH_CONF)
			trustLevelB = 2;
		else if (echoConfB < LOW_CONF)
			trustLevelB = 1;
		else 
			trustLevelB = 0;

		if (lidarConf < ABS_CONF)
			trustLevelL = 3;
		else if (lidarConf < HIGH_CONF)
			trustLevelL = 2;
		else if (lidarConf < LOW_CONF)
			trustLevelL = 1;
		else 
			trustLevelL = 0;

		// Serial.print("Trust Level A:");
		// Serial.print(trustLevelA);
		// Serial.print("Trust Level B:");
		// Serial.print(trustLevelB);
		// Serial.print("Trust Level L:");
		// Serial.println(trustLevelL);

		uint8_t maxTrustLevel = trustLevelA;
		if (trustLevelB>maxTrustLevel)
			maxTrustLevel = trustLevelB;
		if (trustLevelL>maxTrustLevel)
			maxTrustLevel = trustLevelL;


		if (maxTrustLevel == 3){
			uint8_t count = 0;
			estSpeed = 0;
			estDist = 0;
			double error = 0;

			if (trustLevelA == 3){
				solveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
				estSpeed += estSpeedEchoA;
				estDist += prevEsts[rangeEvalMult - 1] + estDistEchoA;
				count++;
				error += echoConfA;
			} 
			if (trustLevelB == 3){
				solveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
				estSpeed += estSpeedEchoB;
				estDist += prevEsts[rangeEvalMult - 1] + estDistEchoB;
				count++;
				error += echoConfB;
			} 
			if (trustLevelL == 3){
				solveRangeMove(estSpeedLidar,estDistLidar,qfitL);
				estSpeed += estSpeedLidar;
				estDist += prevEsts[rangeEvalMult - 1] + estDistLidar;
				count++;
				error += lidarConf;
			} 
			if (count>1){
				estSpeed/=(double)count;
				estDist/=(double)count;
				error /= (double)count;
			}
			return error;
		} else if (maxTrustLevel == 2){
			uint8_t count = 1;
			double error = 0;
			double distAddition = estDist - prevEsts[rangeEvalMult - 1];

			if (trustLevelA == 2){
				solveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
		 		estSpeed +=estSpeedEchoA;
				distAddition+= estDistEchoA;
				count++;
				error += echoConfA;
			} 

			if (trustLevelB == 2){
				solveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
				estSpeed  +=estSpeedEchoB;
				distAddition+= estDistEchoB;
				count++;
				error += echoConfB;
			} 	
			if (trustLevelL == 2){
				solveRangeMove(estSpeedLidar,estDistLidar,qfitL);
		 		estSpeed  +=estSpeedLidar;
				distAddition+= estDistLidar;
				count++;
				error += lidarConf;
			}
			estSpeed/=(double)count;
			estDist = prevEsts[rangeEvalMult - 1] + (distAddition) / (double)count;
			error /= (double)(count -1);
			return error;
		}

		return 999;

	}

	void solveRangeMove(float &echoSpeed,float &echoDist, QuadFit<double> &qfit ){
		echoDist =  qfit.approxYVals[rangeEvalMult -2 ] - qfit.approxYVals[rangeEvalMult -1 ] ;
		echoSpeed = -qfit.calculateD(4) * rangeFreq;
	}



	void shift(uint8_t size, QuadFit<double> &qfit){
		for (uint8_t i =0 ; i <size-1 ; i++){
			qfit.yData[i] = qfit.yData[i+1];
		}
		qfit.yData[size-1] = 0;
	}

	void presample(){
		prevEstSpeed = 0;
		prevEsts[rangeEvalMult - 1] = 0;
		estSpeed = 0;
		estDist = 0;
		for (uint8_t i = 0; i < rangeEvalMult-1 ; i++){
			qfitA.yData[i] = qfitA.yData[rangeEvalMult-1];
			qfitB.yData[i] = qfitB.yData[rangeEvalMult-1];
			qfitL.yData[i] = qfitL.yData[rangeEvalMult-1];
		}
		printStarted = true;
	}

	void start(bool useVision){
		controller->kill(1);
		controller->schedule(1, 0, 100, false, 0, Controller::newString("blink"), 'B', true);
		estHeading = -9999;
		cycleCount = 0;
		rangeSampleCount = 0;
		isMoving = true;
		currentThrottle = 0;
		prevEstSpeed = 0;
		prevEsts[rangeEvalMult - 1] = 0;
		m0 = 0; 
		m1 = 0;
		differential = 0;
		estSpeed = 0;
		estDist = 0;
		estHeadingVel = 0;
		estHeadingAccel = 0;
		
		prevAccelYData=0;
		qfitA.yData[rangeEvalMult-1]  = 0;
		qfitB.yData[rangeEvalMult-1]  = 0;
		qfitL.yData[rangeEvalMult-1]  = 0;
		runSensorTasks(useVision);

	}

	void runSensorTasks(bool useVision){
		if (useVision){
			echo->startScan(ECHO_ID, echoDelay, echoMult*baseInterval, echoABDelay, 0, false);
			lidar->startScan(LIDAR_ID, BASE_DELAY + lidarDelay, lidarMult*baseInterval, 0, true);
			controller->schedule(LIA_ID, BASE_DELAY + accelDelay, accelMult*baseInterval, false, 0, Controller::newString("LIA"), 'G', false);
		}
		controller->schedule(GYRO_ID,BASE_DELAY + gyroDelay,gyroMult*baseInterval,false,0,Controller::newString("H"),'G',false);
	}



	void stop(){
		isMoving = false;
		m0 = m1 = currentThrottle = 0;
		controller->kill(ECHO_ID);
		controller->kill(ECHO_ID+1);
		controller->kill(LIA_ID);
		controller->kill(GYRO_ID);
		controller->kill(LIDAR_ID);
		controller->kill(NAV_ID);
		motor->stopMotor();
		controller->kill(1);
		controller->schedule(1, 0, 1000, false, 0, Controller::newString("blink"), 'B', true);
	}

	

};


#endif