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

