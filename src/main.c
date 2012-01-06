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
	double *output = calloc(height*width, sizeof(double));	

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

	ift(width,height,real_pixels,imag_pixels,output);

	MagickWand *outputWand;
	outputWand = NewMagickWand();	
	PixelWand *bg = NewPixelWand();
	PixelSetColor(bg, "#000000");
	MagickNewImage(outputWand,width,height,bg);
	PixelIterator *outputIterator = NewPixelIterator(outputWand);

	for(y=0;y<height;y++)
	{
		PixelWand **line;
		line = PixelGetNextIteratorRow(outputIterator, &width);
		for(x=0;x<width;x++)
		{
			MagickPixelPacket outpix;
			PixelGetMagickColor(line[x],&outpix);
			int value = (int)(output[y*width+x]*65535.0);
			outpix.red = value;
			outpix.green = value;
			outpix.blue = value;
			PixelSetMagickColor(line[x],&outpix);
		}
		(void)PixelSyncIterator(outputIterator);
	}
	
	status = MagickWriteImages(outputWand,"output.png",MagickTrue);
	
	for (y = 0; y<height; y++)
	{
		PixelWand **prevLine;
		PixelWand **curLine;
		PixelWand **nextLine;
		if(y == 0)
		{	
			curLine = PixelGetNextIteratorRow(iterator,&width);
			nextLine = PixelGetNextIteratorRow(iterator,&width);
		}
		else if(y < (MagickGetImageHeight(image_wand)-1))
		{
			prevLine = PixelGetPreviousIteratorRow(iterator, &width);
			curLine = PixelGetNextIteratorRow(iterator, &width);
			nextLine = PixelGetNextIteratorRow(iterator, &width);
		}
		else
		{
			prevLine = PixelGetPreviousIteratorRow(iterator, &width);
			curLine = PixelGetNextIteratorRow(iterator, &width);
		}
		for(x = 0; x<width; x++)
		{
			tc = NewEAPixel();
			PixelGetMagickColor(curLine[x],&(tc->px));
			(tc->px).red = 0;
			(tc->px).blue = 0;
			PixelSetMagickColor(curLine[x],&(tc->px));
			DestroyEAPixel(tc);	
			pixcount = 0;
			/*if(y>0)
			{
				tc = NewEAPixel();
				tc->x = 0;
				tc->y = 1;
				PixelGetMagickColor(prevLine[x],&(tc->px));
				pixels[pixcount] = tc;
				pixcount++;
				if(x>0)
				{
					tl = NewEAPixel();	
					tl->x = -1;
					tl->y = 1;
					PixelGetMagickColor(prevLine[x-1],&(tl->px));
					pixels[pixcount] = tl;
					pixcount++;
				}
				if(x < (width-1))
				{	
					tr = NewEAPixel();	
					tr->x = 1;
					tr->y = 1;
					PixelGetMagickColor(prevLine[x+1],&(tr->px));
					pixels[pixcount] = tr;
					pixcount++;
				}
			}
			else
			{
			
			}
			if(y<MagickGetImageHeight(image_wand))
			{
				bc = NewEAPixel();
				bc->x = 0;
				bc->y = -1;
				PixelGetMagickColor(nextLine[x],&(bc->px));
				pixels[pixcount] = bc;
				pixcount++;
				if(x>0)
				{
					bl = NewEAPixel();
					bl->x = -1;
					bl->y = -1;
					PixelGetMagickColor(nextLine[x-1],&(bl->px));
					pixels[pixcount] = bl;	
					pixcount++;
				}
				if(x < (width-1))
				{
					br = NewEAPixel();
					br->x = 1;
					br->y = -1;
					PixelGetMagickColor(nextLine[x+1],&(br->px));
					pixels[pixcount] = br;
					pixcount++;
				}
			}
			else
			{
			
			}
			if(x>0)
			{
			ml = NewEAPixel();
				ml->pxwand = curLine[x-1];
				ml->x = -1;
				ml->y = 0;
				PixelGetMagickColor(ml->pxwand,&(ml->px));
				pixels[pixcount] = ml;
				pixcount++;
				(ml->px).red = 600;
				PixelSetMagickColor(curLine[x-1],&(ml->px));
			}
			if(x<(width-1))
			{
				mr = NewEAPixel();	
				mr->x = 1;
				mr->y = 0;
				PixelGetMagickColor(curLine[x+1],&(mr->px));
				pixels[pixcount] = mr;
				pixcount++;
			}
			mc = NewEAPixel();
			mc->x = 0;
			mc->y = 0;
			PixelGetMagickColor(curLine[x],&(mc->px));
			pixels[pixcount] = mc;
			pixcount++;*/
		}
		(void)PixelSyncIterator(iterator);
	}
	status = MagickWriteImages(image_wand,argv[2],MagickTrue);
	image_wand = DestroyMagickWand(image_wand);
	MagickWandTerminus();
	return 0;
}
