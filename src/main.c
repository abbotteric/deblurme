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
	if(argc == 4)
	{
		if(!strcmp(argv[3],"c")) 
		{
			psf = openImage(argv[2],&width,&height);
			
			result = malloc(sizeof(double)*width*height);
			roll(width,height,psf,width/2,height/2);
			convolve(width,height,image,psf,result);
			saveImage(width,height,"convolved.jpg",result,1.0);
			return;
		}
		else if(!strcmp(argv[3],"d"))
		{
			psf = openImage(argv[2], &width, &height);
			result = malloc(sizeof(double)*width*height);
			roll(width,height,psf,width/2,height/2);
			deconvolve(width,height,image,psf,result);
			saveImage(width,height,"deconvolved.jpg",result,1.0);
			return;
		}
		else
		{
			printf("Unknown option: %s\nQuitting...\n\n",argv[3]);
			return;
		}
	}

	int k_max = 20;
	int n,m,k;
	int n_max = 4;
	int m_max = 1;

	psf = malloc(sizeof(double)*width*height);
	image_estimate = malloc(sizeof(double)*width*height);
	double *division = malloc(sizeof(double)*width*height);
	double *correlation  = malloc(sizeof(double)*width*height);
	double *image_norm = malloc(sizeof(double)*width*height);
	result = malloc(sizeof(double)*width*height);
	
	createInitialPSF(width,height,image,psf);	
	memcpy(image_estimate,image,sizeof(double)*width*height);
	char imgname[18];
	long w,h;
	w = width;
	h = height;
	for(k=0;k<k_max;k++)
	{
		updateprogress(k,k_max,50,'=');
		for(n=0;n<n_max;n++)
		{
			normalize(w,h,psf,psf);
			convolve(w,h,psf,image_estimate,result);
			elementdivide(w,h,image,result,division);
			memcpy(result,image_estimate,sizeof(double)*width*height);
			mirror(w,h,result,1,1);
			convolve(w,h,division,result,correlation);
			elementmultiply(w,h,correlation,psf,psf);
		}
		normalize(w,h,psf,psf);
		scaleto(w,h,psf,result,0.0,(double)QuantumRange);
		snprintf(imgname,16,"k%d.jpg",k);
		saveImage(w,h,imgname,result,1.0);
		for(m=0;m<m_max;m++)
		{
			convolve(w,h,image_estimate,psf,result);
			elementdivide(w,h,image,result,division);
			memcpy(result,psf,sizeof(double)*width*height);
			mirror(w,h,result,1,1);
			convolve(w,h,division,result,correlation);
			elementmultiply(w,h,correlation,image_estimate,image_estimate);
			snprintf(imgname,16,"i%d-%d.jpg",k,m);
			saveImage(w,h,imgname,image_estimate,1.0);	
		}
		snprintf(imgname,7,"%d.jpg",k);
		scaleto(w,h,image_estimate,image_estimate,0.0,(double)QuantumRange);
		saveImage(width,height,imgname,image_estimate,1.0);
	}
	saveImage(width,height,"output.jpg",image_estimate,1.0);
	saveImage(width,height,"outputpsf.jpg",psf,1.0);
	elementmultiply(w,h,result,image_estimate,image_norm);
	convolve(w,h,psf,image_estimate,result);
	elementdivide(w,h,image,result,result);
	mirror(w,h,result,1,1);
	convolve(w,h,image_norm,result,image_estimate);;
	printf("\n");
	printf("Done!\n");
	return 0;
}
