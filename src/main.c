#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "EAPixel.h"
#include "fourier.h"
#include <fftw3.h>
#include "deblur.h"

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

	double *image, *psf, *result;
	image = malloc(sizeof(double)*width*height);
	psf = malloc(sizeof(double)*width*height);
	result = malloc(sizeof(double)*width*height);

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
			image[y*width+x] = (double)pix.red;

			MagickPixelPacket pix1;
			PixelGetMagickColor(filter_line[x], &pix1);
			psf[y*width+x] = (double)pix1.red;
		}
	}

	roll(width,height,psf,width/2,height/2);
	convolve(width,height,image,psf,result);

	//create some empty images for output
	MagickWand *outputWand;
	outputWand = NewMagickWand();	
	PixelWand *bg = NewPixelWand();
	PixelSetColor(bg, "#000000");
	MagickNewImage(outputWand,width,height,bg);
	PixelIterator *outputIterator = NewPixelIterator(outputWand);

	//fill the image with the fft output
	for(y=0;y<height;y++)
	{
		PixelWand **line;
		line = PixelGetNextIteratorRow(outputIterator, &width);
		for(x=0;x<width;x++)
		{
			MagickPixelPacket outpix;
			PixelGetMagickColor(line[x],&outpix);
			int value_real = (int)result[y*width+x];
			outpix.red = value_real;
			outpix.green = value_real;
			outpix.blue = value_real;

			PixelSetMagickColor(line[x],&outpix);
		}
		(void)PixelSyncIterator(outputIterator);
	}
	
	status = MagickWriteImages(outputWand,"output-real.jpg",MagickTrue);
	original_wand = DestroyMagickWand(original_wand);
	filter_wand = DestroyMagickWand(filter_wand);
	outputWand = DestroyMagickWand(outputWand);
	MagickWandTerminus();
//	system("./beep");
	return 0;
}
