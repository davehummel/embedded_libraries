#include "dh_movement.h"
#include <dh_logger.h>
#include <queue>
#include <arduino.h>

bool MovementEval::evaluateGyro(double gx,double gy, double gz, uint32_t time){
		if (isGyroMoving){
	
			updateGMinMax(gx,gy,gz,time);

			if (time - stopCheckStartTime > stopReqTime){
				bool output = getMoving();
				isGyroMoving = false;

				if (logger){
					logger->print("Gyro stayed within range for required time ");
					logger->print(stopReqTime);
					logger->print(" in ");
					logger->print(readings);
					readings = 0;
					logger->println();
				}
				return true;
			}

			return false;
		}

		balanceQueue(time);

		mEntry* entry = queue.back();

		
		entry->gX+=gx;
		entry->gY+=gy;
		entry->gZ+=gz;

		gNetX += gx;
		gNetY += gy;
		gNetZ += gz;

	
		bool output = !getMoving();
	
		if ((gx>0?gx:-gx)>gSingleLimit){
			if (logger){
				logger->print("Movement detected on X in sudden move");
				logger->print(gx);
				logger->println();
			}
			isGyroMoving = true;
			beginStillTest(time);
			return output;
		}

		if ((gy>0?gy:-gy)>gSingleLimit){
			if (logger){
				logger->print("Movement detected on Y in sudden move");
				logger->print(gy);
				logger->println();
			}
		
			isGyroMoving = true;
			beginStillTest(time);
			return output;
		}

		if ((gz>0?gz:-gz)>gSingleLimit){
			if (logger){
				logger->print("Movement detected on Z in sudden move");
				logger->print(gz);
				logger->println();
			}
			isGyroMoving = true;
			beginStillTest(time);
			return output;
		}


		if ((gNetX>0?gNetX:-gNetX)>gNetLimit){
			if (logger){
				logger->print("Movement detected on X in net movement at ");
				logger->print(gNetX);
				logger->println();
			}
			isGyroMoving = true;
			beginStillTest(time);
			return output;
		}
	
		if ((gNetY>0?gNetY:-gNetY)>gNetLimit){
			if (logger){
				logger->print("Movement detected on Y in net movement at ");
				logger->print(gNetY);
				logger->println();
			}
			isGyroMoving = true;
			beginStillTest(time);
			return output;
		}

		if ((gNetZ>0?gNetZ:-gNetZ)>gNetLimit){
			if (logger){
				logger->print("Movement detected on Z in net movement at ");
				logger->print(gNetZ);
				logger->println();
			}
			isGyroMoving = true;
			beginStillTest(time);
			return output;
		}

		return false;

}

void MovementEval::balanceQueue(uint32_t time){
	uint32_t roundedTime = time/resolution;
	if (lastUpdateTime == 0) 
		lastUpdateTime = roundedTime;
	uint16_t dif = roundedTime - lastUpdateTime;
	if (dif == 0){
		if (queue.empty())	{
			mEntry* entry = new mEntry();
				if (!entry){
		Serial.println("dead");
	}
			entry->time = roundedTime;
			queue.push(entry);
		}
		return;
	}

	if (queue.empty())	{
		mEntry* entry = new mEntry();
			if (!entry){
		Serial.println("dead");
	}
		entry->time = roundedTime;
		queue.push(entry);
	}

	while (dif>window){
		mEntry* entry = queue.front();
		gNetX-=entry->gX;
		gNetY-=entry->gY;
		gNetZ-=entry->gZ;

		aNetX-=entry->aX;
		aNetY-=entry->aY;
		aNetZ-=entry->aZ;
		
		delete entry;

		queue.pop();


		if (queue.empty()){
			lastUpdateTime = roundedTime;
			dif = 0;
		}else{
			lastUpdateTime = queue.front()->time;
			dif = roundedTime - lastUpdateTime;
		}

	}

	mEntry* entry = new mEntry();
	if (!entry){
		Serial.println("dead");
	}
	entry->time = roundedTime;
	queue.push(entry);

}

void MovementEval::beginStillTest(uint32_t time){

	gNetX = gNetY = gNetZ = 0;
	readings = 0;

	while (!queue.empty()){
		mEntry* entry =  queue.front();
		queue.pop();
		delete entry;
	}

	 restartStillTest(time);
}

void MovementEval::restartStillTest(uint32_t time){
		 gOffsetXMin=10000;
		 gOffsetXMax=-10000;
		 gOffsetYMin=10000;
		 gOffsetYMax=-10000;
		 gOffsetZMin=10000;
		 gOffsetZMax=-10000;
		 stopCheckStartTime = time;
		 readings = 0;
}
