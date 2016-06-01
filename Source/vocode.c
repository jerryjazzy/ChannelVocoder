/*****************************************************************************
 * vocode.c
 *
 * Final Project - Channel Vocoder
 * Written by Xiao Lu
 * Spring 2016
 *****************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fft.h"
#include "test.h"


void magnitude(complex *input, int length, real *output);

void vocode(Buf *mod, Buf *car, int bufSize, int hopSize, int numBand, int numChan, int numFrames, Buf *obuf)
{
	int winSize = bufSize; // might be changed for real-time
	float window[winSize];
	float modBuf[bufSize], carBuf[bufSize];
	complex *scratch = NULL;
	complex *x_mod = NULL;
	complex *x_car = NULL;
	real *x_mod_mag = NULL;
	real *x_car_mag = NULL;
	complex *y = NULL;
	int band_idx = 0;
	float temp_m, temp_c, gain[numBand];
	int bandWidth = (winSize/2) /numBand; ;
	float rms_x = 0, rms_y = 0, scale;
	int i,j,k; // i - general counts; j - no. of chan; k - index of frames

	for(i = 0; i < winSize; i++)   
	{
		// window[i] = (1.0 - cos(2.0*PI*i/(winSize-1)) ) / 2.0; // Hanning Window
		// window[i] = 1; // square window
    	window[i]=0.54-0.46*cos(2.0*PI*i/(winSize-1));   // Hamming Window
	}

	for (j = 0; j < numChan; j++)
	{ 	
		printf("\tProcessing channel %d\n", j+1 );
		for (k = 0; k < numFrames; k+= hopSize)
		{

			/* Put the data into the temporary buffers */
			for (i = 0; i < bufSize; i++)
			{
				if (k+i >= numFrames) break;
				modBuf[i] = mod->buf[j][k+i];
				carBuf[i] = car->buf[j][k+i];
			}				

			/* Allocate memory */
			scratch = (complex *)calloc(winSize, sizeof(complex));
			if (scratch == NULL) {
				printf("Error: unable to allocate memory for FFTs. Exiting.\n");
				exit(1);
			}
			x_mod = (complex *)calloc(winSize, sizeof(complex));
			if (x_mod == NULL) {
				printf("Error: unable to allocate memory for FFTs. Exiting.\n");
				exit(1);
			}
			x_car = (complex *)calloc(winSize, sizeof(complex));
			if (x_car == NULL) {
				printf("Error: unable to allocate memory for FFTs. Exiting.\n");
				exit(1);
			}
			y = (complex *)calloc(winSize, sizeof(complex));
			if (y == NULL) {
				printf("Error: unable to allocate memory for FFTs. Exiting.\n");
				exit(1);
			}
			x_mod_mag = (real *)calloc(winSize, sizeof(real));
			if (x_mod_mag == NULL) {
				printf("Error: unable to allocate memory for FFTs. Exiting.\n");
				exit(1);
			}
			x_car_mag = (real *)calloc(winSize, sizeof(real));
			if (x_mod_mag == NULL) {
				printf("Error: unable to allocate memory for FFTs. Exiting.\n");
				exit(1);
			}
			
			/* Windowing */
			for (i = 0; i < winSize; i++)
			{
				x_mod[i].Re = modBuf[i] * window[i];
				x_car[i].Re = carBuf[i] * window[i];
			}

			/* FFTs */
			fft(x_mod, winSize, scratch);
			fft(x_car, winSize, scratch);
			// printf("FFTs succeeded\n");

			/* Calculate magnitudes */
			magnitude(x_mod, winSize, x_mod_mag); // sqrt(re^2 + im^2)
			magnitude(x_car, winSize, x_car_mag);
			// printf("magnitude calculated\n");

			/* Extract out the estimates(the gain for each band) and re-create the spectrum */
			for (band_idx = 0; band_idx < numBand; band_idx++)
			{
				temp_m = 0;
				temp_c = 0;
				for (i = band_idx * bandWidth ; i < (band_idx+1) * bandWidth  ; i++)
				{
					temp_m += x_mod_mag[i];
					temp_c += x_car_mag[i];
				}

				gain[band_idx] = temp_m / temp_c;

				for (i = band_idx * bandWidth; i < (band_idx+1) * bandWidth  ; i++)
				{
					y[i].Re 	=	 gain[band_idx] * x_car[i].Re;
					y[i].Im 	=	 gain[band_idx] * x_car[i].Im;
				}
			}

			/* Take the inverse FFT of Y */
			ifft(y, winSize, scratch); 

			/* compute RMS values of input and output */
			rms_x = 0;
			rms_y = 0;
			for (i = 0; i < winSize; i++) {
				rms_x += modBuf[i] * modBuf[i];
			}
			rms_x = sqrt(rms_x/winSize);
			for (i = 0; i < winSize; i++) {
				rms_y += y[i].Re * y[i].Re;
			}
			rms_y = sqrt(rms_y/winSize);

			/* Scale so RMS value of output is same as input */
			if (rms_y == 0) {	scale = 0;	}
			else			{	scale = rms_x/rms_y * 0.6;	}
			// printf("RMS values RMS_X: %f, RMS_Y: %f, Scale: %f\n", 
			// 	rms_x, rms_y, scale);
			

			// printf("*** frame no. %d\n",k+i );

			for (i = 0; i < (winSize); i++) {
				obuf->buf[j][k+i] += y[i].Re * scale;
			}

			free(scratch);
			free(x_mod);
			free(x_car);
			free(y);
		}
	}
}

/* This function calculates the magnitude of a complex number */
void magnitude(complex *input, int length, real *output)
{
	real temp;
	for (int i=0; i < length; i++)
	{
		temp = input[i].Re * input[i].Re + input[i].Im * input[i].Im;
		output[i] = sqrt(temp);
	}
} 
