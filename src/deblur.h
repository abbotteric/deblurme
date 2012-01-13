#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "fourier.h"
#include <fftw3.h>

typedef struct _Pixel
{
	double red;
	double green;
	double blue;
}Pixel;

int convolve(int width, int height, double *image, double *point_spread, double *result);
double * openImage(const char *filename, long *width, long *height);
void saveImage(int width, int height, const char *filename, double *data, float scaler);
