#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "fourier.h"
#include <fftw3.h>

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
	return 1;
}

