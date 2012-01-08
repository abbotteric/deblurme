#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "EAPixel.h"
#include "fourier.h"

int main(int argc, char **argv)
{
	long y;
	MagickBooleanType status;
	MagickWand *image_wand;
	PixelIterator *iterator;
	register long x;
	PixelWand *color = NewPixelWand();

	MagickWandGenesis();
	image_wand=NewMagickWand();
	status = MagickReadImage(image_wand,argv[1]);
	iterator = NewPixelIterator(image_wand);

	EAPixel *tl,*tc,*tr,*ml,*mc,*mr,*bl,*bc,*br;
	EAPixel **pixels = malloc(9*sizeof(EAPixel*));
	int pixcount;

	long height = MagickGetImageHeight(image_wand);
	long width = MagickGetImageWidth(image_wand);

	double *real_pixels = calloc(height*width,sizeof(double));
	double *imag_pixels = calloc(height*width,sizeof(double));
	double *output_real = calloc(height*width, sizeof(double));	
	double *output_imag = calloc(height*width, sizeof(double));

	for(y=0;y<height;y++)
	{
		PixelWand **line;
		line = PixelGetNextIteratorRow(iterator,&width);
		for(x=0;x<width;x++)
		{
			MagickPixelPacket pix;
			PixelGetMagickColor(line[x],&pix);
			real_pixels[y*width+x] = (double)pix.red;
			imag_pixels[y*width+x] = 0.0;
		}
	}

	fdft(width,height,real_pixels,imag_pixels,output_real,output_imag);

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

	double *inverse_real = calloc(height*width, sizeof(double));
	double *inverse_imag = calloc(height*width, sizeof(double));

	idft(width, height,output_real,output_imag,inverse_real,inverse_imag);

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
			int value_real = (int)inverse_real[y*width+x];
			outpix.red = value_real;
			outpix.green = value_real;
			outpix.blue = value_real;

			MagickPixelPacket out_imag;
			PixelGetMagickColor(imag_line[x], &out_imag);
			int value_imag = (int)inverse_imag[y*width+x];
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
	return 0;
}
