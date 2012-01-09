#include "EAComplex.h"

void add(fftw_complex a, fftw_complex b, fftw_complex out)
{
	out[0]= a[0] + b[0];
	out[1] = a[1] + b[1];
}

void subtract(fftw_complex a, fftw_complex b, fftw_complex out)
{
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
}

void multiply(fftw_complex a, fftw_complex b, fftw_complex out)
{
	out[0] = (a[0]*b[0] - a[1]*b[1]);
	out[1] = (a[1]*b[0] - a[0]*b[1]);
}

void divide(fftw_complex a, fftw_complex b, fftw_complex out)
{
	out[0] = b[0]/a[0];
	out[1] = b[1] - a[1];
}
