#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "EAPixel.h"
#include "fourier.h"
#include <fftw3.h>

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("\nUse: %s <image file> <filter image>\n\n",argv[0]);
		return -1;
	}

	//set up ImageMagick stuff
	long y;
	MagickBooleanType status;
	register long x;

	MagickWandGenesis();
	MagickWand *original_wand = NewMagickWand();
	status = MagickReadImage(original_wand,argv[1]);
	PixelIterator *original_iterator = NewPixelIterator(original_wand);

	MagickWand *filter_wand = NewMagickWand();
	status = MagickReadImage(filter_wand, argv[2]);
	PixelIterator *filter_iterator = NewPixelIterator(filter_wand);

	//set input image stats
	long height = MagickGetImageHeight(original_wand);
	long width = MagickGetImageWidth(original_wand);

	//set up fftw plan
	fftw_complex *original_in, *filter_in, *output_fft;
	fftw_plan original_forward, backward, filter_forward;
	original_in = (fftw_complex*)fftw_alloc_complex(width*height);
	filter_in = (fftw_complex*)fftw_alloc_complex(width*height);
	output_fft = (fftw_complex*)fftw_alloc_complex(width*height);
	original_forward = fftw_plan_dft_2d(height,width,original_in,original_in,FFTW_FORWARD,FFTW_MEASURE);
	filter_forward = fftw_plan_dft_2d(height,width,filter_in,filter_in,FFTW_FORWARD,FFTW_MEASURE);	
	backward = fftw_plan_dft_2d(height,width,output_fft,output_fft,FFTW_BACKWARD,FFTW_MEASURE);

	//fill the 'in' array with pixel values
	for(y=0;y<height;y++)
	{
		PixelWand **original_line;
		original_line = PixelGetNextIteratorRow(original_iterator,&width);
		PixelWand **filter_line;
		filter_line = PixelGetNextIteratorRow(filter_iterator,&width);
		for(x=0;x<width;x++)
		{
			MagickPixelPacket pix;
			PixelGetMagickColor(original_line[x],&pix);
			original_in[y*width+x][0] = (double)pix.red;
			original_in[y*width+x][1] = 0.0;

			MagickPixelPacket pix1;
			PixelGetMagickColor(filter_line[x], &pix1);
			filter_in[y*width+x][0] = ((double)pix1.red/(double)QuantumRange);;
			filter_in[y*width+x][1] = 0.0;
		}
	}

	//execute the planned fft
	fftw_execute(original_forward);	
//	fftw_execute(filter_forward);

	//normalize the fft values
	double scaler = (double)width*(double)height;
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			int index = y*width+x;
			original_in[index][0] = original_in[index][0]/scaler;
			original_in[index][1] = original_in[index][1]/scaler;
			//filter_in[index][0] = filter_in[index][0]/scaler;
			//filter_in[index][1] = filter_in[index][1]/scaler;
		}
	}
	
	//multiply the frequency-domain pixels together
	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			output_fft[y*width+x][0] = original_in[y*width+x][0]*filter_in[y*width+x][0];
			output_fft[y*width+x][1] = original_in[y*width+x][1];
//			EAmultiply(original_in[y*width+x],filter_in[y*width+x], output_fft[y*width+x]);	
		}
	}

	fftw_execute(backward);

	//create some empty images for output
	MagickWand *outputWand;
	outputWand = NewMagickWand();	
	PixelWand *bg = NewPixelWand();
	PixelSetColor(bg, "#000000");
	MagickNewImage(outputWand,width,height,bg);
	PixelIterator *outputIterator = NewPixelIterator(outputWand);

	MagickWand *outputImag;
	outputImag = NewMagickWand();
	MagickNewImage(outputImag, width, height, bg);
	PixelIterator *output_imag_iterator = NewPixelIterator(outputImag);

	//fill the images with the fft output
	for(y=0;y<height;y++)
	{
		PixelWand **line;
		line = PixelGetNextIteratorRow(outputIterator, &width);
		PixelWand **imag_line;
		imag_line = PixelGetNextIteratorRow(output_imag_iterator, &width);
		for(x=0;x<width;x++)
		{
			MagickPixelPacket outpix;
			PixelGetMagickColor(line[x],&outpix);
			int value_real = (int)output_fft[y*width+x][0];
			outpix.red = value_real;
			outpix.green = value_real;
			outpix.blue = value_real;

			MagickPixelPacket out_imag;
			PixelGetMagickColor(imag_line[x], &out_imag);
			int value_imag = (int)output_fft[y*width+x][1];
			out_imag.red = value_imag;
			out_imag.green = value_imag;
			out_imag.blue = value_imag;

			PixelSetMagickColor(line[x],&outpix);
			PixelSetMagickColor(imag_line[x], &out_imag);
		}
		(void)PixelSyncIterator(outputIterator);
		(void)PixelSyncIterator(output_imag_iterator);
	}
	
	status = MagickWriteImages(outputWand,"output-real.jpg",MagickTrue);
	status = MagickWriteImages(outputImag,"output-imag.jpg",MagickTrue);
	original_wand = DestroyMagickWand(original_wand);
	filter_wand = DestroyMagickWand(filter_wand);
	outputWand = DestroyMagickWand(outputWand);
	outputImag = DestroyMagickWand(outputImag);
	MagickWandTerminus();
	system("./beep");
	return 0;
}
