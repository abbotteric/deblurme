#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "fourier.h"
#include <fftw3.h>
#include "deblur.h"
#include <string.h>
#include <assert.h>

int convolve(int width, int height, double *image, double *point_spread, double *result)
{
	long x,y;

	fftw_complex *img, *psf, *out;
	fftw_plan img_fft, psf_fft, out_fft;
	img = (fftw_complex *)fftw_alloc_complex(width*height);
	psf = (fftw_complex *)fftw_alloc_complex(width*height);
	out = (fftw_complex *)fftw_alloc_complex(width*height);

	img_fft = fftw_plan_dft_2d(height,width,img,img,FFTW_FORWARD,FFTW_MEASURE);
	psf_fft = fftw_plan_dft_2d(height,width,psf,psf,FFTW_FORWARD,FFTW_MEASURE);
	out_fft = fftw_plan_dft_2d(height,width,out,out,FFTW_BACKWARD,FFTW_MEASURE);

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			img[i][0] = image[i];
			img[i][1] = 0.0;
			psf[i][0] = point_spread[i];
			psf[i][1] = 0.0;
		}
	}

	fftw_execute(img_fft);
	fftw_execute(psf_fft);

	double normalizer = (double)width*(double)height;
	double psf_max_real = 0.0;
	double psf_max_imag = 0.0;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			img[i][0] = img[i][0]/normalizer;
			img[i][1] = img[i][1]/normalizer;
			psf[i][0] = psf[i][0]/normalizer;
			psf[i][1] = psf[i][1]/normalizer;

			if(psf[i][0] > psf_max_real)
			{
				psf_max_real = psf[i][0];
			}
			if(psf[i][1] > psf_max_imag)
			{
				psf_max_imag = psf[i][1];
			}
			if(psf[i][0] < 0)
				psf[i][0] = 0.0;
			if(psf[i][1] < 0)
				psf[i][1] = 0.0;
		}
	}

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			psf[i][0] = psf[i][0]/psf_max_real;
			psf[i][1] = psf[i][1]/psf_max_imag;
		}
	}

	//do the convolution (multiplication)
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			EAmultiply(img[i],psf[i],&out[i]);
		}
	}

	fftw_execute(out_fft);
	
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			result[i] = out[i][0];
		}
	}
	fftw_destroy_plan(out_fft);
	fftw_destroy_plan(img_fft);
	fftw_destroy_plan(psf_fft);
	fftw_free(img);
	fftw_free(out);
	fftw_free(psf);
	return 1;
}

int deconvolve(int width, int height, double *image, double *point_spread, double *result)
{
	long x,y;

	fftw_complex *img, *psf, *out;
	fftw_plan img_fft, psf_fft, out_fft;
	img = (fftw_complex *)fftw_alloc_complex(width*height);
	psf = (fftw_complex *)fftw_alloc_complex(width*height);
	out = (fftw_complex *)fftw_alloc_complex(width*height);

	img_fft = fftw_plan_dft_2d(height,width,img,img,FFTW_FORWARD,FFTW_MEASURE);
	psf_fft = fftw_plan_dft_2d(height,width,psf,psf,FFTW_FORWARD,FFTW_MEASURE);
	out_fft = fftw_plan_dft_2d(height,width,out,out,FFTW_BACKWARD,FFTW_MEASURE);

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			img[i][0] = image[i];
			img[i][1] = 0.0;
			psf[i][0] = point_spread[i];
			psf[i][1] = 0.0;
		}
	}

	fftw_execute(img_fft);
	fftw_execute(psf_fft);

	double normalizer = (double)width*(double)height;
	double psf_max_real = 0.0;
	double psf_max_imag = 0.0;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			img[i][0] = img[i][0]/normalizer;
			img[i][1] = img[i][1]/normalizer;
			psf[i][0] = psf[i][0]/normalizer;
			psf[i][1] = psf[i][1]/normalizer;

			if(psf[i][0] > psf_max_real)
			{
				psf_max_real = psf[i][0];
			}
			if(psf[i][1] > psf_max_imag)
			{
				psf_max_imag = psf[i][1];
			}
			if(psf[i][0] < 0)
				psf[i][0] = 0.0;
			if(psf[i][1] < 0)
				psf[i][1] = 0.0;
		}
	}

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			psf[i][0] = psf[i][0]/psf_max_real;
			psf[i][1] = psf[i][1]/psf_max_imag;
		}
	}

	//do the convolution (multiplication)
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			EAdivide(img[i],psf[i],&out[i]);
		}
	}

	fftw_execute(out_fft);
	
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int i = y*width+x;
			result[i] = out[i][0];
		}
	}
	fftw_destroy_plan(out_fft);
	fftw_destroy_plan(img_fft);
	fftw_destroy_plan(psf_fft);
	fftw_free(img);
	fftw_free(out);
	fftw_free(psf);
	return 1;
}


void roll(long width, long height, double *img, long xoffset, long yoffset)
{
	long x,y;
	double *temp = malloc(sizeof(double)*width*height);
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			temp[y*width+x] = img[y*width+x];
		}
	}
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			long i = y*width+x;
			long xnew = x+xoffset;
			long ynew = y+yoffset;
			if(xnew >= width)
				xnew = x-xoffset;
			if(ynew >= height)
				ynew = y-yoffset;
			img[ynew*width+xnew] = temp[i];
		}
	}
	free(temp);
}

void mirror(int width, int height, double *image, int flipx, int flipy)
{
	int x,y;
	double *temp = malloc(sizeof(double)*width*height);
	memcpy(temp,image,sizeof(double)*width*height);
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int newx = x;
			int newy = y;	
			if(flipx)
				newx = width-x-1;	
			if(flipy)
				newy = height-y-1;
			int index = newy*width+newx;
			image[index] = temp[y*width+x];
		}
	}
}

void elementdivide(int width, int height, double *numerator, double *denominator, double * result)
{
	int x,y;
	float cutoff = 1e-6;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			double denom = denominator[y*width+x];
			if(denom < cutoff)
				denom = cutoff;
			result[y*width+x] = numerator[y*width+x]/denom;
		}
	}
}

void elementmultiply(int width, int height, double *a, double *b, double *result)
{
	int x,y;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			result[y*width+x] = a[y*width+x]*b[y*width+x];
		}
	}
}

void normalize(int width, int height, double *input, double *result)
{
	int x,y;
	double sum;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			sum+=input[y*width+x];
		}
	}
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			result[y*width+x] = input[y*width+x]/sum;
		}
	}
	//printf("Sum: %0.4f\n",sum);
}

void iteratePSF(int width, int height, double *image, double *psf, double *original_image)
{
	double *result1 = malloc(sizeof(double)*width*height);
	double *divided = malloc(sizeof(double)*width*height);
	double *correlation = malloc(sizeof(double)*width*height);
	convolve(width,height,image,psf,result1);
	elementdivide(width,height,original_image,result1,divided);
	normalize(width,height,image,result1);
	mirror(width,height,divided,1,1);
	convolve(width,height,result1,divided,result1);
	elementmultiply(width,height,result1,psf,psf);
	free(result1);
	free(divided);
	free(correlation);
}

void iterateImage(int width, int height, double *image, double *psf, double *original_image)
{
	double *result = malloc(sizeof(double)*width*height);
	double *divided = malloc(sizeof(double)*width*height);
	double *correlation = malloc(sizeof(double)*width*height);
	convolve(width,height,image,psf,result);
	elementdivide(width,height,original_image,result,divided);
	normalize(width,height,psf,result);
	mirror(width,height,divided,1,1);
	convolve(width,height,result,divided,result);
	elementmultiply(width,height,result,image,image);
	free(result);
	free(divided);
	free(correlation);
}

void createInitialPSF(int width, int height, double *image, double *psf)
{
	double *mirrored = malloc(sizeof(double)*width*height);
	memcpy(mirrored,image,sizeof(double)*width*height);
	mirror(width,height,mirrored,1,1);
	convolve(width,height,image,mirrored,psf);
	double min, max, eps, sum;
	min = 10000000;
	max = 0.0;
	sum = 0.0;
	eps = 0.01;
	int x,y;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			if(psf[y*width+x] < min)
				min = psf[y*width+x];
			if(psf[y*width+x] > max)
				max = psf[y*width+x];
			sum += psf[y*width+x];
		}
	}
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			psf[y*width+x] = psf[y*width+x] - min + eps*(max-min);
			psf[y*width+x] = psf[y*width+x]/sum;
		}
	}
	free(mirrored);
}

void saveImage(int width, int height, const char *filename, double *data, float scaler)
{
	MagickWandGenesis();
	MagickWand *output;
	output = NewMagickWand();
	PixelWand *bg = NewPixelWand();
	PixelSetColor(bg, "#000000");
	MagickNewImage(output,width,height,bg);
	PixelIterator *iterator = NewPixelIterator(output);
	long w;

	int x,y;
	for(y=0;y<height;y++)
	{
		PixelWand **line;
		line = PixelGetNextIteratorRow(iterator, &w);	
		for(x=0;x<width;x++)
		{
			MagickPixelPacket outpix;
			PixelGetMagickColor(line[x],&outpix);
			int value = (int)(data[y*width+x]*scaler);
			outpix.red = value;
			outpix.blue = value;
			outpix.green = value;
			PixelSetMagickColor(line[x],&outpix);
		}
		(void)PixelSyncIterator(iterator);
	}

	MagickWriteImages(output, filename, MagickTrue);
	DestroyMagickWand(output);
	MagickWandTerminus();
}

double * openImage(const char *filename, long *width, long *height)
{
	MagickWandGenesis();
	MagickWand *image = NewMagickWand();
	MagickReadImage(image,filename);
	PixelIterator *iterator = NewPixelIterator(image);

	*width = MagickGetImageWidth(image);
	*height = MagickGetImageHeight(image);
	
	int w,h;
	w = *width;
	h = *height;
	long wid;

	double *temp = malloc(sizeof(double)*w*h);

	int x,y;
	for(y=0;y<h;y++)
	{
		PixelWand **line = PixelGetNextIteratorRow(iterator,&wid);
		for(x=0;x<w;x++)
		{
			MagickPixelPacket pix;
			PixelGetMagickColor(line[x],&pix);
			temp[y*w+x] = (double)pix.red;
		}
	}
	return temp;	
	DestroyMagickWand(image);
	MagickWandTerminus();
}

double * scaleto(long width, long height, double *input, double *output, double scalemin, double scalemax)
{
	long x,y;
	double max,min;
	max = 0.0;
	min = 9999999999999;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			if(input[y*width+x] > max)
				max = input[y*width+x];
			if(input[y*width+x] < min)
				min = input[y*width+x];
		}
	}
	assert(max > 0);
	double scale = scalemax/max;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			output[y*width+x] = input[y*width+x]*scale-(scale*min)+scalemin;
		}
	}
}
