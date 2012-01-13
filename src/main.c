#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "EAPixel.h"
#include "fourier.h"
#include <fftw3.h>
#include "deblur.h"
#include <string.h>

int main(int argc, char **argv)
{
	if(argc != 3)
	{
//		printf("\nUse: %s <image file> <filter image>\n\n",argv[0]);
//		return -1;
	}

	long x,y;

	double *image, *psf, *result, *image_estimate;
	long width, height;
	image = openImage(argv[1],&width,&height);
	if(argc == 3)
	{
		psf = openImage(argv[2],&width,&height);
		
		result = malloc(sizeof(double)*width*height);
		roll(width,height,psf,width/2,height/2);
		convolve(width,height,image,psf,result);
		saveImage(width,height,"convolved.jpg",result,1.0);
		return;
	}
	int k_max = 10;
	int n,m,k;
	int n_max = 4;
	int m_max = 2;

	psf = malloc(sizeof(double)*width*height);
	image_estimate = malloc(sizeof(double)*width*height);

	createInitialPSF(width,height,image,psf);	
	memcpy(image_estimate,image,sizeof(double)*width*height);
	char imgname[16];	
	for(k=0;k<k_max;k++)
	{
		printf("k = %d\n",k);	
		for(n=0;n<n_max;n++)
		{
			printf("n = %d\n",n);	
			iteratePSF(width,height,image_estimate,psf,image);
		}
		for(m=0;m<m_max;m++)
		{
			printf("m = %d\n",m);	
			iterateImage(width,height,image_estimate,psf,image);
		}
		snprintf(imgname,6,"%d.jpg",k);
		saveImage(width,height,imgname,image_estimate,1.0);
	}
	saveImage(width,height,"output.jpg",image_estimate,1.0);
	return 0;
}
