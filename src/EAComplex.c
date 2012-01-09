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

void EAmultiply(fftw_complex a, fftw_complex b, fftw_complex out)
{
	out[0] = (a[0]*b[0] - a[1]*b[1]);
	out[1] = (a[1]*b[0] - a[0]*b[1]);
}

void EAdivide(fftw_complex a, fftw_complex b, fftw_complex out)
{
	out[0] = ((a[0]*b[0] + a[1]*b[1])/(a[0]*a[0] + a[1]*a[1]));
	out[1] = ((a[0]*b[1] - a[1]*b[0])/(a[0]*a[0] + a[1]*a[1]));
}
