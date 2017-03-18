
#ifndef DH_SENSOR_PROC_H__
#define DH_SENSOR_PROC_H__


#include <dh_logger.h>
#include <dh_movement.h>
#include <dh_position.h>
#include <dh_averaged_measure.h>

class SensorProcessor{
public:

	SensorProcessor(uint32_t _window){
		gTrim[0]=new AveragedMeasure<int32_t>(_window);
		gTrim[1]=new AveragedMeasure<int32_t>(_window);
		gTrim[2]=new AveragedMeasure<int32_t>(_window);

		gTrimValue[0] = 0;
		gTrimValue[1] = 0;
		gTrimValue[2] = 0;

		window = _window;
	}

	bool getReady(){
		return gTrim[0]->getReady();
	}

	void processGyro(int16_t gx,int16_t gy,int16_t gz, uint32_t time){
		if (time<lastTime)
			return;

		bool movingNow = false;

		if (movement->getMoving()){
			gTrim[0]->addMeasure(gx,time);
			gTrim[1]->addMeasure(gy,time);
			gTrim[2]->addMeasure(gz,time);

			dpsX = (gx-gTrimValue[0])*DPSXFACTOR;
			dpsY = (gy-gTrimValue[1])*DPSYFACTOR;
			dpsZ = (gz-gTrimValue[2])*DPSZFACTOR;
			
			bool stateChange = movement->evaluateGyro(dpsX,dpsY,dpsZ,time);

			if (stateChange){
				revertState();
			}else{
				movingNow =true;
			}

		}else{
			gTrim[0]->addMeasure(gx,time);
			gTrim[1]->addMeasure(gy,time);
			gTrim[2]->addMeasure(gz,time);

			gTrimValue[0] = gTrim[0]->getValue();
			gTrimValue[1] = gTrim[1]->getValue();
			gTrimValue[2] = gTrim[2]->getValue();

	

			if (gTrim[0]->getReady()){

			dpsX = (gx-gTrimValue[0])*DPSXFACTOR;
			dpsY = (gy-gTrimValue[1])*DPSYFACTOR;
			dpsZ = (gz-gTrimValue[2])*DPSZFACTOR;

				bool stateChange = movement->evaluateGyro(dpsX,dpsY,dpsZ,time);

				if (stateChange){
				//	replayWindow();
					movingNow = true;
				}

			}else{
				if (logger){
					logger->print("Not yet Ready at ");
					logger->print(time);
					logger->println();
				}
			}
		}

		if (movingNow){
				uint32_t dif = time-lastTime;

		

				oldHeading.push(netZ);
				oldHeadingTime.push(lastTime);

				if (oldHeadingTime.front()<time - window){
					oldHeadingTime.pop();
					oldHeading.pop();
				}

				netX+=dpsX*(dif/1000.0L);
				netY+=dpsY*(dif/1000.0L);
				netZ+=dpsZ*(dif/1000.0L);
		}

		lastTime = time;
	}

	

	void setExternalMoveIndivator(bool isMoving,bool overrideActive){
		if (movement->overrideEvaluation(isMoving,overrideActive)){
			if (isMoving = movement->getMoving()){
				if (logger){
					logger->print("Started moving\n");
				}
			//	replayWindow();
			}else{
				if (logger){
					logger->print("Stopped moving\n");
				}
				//position->revertState();
			}
		}
	}

	void printState(){
		if (logger){
			logger->print("Sensor Processor:");
			logger->print(isMoving?"moving @":"still @");
			logger->print(lastTime);
			logger->println();
			if (gTrim[0]->getReady()){
				logger->print("GTrim Values : ");
				logger->print(gTrimValue[0]);
				logger->print(",");
				logger->print(gTrimValue[1]);
				logger->print(",");
				logger->print(gTrimValue[2]);
				logger->println();
			} else {
				logger->print("Not yet Ready - adjusting trim\n");
			}

			logger->print("RollBackQueue Sizes : ");
			logger->print((uint16_t)oldHeading.size());
			logger->print(",");
			logger->print((uint16_t)oldHeadingTime.size());
			logger->println();

			logger->print(" XTRIM: ");
			gTrim[0]->printDebugStatus();
			logger->print(" YTRIM: ");
			gTrim[1]->printDebugStatus();
			logger->print(" ZTRIM: ");
			gTrim[2]->printDebugStatus();

			if (movement)
				movement->printState();
		}
	}

	Logger *logger=0;
	MovementEval *movement;
	PositionState* position;

	bool isMoving=false;

	double DPSXFACTOR = 560.0l / 32768.0l;
	double DPSYFACTOR = 560.0l / 32768.0l;
	double DPSZFACTOR = 560.0l / 32768.0l;

	int32_t gTrimValue[3];

	double dpsX=0,dpsY=0,dpsZ=0;

	double netX=0,netY=0,netZ=0;

	std::queue<double> oldHeading;
	std::queue<uint32_t> oldHeadingTime;

private:
	uint32_t lastTime = 0;

	uint32_t window = 0;

	AveragedMeasure<int32_t> *gTrim[3];

	void revertState(){
		if (!oldHeading.empty()){
				if (logger){
					logger->print("Reverting heading from ");
					logger->print(netZ);
					logger->print(" to ");
					logger->print(oldHeading.front());
					logger->println();
				}
			netZ = oldHeading.front();
			while(!oldHeading.empty())
				oldHeading.pop();
			while(!oldHeadingTime.empty())
				oldHeadingTime.pop();
		}
		
	}

};




#endif