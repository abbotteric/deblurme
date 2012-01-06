#include <math.h>
#include <stdlib.h>

int fkl(int k, int l, int width, int height, double *real_values, double *imag_values, double *F_real, double *F_imag);

int ift(int width, int height, double *real_values, double *imag_values, double *F_r);

int dft(long length, double input_real[], double input_imag[]);
