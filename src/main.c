#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "EAPixel.h"
#include "fourier.h"
#include <fftw3.h>

int main(int argc, char **argv)
{
	//set up ImageMagick stuff
	long y;
	MagickBooleanType status;
	MagickWand *image_wand;
	PixelIterator *iterator;
	register long x;

	MagickWandGenesis();
	image_wand=NewMagickWand();
	status = MagickReadImage(image_wand,argv[1]);
	iterator = NewPixelIterator(image_wand);

	//set input image stats
	long height = MagickGetImageHeight(image_wand);
	long width = MagickGetImageWidth(image_wand);

	//set up fftw plan
	fftw_complex *in, *out;
	fftw_plan p;
	in = (fftw_complex*)fftw_alloc_complex(width*height);
	out = (fftw_complex*)fftw_alloc_complex(width*height);
	p = fftw_plan_dft_2d(height,width,in,out,FFTW_FORWARD,FFTW_MEASURE);

	//fill the 'in' array with pixel values
	for(y=0;y<height;y++)
	{
		PixelWand **line;
		line = PixelGetNextIteratorRow(iterator,&width);
		for(x=0;x<width;x++)
		{
			MagickPixelPacket pix;
			PixelGetMagickColor(line[x],&pix);
			in[y*width+x][0] = (double)pix.red;
			in[y*width+x][1] = 0.0;
		}
	}

	//execute the planned fft
	fftw_execute(p);	

	//create a new input array for the inverse fft
	fftw_complex *newin = (fftw_complex*)fftw_alloc_complex(width*height);
	fftw_destroy_plan(p);
	p = fftw_plan_dft_2d(height,width,newin,in,FFTW_BACKWARD,FFTW_MEASURE);

	//fill the new input array	
	int w,h,mult;
	for(h=0;h<height;h++)
	{
		for(w=0;w<width;w++)
		{
			newin[h*width+w][0] = out[h*width+w][0];
			newin[h*width+w][1] = out[h*width+w][1];
		}
	}

	//execute the inverse fft plan
	fftw_execute(p);
	
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
			int value_real = (int)(in[y*width+x][0]/((float)width*(float)height));
			outpix.red = value_real;
			outpix.green = value_real;
			outpix.blue = value_real;

			MagickPixelPacket out_imag;
			PixelGetMagickColor(imag_line[x], &out_imag);
			int value_imag = (int)out[y*width+x][1];
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
	image_wand = DestroyMagickWand(image_wand);
	outputWand = DestroyMagickWand(outputWand);
	outputImag = DestroyMagickWand(outputImag);
	MagickWandTerminus();
	system("./beep");
	return 0;
}
