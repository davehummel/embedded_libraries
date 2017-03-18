#ifndef DH_AVGMEASURE_H__
#define DH_AVGMEASURE_H__

#include <queue>
#include <stdint.h>
#include <arduino.h>

template<class _amt> 
class AveragedMeasure{

public:
	
	AveragedMeasure( uint32_t rewindWindowDur );

	void addMeasure(_amt measure, uint32_t time);

	bool getReady(){
		return isReady;
	}

	_amt getValue(){
		return value;
	}

	void shiftQueues();

	void printDebugStatus(){
		Serial1.print("Value:");
		Serial1.print(value);

		Serial1.print(" Sum:");
		Serial1.println(sum);

		Serial1.print("Front Queue (");
		Serial1.print(frontQueue.size());


		printQueue(frontQueue);

		Serial1.print("Back Queue (");
		Serial1.print(backQueue.size());
		printQueue(backQueue);
	}

	uint16_t MAX_QUEUE_SIZE = 200;

private:

	_amt value;

	_amt sum;
	
	uint32_t lastWriteTime = 0; 

	typedef struct vector_am{
		_amt val=0;
		uint32_t time=0;
	}amEntry;

	void processQueue();

	void updateValue();

	bool isReady = false;

	std::queue<amEntry*> frontQueue;
	std::queue<amEntry*> backQueue;

	uint32_t windowDur;

	void printQueue(std::queue<amEntry*> input){
		Serial1.println(") [");
		// std::queue<amEntry*> myCopy(input);
		// int i = 0;
		// while (!myCopy.empty()){
		// 	Serial1.print(i);
		// 	Serial1.print(" in @ ");
		// 	Serial1.print(myCopy.front()->time);
		// 	Serial1.print("ms = ");
		// 	Serial1.println(myCopy.front()->val);
		// 	myCopy.pop();
		// 	i++;
		// }
		Serial1.println("]");
	}
};

	
template<class _amt> 
AveragedMeasure<_amt>::AveragedMeasure( uint32_t rewindWindowDur){
	if (rewindWindowDur<1)
		rewindWindowDur = 1;
	windowDur = rewindWindowDur;
	value = 0;
	sum = 0;
}

template<class _amt> 
void AveragedMeasure<_amt>::addMeasure(_amt measure, uint32_t time){
	if (time < lastWriteTime)
		return;
	lastWriteTime = time;
	amEntry* entry = new amEntry();
	if (!entry){
		Serial.println("DEADDD!");
		return;
	}
	entry->val = measure;
	entry->time = time;
	frontQueue.push(entry);
	processQueue();
}

	

// private:
template<class _amt> 
void AveragedMeasure<_amt>::processQueue(){
	bool recalc = false;

  	amEntry* val;

  	if (!frontQueue.empty()){

		while (lastWriteTime - (val=frontQueue.front())->time >= windowDur){
			frontQueue.pop();
			sum=sum+val->val;
			backQueue.push(val);
			recalc = true;
		}

	}

	if (backQueue.empty())
		return;

	uint32_t backQueueTime = backQueue.back()->time;
	while (backQueueTime - (val=backQueue.front()) -> time >= windowDur){
		sum=sum-val->val;
		backQueue.pop();
		delete val;
		recalc = true;
		isReady = true;
	}

	if (recalc)
		updateValue();

}

template<class _amt> 
void AveragedMeasure<_amt>::updateValue(){
	int16_t size = backQueue.size();
	value = sum / size;
}

template<class _amt>
void AveragedMeasure<_amt>::shiftQueues(){
	if (!isReady)
		return;

	while(!frontQueue.empty()){
		amEntry* val = frontQueue.front();
		sum=sum+val->val;
		frontQueue.pop();
		backQueue.push(val);
	}

	processQueue();
}


#endif