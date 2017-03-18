
#ifndef DH_MOVEMENTEVAL_H__
#define DH_MOVEMENTEVAL_H__

#include <dh_logger.h>
#include <queue>


class MovementEval{
public:


	MovementEval(double _gNetLimit=.4,double _gSingleLimit=.25, double _gStopRange=.25, uint32_t _window=100, uint32_t _resolution=20){
		gNetLimit = _gNetLimit;
		gSingleLimit = _gSingleLimit;
		gStopRange = _gStopRange;
		stopReqTime = _window;
		window = _window/_resolution;
		resolution = _resolution;
	}

	// return true if this evauation triggered a change in movement state
	bool evaluateGyro (double gx, double gy, double gz, uint32_t time); 

	bool evaluateAcl (float a[3], uint32_t time);

	// Forces Evaluator into set state 
	bool overrideEvaluation(bool moving, bool _overrideActive){
		bool prevState = getMoving();
		isOverrideMoving = moving;
		isOverriding = _overrideActive;
		bool curState = getMoving();
		return curState!=prevState;
	}

	bool getMoving(){
		if (isOverriding){
			if (logger)
				logger->print("OVERRIDING!\n");
			return isOverrideMoving;
		}

		return isGyroMoving||isAccelMoving;
	}

	void printState(){
		if (logger){
			logger->print("Movement : ");
			logger->print((getMoving()?"Is Moving":"Is Still"));
			logger->println();
			if (getMoving()){
				logger->print(" With in Variance since ");
				logger->print(stopCheckStartTime);
				logger->println();
				logger->print("Bounds: X=");
				logger->print(gOffsetXMin);
				logger->print(" to "); 
				logger->print(gOffsetXMax);
				logger->print("Bounds: Y=");
				logger->print(gOffsetYMin);
				logger->print(" to ");
				logger->print(gOffsetYMax);
				logger->print("Bounds: Z=");
				logger->print(gOffsetZMin);
				logger->print(" to ");
				logger->print(gOffsetZMax);
				logger->println();
			} else {
				logger->print ("Net : ");
				logger->print (gNetX);
				logger->print (",");
				logger->print (gNetY);
				logger->print (",");
				logger->print (gNetZ);
				logger->println();
			}
		}
	}

	double gNetLimit;
	double gSingleLimit;
	double aNetLimit;
	double aSingleLimit;

	
	double gNetX=0;
	double gNetY=0;
	double gNetZ=0;

	double aNetX =0;
	double aNetY =0;
	double aNetZ =0;


	double gOffsetXMin=10000;
	double gOffsetXMax=-10000;
	double gOffsetYMin=10000;
	double gOffsetYMax=-10000;
	double gOffsetZMin=10000;
	double gOffsetZMax=-10000;


	Logger *logger = 0;

private:

	typedef struct {
		float gX=0;
		float gY=0;
		float gZ=0;

		float aX=0;
		float aY=0;
		float aZ=0;

		uint32_t time =0;
	} mEntry;

	void balanceQueue(uint32_t time);

	void clearResidue(uint32_t time);

	void updateGMinMax(double gx,double gy, double gz, uint32_t time){
		readings ++;
		if (gx>gOffsetXMax)
			gOffsetXMax = gx;
		if (gx<gOffsetXMin)
			gOffsetXMin = gx;

		if (gy>gOffsetYMax)
			gOffsetYMax = gy;
		if (gy<gOffsetYMin)
			gOffsetYMin = gy;

		if (gz>gOffsetZMax)
			gOffsetZMax = gz;
		if (gz<gOffsetZMin)
			gOffsetZMin = gz;

		if (gOffsetXMax - gOffsetXMin > gStopRange){
			restartStillTest(time);
			return;
		}

		if (gOffsetYMax - gOffsetYMin > gStopRange){
			restartStillTest(time);
			return;
		}

		if (gOffsetZMax - gOffsetZMin > gStopRange){
			restartStillTest(time);
			return;
		}
		
	}

	void beginStillTest(uint32_t time);
	void restartStillTest(uint32_t time);


	uint32_t window;
	uint32_t resolution;
	uint32_t stopReqTime;

	double gStopRange = 0;
	double stopCheckStartTime = 0;

	uint16_t readings = 0;


	bool isGyroMoving=false;
	bool isAccelMoving=false;
	bool isOverrideMoving=false;
	bool isOverriding=false;

	uint32_t lastUpdateTime=0;

	std::queue<mEntry*> queue;

};

#endif