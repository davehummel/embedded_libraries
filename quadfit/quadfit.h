#ifndef _QUADFIT_H__
#define _QUADFIT_H__
#define REAL double


    inline static REAL sqr(REAL x) {
        return x*x;
    }


    int linreg(int n, const REAL x[], const REAL y[], REAL& m, REAL& b)
    {
        REAL   sumx = 0.0;                        /* sum of x                      */
        REAL   sumx2 = 0.0;                       /* sum of x**2                   */
        REAL   sumxy = 0.0;                       /* sum of x * y                  */
        REAL   sumy = 0.0;                        /* sum of y                      */
        REAL   sumy2 = 0.0;                       /* sum of y**2                   */

       for (int i=0;i<n;i++)   
          { 
          sumx  += x[i];       
          sumx2 += sqr(x[i]);  
          sumxy += x[i] * y[i];
          sumy  += y[i];      
          sumy2 += sqr(y[i]); 
          } 

       REAL denom = (n * sumx2 - sqr(sumx));
       if (denom == 0) {
           // singular matrix. can't solve the problem.
           m = 0;
           b = 0;
           return 1;
       }

       m = (n * sumxy  -  sumx * sumy) / denom;
       b = (sumy * sumx2  -  sumx * sumxy) / denom;
      

       return 0; 
    }
    
template<typename T>
class QuadFit{
public:

	~QuadFit(){
		if (temp0){

			delete[] temp0;
			delete[] temp1;
			delete[] temp2;
		}
	}



	void setSampleCount(const size_t count){
		samples = count;
		if (temp0){

			delete[] temp0;
			delete[] temp1;
			delete[] temp2;
		}

		temp0 = new T[count];
		temp1 = new T[count];
		temp2 = new T[count];

	}

	void processXData(){
	T transpose0[samples];
	T transpose1[samples];
	T transpose2[samples];
		for (uint16_t i = 0; i < samples ; i++){
		    T temp = xData[i];
		    transpose1[i] = temp;
		    sumx1 += temp;
		    temp*=xData[i];
		    transpose0[i] = temp;
		    sumx2 += temp;
		    temp*=xData[i];
		    sumx3 += temp;
		    temp*=xData[i];
            sumx4 += temp;

            transpose2[i] = 1;

		}
		det = (sumx4*sumx2*samples) + (sumx3*sumx1*sumx2) + (sumx2*sumx3*sumx1) - (sumx2*sumx2*sumx2) - (sumx1*sumx1*sumx4) - (samples*sumx3*sumx3);
		
		inverse [0][0] = samples*sumx2 - sumx1*sumx1;
		inverse [0][1] =  -(samples*sumx3 - sumx1*sumx2);
		inverse [0][2] = sumx1*sumx3-sumx2*sumx2;
		inverse [1][0] = -(samples*sumx3-sumx2*sumx1);
		inverse [1][1] =  samples*sumx4-sumx2*sumx2;
		inverse [1][2] =  -(sumx1*sumx4-sumx3*sumx2);
		inverse [2][0] = sumx1*sumx3 - sumx2*sumx2;
		inverse [2][1] = -(sumx1*sumx4 - sumx2*sumx3);
		inverse [2][2] =  sumx2*sumx4 - sumx3*sumx3;

		for (int j=0; j < samples; j++)
        {
            temp0[j] = inverse[0][0]*transpose0[j] + inverse[0][1]*transpose1[j] + inverse[0][2]*transpose2[j];
            temp1[j] = inverse[1][0]*transpose0[j] + inverse[1][1]*transpose1[j] + inverse[1][2]*transpose2[j];
            temp2[j] = inverse[2][0]*transpose0[j] + inverse[2][1]*transpose1[j] + inverse[2][2]*transpose2[j];
        }

        /*
		Serial1.print("sumx1 new=");
        Serial1.println(sumx1);
        Serial1.print("sumx2 new=");
        Serial1.println(sumx2);
        Serial1.print("sumx3 new=");
        Serial1.println(sumx3);
        Serial1.print("sumx4 new=");
        Serial1.println(sumx4);
		Serial1.print("det new=");
        Serial1.println(det);
        Serial1.println(inverse[0][1]);
        Serial1.println(inverse[1][2]);
        Serial1.println(temp0[1]);
        */
	}

	void solve(){
		c0 = 0;
		for (int j=0; j < samples; j++)
		{
		    c0 += temp0[j]*yData[j];
		}

		c0 /= det;

		c1 = 0;
		for (int j=0; j < samples; j++)
		{
		    c1 += temp1[j]*yData[j];
		}

		c1 /= det;

		c2 = 0;
		for (int j=0; j < samples; j++)
		{
		    c2 += temp2[j]*yData[j];
		}

		c2 /= det;
	}

	void calculate (){
		for ( size_t i = 0; i < samples; i++ )
		{
			approxYVals[i] = calculate(xData[i]);
		}
	}

	T calculate(T nX){
		
		return c2 + c1 *nX + c0*nX* nX;
	}

	T calculateD(T nX){
		return c1 + 2 * c0 * nX;
	}


	T *xData;
	T *yData;
	T *approxYVals;

	T c0,c1,c2;
private:
	T sumx4 = 0, sumx3 = 0, sumx2 = 0, sumx1 = 0, det;

	T *temp0;
	T *temp1;
	T *temp2;
	uint16_t samples;
	T inverse [3][3];
};


#endif