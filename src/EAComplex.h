#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

void add(fftw_complex a, fftw_complex b, fftw_complex out);
void subtract(fftw_complex a, fftw_complex b, fftw_complex out);
void multiply(fftw_complex a, fftw_complex b, fftw_complex out);
void divide(fftw_complex a, fftw_complex b, fftw_complex out);
