#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>
#include "EAPixel.h"

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
//	PixelWand **pixels;
	register long x;
//	unsigned long width;
	PixelWand *color = NewPixelWand();

	MagickWandGenesis();
	image_wand=NewMagickWand();
	status = MagickReadImage(image_wand,argv[1]);
	iterator = NewPixelIterator(image_wand);

	//Initialize the pixels that we'll be using
//	MagickPixelPacket tl,tc,tr,ml,mc,mr,bl,bc,br;
	EAPixel *tl,*tc,*tr,*ml,*mc,*mr,*bl,*bc,*br;
	EAPixel **pixels = malloc(9*sizeof(EAPixel*));
	int pixcount;

	if(status == MagickFalse)
	{
		printf("Bad Stuff!!");
	}
	long height = MagickGetImageHeight(image_wand);
	long width = MagickGetImageWidth(image_wand);
	for (y = 0; y<height/*(long)MagickGetImageHeight(image_wand)*/; y++)
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
		for(x = 0; x<width/*(long)MagickGetImageWidth(image_wand)*/; x++)
		{
			pixcount = 0;
			if(y>0)
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
				/*tl = NULL;
				tc = NULL;
				tr = NULL;*/
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
				/*bl = NULL;
				bc = NULL;
				br = NULL;*/
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
				(ml->px).red = 65000;
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
			pixcount++;
		}
		(void)PixelSyncIterator(iterator);
	}
	status = MagickWriteImages(image_wand,argv[2],MagickTrue);
	image_wand = DestroyMagickWand(image_wand);
	MagickWandTerminus();
	return 0;
}
