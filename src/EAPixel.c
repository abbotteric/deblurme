#include "EAPixel.h"
#include <stdio.h>
#include <stdlib.h>


EAPixel *NewEAPixel(void)
{
	EAPixel *px = (EAPixel *)malloc(sizeof(EAPixel));
	return px;
}

void DestroyEAPixel(EAPixel *px)
{
	free(px);
}
