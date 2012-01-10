#include "EAComplex.h"

void EAadd(fftw_complex a, fftw_complex b, fftw_complex out)
{
	out[0]= a[0] + b[0];
	out[1] = a[1] + b[1];
}

void EAsubtract(fftw_complex a, fftw_complex b, fftw_complex out)
{
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
}

void EAmultiply(fftw_complex a, fftw_complex b, fftw_complex *out)
{
	(*out)[0] = (a[0]*b[0] - a[1]*b[1]);
	(*out)[1] = (a[1]*b[0] + a[0]*b[1]);
}

void EAdivide(fftw_complex a, fftw_complex b, fftw_complex *out)
{
	double mag = b[0]*b[0] + b[1]*b[1];
	if(mag<0.0005)
		mag = 0.0005;
	(*out)[0] = (a[0]*b[0] + a[1]*b[1])/mag;
	(*out)[1] = (a[1]*b[0] - a[0]*b[1])/mag;
}

void EAnormalize(fftw_complex *in)
{
	fftw_complex swap;
	swap[0] = (*in)[0];
	swap[1] = (*in)[1];
	double magnitude = sqrt(swap[0]*swap[0] + swap[1]*swap[1]);
	if(magnitude == 0.0)
	{
		(*in)[0] = 0.0;
		(*in)[1] = 0.0;
		return;
	}
	(*in)[0] = swap[0]/magnitude;
	(*in)[1] = -swap[1]/magnitude;
}

/*int main()
{
	fftw_complex test;
	test[0] = 50;
	test[1] = 12;
	printf("%0.4f + %0.4fi\n",test[0],test[1]);
	EAnormalize(&test);
	printf("%0.4f + %0.4fi\n",test[0],test[1]);
}*/
