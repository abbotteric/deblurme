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

	//Initialize the pixels that we'll be using
	MagickPixelPacket tl,tc,tr,ml,mc,mr,bl,bc,br;
	PixelWand *newPix = NewPixelWand();

	if(status == MagickFalse)
	{
		printf("Bad Stuff!!");
	}
	for (y = 0; y<(long)MagickGetImageHeight(image_wand); y++)
	{
		PixelWand **prevLine;
		PixelWand **curLine;
		PixelWand **nextLine;
		if(y != 0)
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
			nextLine = NULL;
		}
		for(x = 0; x<(long)MagickGetImageWidth(image_wand); x++)
		{
			MagickPixelPacket *px = newPix.pixel;
			int numpix = 0;
			if(y>0)
			{
				PixelGetMagickColor(prevLine[x],&tc);
				if(x>0)
					PixelGetMagickColor(prevLine[x-1],&tl);
				else
					tl = NULL;
				if(x < width)
					PixelGetMagickColor(prevLine[x+1],&tr);
				else
					tr = NULL;
			}
			else
			{
				tl = NULL;
				tc = NULL;
				tr = NULL;
			}
			if(y<MagickGetImageHeight(image_wand))
			{
				PixelGetMagickColor(nextLine[x],&bc);
				if(x>0)
					PixelGetMagickColor(nextLine[x-1],&bl);
				else
					bl = NULL;
				if(x < width)
					PixelGetMagickColor(nextLine[x+1],&br);
				else
					br = NULL;
			}
			else
			{
				bl = NULL;
				bc = NULL;
				br = NULL;
			}
			if(x>0)
				PixelGetMagickColor(curLine[x-1],&ml);
			else
				ml = NULL;
			if(x<width)
				PixelGetMagickColor(curLine[x+1],&mr);
			else
				mr = NULL;

			PixelGetMagickColor(curLine[x],&mc);
		}
		(void)PixelSyncIterator(iterator);
	}
	status = MagickWriteImages(image_wand,argv[2],MagickTrue);
	image_wand = DestroyMagickWand(image_wand);
	MagickWandTerminus();
	return 0;
}
