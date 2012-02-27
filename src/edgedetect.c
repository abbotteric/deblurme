#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "EAPixel.h"
#include "fourier.h"
#include <fftw3.h>
#include "deblur.h"
#include <string.h>

void edgedetect(long w, long h, double *data, double *output)
{
	double *detector = malloc(sizeof(double)*w*h);
	//if (output == NULL)
	output = malloc(sizeof(double)*w*h);
	
	int i,j;
	for(i=0;i<h;i++)
	{
		for(j=0;j<w;j++)
		{
			detector[i*w+j] = 0.0;
			data[i*w+j] /= (double)QuantumRange;
			data[i*w+j] -= 0.5;
		}
	}
		
	long mid_x, mid_y;
	mid_x = w/2;
	mid_y = h/2;
	printf("%d,%d\n",mid_x,mid_y);
	//fill in the detector matrix
	detector[(mid_y-1)*w+mid_x] = 2.0/12.0;
	detector[(mid_y-1)*w+(mid_x+1)] = 1.0/12.0;
	detector[(mid_y)*w+mid_x] = -2.0/12.0;
	detector[mid_y*w+(mid_x+1)] = -1.0/12.0;
	detector[mid_x-1+(mid_y-1)*w] = 2.0/12.0;
	detector[mid_x-2+(mid_y-1)*w] = 1.0/12.0;
	detector[mid_x-1+(mid_y)*w] = -2.0/12.0;
	detector[mid_x-2+mid_y*w] = -1.0/12.0;
	
	//saveImage(w,h,"psf.png",detector,12*(double)QuantumRange);
	
	//roll(w,h,detector,w/2,h/2);
	
	convolve(w,h,data,detector,output);
	
	
	
	for(i = 0; i < h; i++)
	{
		for(j=0;j<w;j++);
		{
			output[i*w+j] += 0.5;
			output[i*w+j] *= (double)QuantumRange;
			if(output[i*w+j] >= 1.0)
			{
				//printf("%d,%d %5.2f\n",j,i,output[i*w+j]);
			}
		}
	}
	
	saveImage(w,h,"edge.png",output,1);
}