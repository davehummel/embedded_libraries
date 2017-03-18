#ifndef DH_QUADREG_H__
#define DH_QUADREG_H__
#include <math.h>

void quadraticRegression(double &a, double &b, double &c, uint16_t size, double *xdata, double *ydata){
	double a0=size,a1=0,a2=0,a3=0,a4=0;
	double b0=0,b1=0,b2=0;
	for (int i = 0 ; i < size ; i ++){
		double y = ydata[i];
		b0 += y;
		double x = xdata==0 ? i : xdata[i];
		Serial1.print(x);
		Serial1.print(',');
		Serial1.println(y);
		a1+=x;
		b1 += x*y;
		x *=x;
		a2+=x;
		b2+=x*y;
		x*=x;
		a3+=x;
		x*=x;
		a4+=x;
	}

	double a2Cubed = a2*a2*a2;

	a = (b0*a1*a3 - b1*a0*a3 - b0*a2*a2
       + b1*a1*a2 + b2*a0*a2 - b2*a1*a1)
    /(a0*a2*a4 - a1*a1*a4 - a0*a3*a3 + 2*a1*a2*a3 - a2Cubed);

    b = (b1*a0*a4 - b0*a1*a4 + b0*a2*a3
       - b2*a0*a3 - b1*a2*a2 + b2*a1*a2)
    /(a0*a2*a4 - a1*a1*a4 - a0*a3*a3 + 2*a1*a2*a3 - a2Cubed);

    c  =(b0*a2*a4 - b1*a1*a4 - b0*a3*a3
       + b1*a2*a3 + b2*a1*a3 - b2*a2*a2)
    /(a0*a2*a4 - a1*a1*a4 - a0*a3*a3 + 2*a1*a2*a3 - a2Cubed);

}



#endif