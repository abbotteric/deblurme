#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "EAPixel.h"
#include "fourier.h"
#include <fftw3.h>
#include "deblur.h"
#include <string.h>

void updateprogress(int current, int total, int width, char c)
{
	char bar[width];
	bar[0] = '[';
	bar[width-1] = ']';
	double cur = (double)current;
	double tot = (double)total;
	double wid = (double)width;
	int i; 
	for(i=1;i<width-1;i++)
	{
		if(cur/total < (double)i/wid)
			bar[i] = ' ';
		else
		{
			bar[i] = '>';
			if(i>1)	
				bar[i-1] = c;
		}
	}
	double percent = 100*(double)current/(double)total;
	printf("\r%s%3.0f%s",bar,percent+1,"%");
	fflush(stdout);
}


int main(int argc, char **argv)
{
	printf("\n");	
	if(argc != 3)
	{
	
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

	int k_max = 50;
	int n,m,k;
	int n_max = 4;
	int m_max = 3;

	psf = malloc(sizeof(double)*width*height);
	image_estimate = malloc(sizeof(double)*width*height);
	double *division = malloc(sizeof(double)*width*height);
	double *correlation  = malloc(sizeof(double)*width*height);
	double *image_norm = malloc(sizeof(double)*width*height);
	result = malloc(sizeof(double)*width*height);

	createInitialPSF(width,height,image,psf);	
	memcpy(image_estimate,image,sizeof(double)*width*height);
	/*convolve(width,height,image_estimate,psf,result);
	saveImage(width,height,"convolution.jpg",result,1.0);
	elementdivide(width,height,image,result,division);
	scaleto(width,height,image_estimate,result,0.0,1.0);	
	mirror(width,height,result,1,1);
	convolve(width,height,result,division,result);
	elementmultiply(width,height,result,psf,result);
	scaleto(width,height,result,result,0.0,(double)QuantumRange);
	saveImage(width,height,"test.jpg",result,1.0);*/
	char imgname[18];
	for(k=0;k<k_max;k++)
	{
		updateprogress(k,k_max,50,'=');
		for(n=0;n<n_max;n++)
		{
			convolve(width,height,image_estimate,psf,result);
			elementdivide(width,height,image,result,division);
			scaleto(width,height,image_estimate,result,0.0,1.0);
			scaleto(width,height,division,division,0.0,1.0);
			mirror(width,height,result,1,1);
			convolve(width,height,result,division,result);
			elementmultiply(width,height,result,psf,psf);
			scaleto(width,height,psf,psf,0.0,(double)QuantumRange);
			snprintf(imgname,8,"n%d.jpg",n);
			saveImage(width,height,imgname,psf,1.0);
		}
		for(m=0;m<m_max;m++)
		{
			convolve(width,height,image_estimate,psf,result);
			elementdivide(width,height,image,result,division);
			scaleto(width,height,psf,result,0.0,1.0);
			scaleto(width,height,division,division,0.0,1.0);
			mirror(width,height,result,1,1);
			convolve(width,height,result,division,result);
			elementmultiply(width,height,result,image_estimate,image_estimate);
			scaleto(width,height,image_estimate,image_estimate,0.0,(double)QuantumRange);
		}
		snprintf(imgname,7,"%d.jpg",k);
		saveImage(width,height,imgname,image_estimate,1.0);
	}
	saveImage(width,height,"output.jpg",image_estimate,1.0);
	saveImage(width,height,"outputpsf.jpg",psf,1.0);
	printf("\n");
	printf("Done!\n");
	return 0;
}
