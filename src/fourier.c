#include "fourier.h"
#include <stdio.h>

int dft(long length, double real_values[], double imag_values[])
{
	long i, j;
	double arg;
	double cosarg, sinarg;
	double *temp_real=NULL, *temp_imag=NULL;

	temp_real = calloc(length, sizeof(double));
	temp_imag = calloc(length, sizeof(double));
	if(temp_real == NULL || temp_imag == NULL)
	{
		return(0);
	}

	for(i=0; i<length; i++)
	{
		temp_real[i] = 0;
		temp_imag[i] = 0;
		arg = -1.0*2.0*3.1415926535*(double)i/(double)length;
		for(j=0;j<length;j+=1)
		{
			cosarg = cos(j*arg);
			sinarg = sin(j*arg);
			temp_real[i]+=(real_values[j]*cosarg - imag_values[j]*sinarg);
			temp_imag[i]+=(real_values[j]*sinarg + imag_values[j]*cosarg);
		}
	}
	for(i=0;i<length;i++)
	{
		real_values[i] = temp_real[i];
		imag_values[i] = temp_imag[i];
	}

	free(temp_real);
	free(temp_imag);
	return(1);
}

int fkl(int k, int l, int width, int height, double *real_values, double *imag_values, double *F_real, double *F_imag, int inverse)
{
	float inverter = inverse ? 1.0 : -1.0;
	printf("Progress:%3.2f%s\r",(100*(((float)l+(double)height/2.0)*(float)width+((float)k+(double)width))/((float)width*(float)height)),"%");
	fflush(stdout);
	int x,y;
	double x_arg, p_arg;
	double cosx_arg,sinx_arg,cosp_arg,sinp_arg;	
	long N = width*height;
	double *temp_real = NULL, *temp_imag=NULL;
	double F_r, F_i;	
	double P_real, P_imag;
	x_arg = inverter*2.0*3.1415926535*(double)k/(double)width;
	p_arg = inverter*2.0*3.1415926535*(double)l/(double)height;
	for(y=0;y<height;y++)
	{
		P_real = 0;
		P_imag = 0;
		for(x=0;x<width;x++)
		{
			cosx_arg = cos((double)x*x_arg);
			sinx_arg = sin((double)x*x_arg);
			P_real += (real_values[y*width+x]*cosx_arg - imag_values[y*width+x]*sinx_arg);
			P_imag += (real_values[y*width+x]*sinx_arg + imag_values[y*width+x]*cosx_arg);
		}
		P_real = P_real/(double)width;
		P_imag = P_imag/(double)width;
			
		cosp_arg = cos((double)y*p_arg);
		sinp_arg = sin((double)y*p_arg);
		F_r += (P_real*cosp_arg - P_imag*sinp_arg);
		F_i += (P_real*sinp_arg + P_imag*cosp_arg);
	}
	F_r = F_r/(double)height;
	F_i = F_i/(double)height; 
	*F_real = F_r;
	*F_imag = F_i;
	return 1;
	}

ikl(int k, int l, int width, int height, double *real_values, double *imag_values, double *F_real, double *F_imag)
{
	
}


int fdft(int width, int height, double *real_values, double *imag_values, double *F_r, double *F_i)
{
	double F_real, F_imag;
	int k,l;
	for(l=-(height/2);l<(height/2);l++)
	{
		for(k=-(width/2);k<(width/2);k++)
		{
			F_real = 0;
			F_imag = 0;
			fkl(k,l,width,height,real_values,imag_values,&F_real,&F_imag,0);
			F_r[(l+height/2)*width+(k+width/2)] = F_real;
			F_i[(l+height/2)*width+(k+width/2)] = F_imag;
		}
//		printf("Real: %5.11f\n",F_real);
	}

	return 1;
}

int idft(int width, int height, double *real_values, double *imag_values, double *F_r, double *F_i)
{
	double F_real, F_imag;
	int k,l;
	for(l=-(height/2);l<(height/2);l++)
	{
		for(k=-(width/2);k<(width/2);k++)
		{
			F_real = 0;
			F_imag = 0;
			fkl(k,l,width,height,real_values,imag_values,&F_real,&F_imag,1);
			F_r[(l+height/2)*width+(k+width/2)] = F_real;
			F_i[(l+height/2)*width+(k+width/2)] = F_imag;
		}
	}
	return 1;
}

/*int main(void)
{
	int width = 100;
	int height = 50;
	double *reals;
	double *imag;
	reals = calloc(height*width, sizeof(double));
	imag = calloc(height*width, sizeof(double));
	int i = 0;
	int j = 0;
	for(i=0;i<width;i++)
	{
		for(j=0;j<height;j++)
		{
			reals[j*width+i] = cos((float)j/2.0)+sin((float)i/4.5);
			imag[j*width+i] = 0.0;
		}
	}
	double *output_Real;
	output_Real = calloc(height*width, sizeof(double));
	ift(width, height, reals, imag, output_Real);
	for(i=0;i<width;i++)
	{
		//printf("\n");
		for(j=0;j<height;j++)
		{
			printf("(%d,%d): %3.7f \n",i,j,output_Real[j*width+i]);
		}
	}
	//printf("\n");
	return 0;
}*/
