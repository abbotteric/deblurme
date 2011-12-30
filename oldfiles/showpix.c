#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>

int main(int argc, char **argv)
{
	/*#define ThrowWandException(wand)\
	{\
		char *description;\
		ExceptionType\
		severity;\
		\
		description=MagickGetException(wand,&severity);\
		(void)fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description);\
		exit(-1);\
	}*/
	
	long y;
	MagickBooleanType status;
	MagickPixelPacket pixel;
	MagickWand *image_wand;
	PixelIterator *iterator;
	PixelWand **pixels;
	register long x;
	unsigned long width;
	PixelWand *color = NewPixelWand();

	MagickWandGenesis();
	image_wand=NewMagickWand();
	status = MagickReadImage(image_wand,argv[1]);
	iterator = NewPixelIterator(image_wand);

	if(status == MagickFalse)
	{
		printf("Bad Stuff!!");
	}
	for (y = 0; y<(long)MagickGetImageHeight(image_wand); y++)
	{
		pixels = PixelGetNextIteratorRow(iterator,&width);
		for(x = 0; x<(long)MagickGetImageWidth(image_wand); x++)
		{
			if (x%3 == 0 || y%3 == 0)
			{
				//MagickGetImagePixelColor(image_wand,x,y,color);
				//PixelSetColor(color, "#0000ff");
				PixelGetMagickColor(pixels[x], &pixel);
				pixel.blue+=15000;
				PixelSetMagickColor(pixels[x], &pixel);
			//	PixelSetColor(pixels[x], "#0000ff");	
			}
		}
		(void)PixelSyncIterator(iterator);
	}
	status = MagickWriteImages(image_wand,argv[2],MagickTrue);
	image_wand = DestroyMagickWand(image_wand);
	MagickWandTerminus();
	return 0;
}
