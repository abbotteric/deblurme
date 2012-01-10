#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

void EAadd(fftw_complex a, fftw_complex b, fftw_complex out);
void EAsubtract(fftw_complex a, fftw_complex b, fftw_complex out);
void EAmultiply(fftw_complex a, fftw_complex b, fftw_complex *out);
void EAdivide(fftw_complex a, fftw_complex b, fftw_complex out);
void EAnormalize(fftw_complex *in);
