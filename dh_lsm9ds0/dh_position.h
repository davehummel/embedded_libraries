
#ifndef DH_POSITION_H__
#define DH_POSITION_H__

#include <dh_logger.h>
#include <math.h>
#include <quaternion.h>


#define PI_FACT 3.14159265/180

class PositionState{
public:

	typedef struct {
		float xRot = 0;
		float yRot = 0;
		float zRot = 0;

		int32_t x = 0;
		int32_t y = 0;
		int32_t z = 0;
	} PSposition;

// TODO Primitive first version
	void inputGyro (float gx, float gy,float gz, uint16_t duration,bool firstMove){
		float deltaX,deltaY,deltaZ;
		if (firstMove){
			deltaX=(gx*duration*factorX)/(divider);
			deltaY=(gy*duration*factorY)/(divider);
			deltaZ=(gz*duration*factorZ)/(divider);
		}else{
			deltaX=((prevXRotVel+gx)*duration*factorX)/(divider2);
			deltaY=((prevYRotVel+gy)*duration*factorY)/(divider2);
			deltaZ=((prevZRotVel+gz)*duration*factorZ)/(divider2);
		}

		xRot +=deltaX;
		yRot +=deltaY;
		zRot +=deltaZ;



		// TODO this is alternate calculation
		double axisVec[3];
	    if (firstMove){
	    	axisVec[0]=gx*factorX;
			axisVec[1]=gy*factorY;
			axisVec[2]=gz*factorZ;
	    }else{
			axisVec[0]=(gx+prevXRotVel)*factorX/2.0;
			axisVec[1]=(gy+prevYRotVel)*factorY/2.0;
			axisVec[2]=(gz+prevZRotVel)*factorZ/2.0;
		}

		double durFactor = duration/divider;

		axisVec[0]*=durFactor;
		axisVec[1]*=durFactor;
		axisVec[2]*=durFactor;

		Quaternion<double> gyroQ(axisVec[0],axisVec[1],axisVec[2]); 
		rotation=gyroQ*rotation;

		// THis must be last
		prevXRotVel = gx;
		prevYRotVel = gy;
		prevZRotVel = gz;
	}


	void inputAcl (float ax,float ay, float gz, uint16_t duration, bool firstMove);

	PSposition getPosition(){
		PSposition position =  PSposition();
		position.xRot = xRot;
		position.yRot = yRot;
		position.zRot = zRot;
		return position;
	}

	void getHeading(double val[3]){
	 val[0] = 0;
	 val[1] = 1;
	 val[2] = 0;
	rotation.QuatRotation(val);
	Serial1.print(val[0],6);
	Serial1.print(",");
	Serial1.print(val[1],6);
	Serial1.print(",");
	Serial1.println(val[2],6);

		// rotation.getEuler(val);

		// return val[2];
		//rotation = rotation.UnitQuaternion();

		//return  asin(2*rotation.x*rotation.y + 2*rotation.z*rotation.w);
	}

	void printState(){
		
	}


	Logger *logger;

private:
     //TODO primitive disconnected dimensions version
	 float prevXRotVel=0,prevYRotVel =0,prevZRotVel = 0;
     float xRot=0,yRot=0,zRot = 0; 

     Quaternion<double> rotation;

     const double divider = 1000000.0;
     const double divider2 = 2000000.0;

     const double factorX = 1.135; 
     const double factorY = 1.135; 
     const double factorZ = 1.135; 

};

#endif