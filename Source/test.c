/*****************************************************************************
 * main.c
 *
 * Final Project - Channel Vocoder
 * Written by Xiao Lu
 * Spring 2016
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h> 	/* malloc() */
#include <unistd.h>     /* sleep() */
#include <stdbool.h>	/* true, false */
#include <string.h>		/* memset() */
// #include <ctype.h>		/* tolower() */
#include <math.h>		/* sin() */
#include <sndfile.h>	/* libsndfile */
// #include <portaudio.h>	/* portaudio */
// #include <ncurses.h> 	/* This library is for getting input without hitting return */
#include "test.h"
#include "fft.h"        /* FFTs and IFFTs */
#include "vocode.h"		/* The vocoding part */

// #define PI  				3.14159265358979323846264338327950288
// #define SAMPLE_RATE 		44100
// #define FRAMES_PER_BUFFER 	1024
// #define MAX_CHN			 	2
// #define NUM_BAND			32
// #define BUFFER_SIZE			1024

/* PortAudio callback function protoype */
// static int paCallback( const void *inputBuffer, void *outputBuffer,
//                         unsigned long framesPerBuffer,
//                         const PaStreamCallbackTimeInfo* timeInfo,
//                         PaStreamCallbackFlags statusFlags,
//                         void *userData );

/* local function prototypes */
bool read_input(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf);
bool write_output(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf, long frames);

int main(int argc, char *argv[])
{
	char *mod_filename, *car_filename, *ofilename;
	int i,j, oframes;
	char *usageLine = "main modulator.wav carrier.wav [ofile.wav]";
	int bufSize = BUFFER_SIZE, numBand = NUM_BAND, hopSize = HOP_SIZE;
	/* my data structures */
	Buf mbuf, cbuf, obuf;
	/* libsndfile data structures */
	SNDFILE *mod_sfile, *car_sfile, *osfile; 
	SF_INFO mod_sfinfo, car_sfinfo, osfinfo;

	/* zero libsndfile structures */
	memset(&mod_sfinfo, 0, sizeof(mod_sfinfo));
  	memset(&car_sfile, 0, sizeof(car_sfile));
  	memset(&osfinfo, 0, sizeof(osfinfo));

  	/* zero buffer pointers in Buf structures */
  	for (i=0; i<MAX_CHN; i++) {
  		mbuf.buf[i] = 0;
  		cbuf.buf[i] = 0;
  		obuf.buf[i] = 0;
  	}

  	/* Parsing Command Line*/
	if ( argc < 3 || argc > 4 )
    {
        printf("%s\n", usageLine);
        return -1;
    }
    mod_filename = argv[1];
    car_filename = argv[2];
    if (argc == 3)
    {	ofilename = "foo.wav"; }
    else 
    { 	ofilename = argv[3]; }	

    /* Open input files */
    if ( (mod_sfile = sf_open (mod_filename, SFM_READ, &mod_sfinfo)) == NULL ) {
        fprintf (stderr, "Error: could not open wav file: %s\n", mod_filename);
        return -1;
    }
    if ( (car_sfile = sf_open (car_filename, SFM_READ, &car_sfinfo)) == NULL ) {
        fprintf (stderr, "Error: could not open wav file: %s\n", car_filename);
        return -1;
    }


	/* Print input file information */
	printf("%s:\n\tFrames: %d Channels: %d Samplerate: %d\n", 
		mod_filename, (int)mod_sfinfo.frames, mod_sfinfo.channels, mod_sfinfo.samplerate);

	printf("%s:\n\tFrames: %d Channels: %d Samplerate: %d\n", 
		car_filename, (int)car_sfinfo.frames, car_sfinfo.channels, car_sfinfo.samplerate);

	/* If sample rates don't match, exit */
	if ( mod_sfinfo.samplerate != car_sfinfo.samplerate )
	{
		printf("Sample rates dont' match.\n");
		return -1;
	}
	/* If number of channels don't match or too many channels, exit */
	if ( mod_sfinfo.channels != car_sfinfo.channels || mod_sfinfo.channels > MAX_CHN )
	{
		printf("No. of channels dont' match or too many channels.\n");
		return -1;
	}

	/* When opening a file for writing, the caller must fill in structure members 
	* 	samplerate, channels, and format. 
	* Make these the same as input file.
	*/
	osfinfo.samplerate = mod_sfinfo.samplerate;
	osfinfo.channels = mod_sfinfo.channels;
	osfinfo.format = mod_sfinfo.format;
    osfinfo.frames = mod_sfinfo.frames;
    oframes = osfinfo.frames;
	/* open output file */
    if ( (osfile = sf_open (ofilename, SFM_WRITE, &osfinfo)) == NULL ) {
        printf ("Error : could not open wav file : %s\n", ofilename);
        return -1;
    }


	/* Allocate separate buffers for each channel of 
	 * input and output files
	 */
	for (i=0; i<mod_sfinfo.channels; i++) {
		if ( (mbuf.buf[i] = (float *)malloc(mod_sfinfo.frames * sizeof(*mbuf.buf[i]))) == NULL ) 
	    {
	        printf("Fail to allocate storage for the mbuf.\n");
	        return -1;
	    }
	    if ( (cbuf.buf[i] = (float *)malloc(car_sfinfo.frames * sizeof(*cbuf.buf[i]))) == NULL ) 
	    {
	        printf("Fail to allocate storage for the cbuf.\n");
	        return -1;
	    }
		if ( ( obuf.buf[i] = (float *)malloc(osfinfo.frames * sizeof(*obuf.buf[i])) ) == NULL ) 
	    {
	        printf("Fail to allocate storage for the obuf.\n");
	        return -1;
	    }

	}
	// printf("Allocated buffers\n");

	/* read interleaved data from files into de-interleaved buffers */
	/* mod */
	if ( !read_input(mod_sfile, &mod_sfinfo, &mbuf) ) {
		fprintf(stderr, "ERROR: Cannot read input file %s", mod_filename);
		return -1;
	}
	/* car */
	if ( !read_input(car_sfile, &car_sfinfo, &cbuf) ) {
		fprintf(stderr, "ERROR: Cannot read input file %s", car_filename);
		return -1;
	}
	printf("Read input files\n");


	/* VOCODE */
	printf("Processing audio...\n");
	int numChan = mod_sfinfo.channels;
	int numFrames = (mod_sfinfo.frames <= car_sfinfo.frames) ? mod_sfinfo.frames : car_sfinfo.frames;

	vocode(&mbuf, &cbuf, bufSize, hopSize, numBand, numChan,numFrames, &obuf);
	printf("Finished Processing.\n");
	/* interleave output data and write output file */
	if ( !write_output(osfile, &osfinfo, &obuf, oframes) ) {
		fprintf(stderr, "ERROR: Cannot write output file %s", ofilename);
		return -1;
	}

	/* Must close file; output will not be written correctly if you do not do this */
	sf_close (mod_sfile);
	sf_close (car_sfile);
	sf_close (osfile);

	/* free all buffer storage */
	printf("Freeing buffers\n");
	for (i=0; i<MAX_CHN; i++) {
		if (mbuf.buf[i] != NULL)
			free(mbuf.buf[i]);
		if (cbuf.buf[i] != NULL)
			free(cbuf.buf[i]);
		if (obuf.buf[i] != NULL)
			free(obuf.buf[i]);
	}
	printf("ALL DONE.\n");
	return 0;
}

bool read_input(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf)
{
	int i, j, count;
	float frame_buf[MAX_CHN]; /* to hold one sample frame of audio data */
	for (i=0; i < sfinfo->frames; i++) {
		/* for each frame */
		if ( (count = sf_read_float (sfile, frame_buf, sfinfo->channels)) != sfinfo->channels ) 
		{
			fprintf(stderr, "Error: on sample frame %d\n", i);
			return false;
		}
		//de-interleave the frame[j] into separate channel buffers buf->buf[j][i]
		for (j = 0; j < count; j++)
		{
			buf->buf[j][i] = frame_buf[j];
		}
	}
	return true;
}

bool write_output(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf, long frames)
{
	int i, j, count;
	float frame_buf[MAX_CHN]; /* to hold one sample frame of audio data */
	for (i=0; i<frames; i++) {
		/* for each frame */
		//interleave separate channel buffers buf->buf[j][i] into a frame_buf[j]
		for (j = 0; j < sfinfo->channels; j++)
		{
			frame_buf[j] = buf->buf[j][i];
		}
		if ( (count = sf_write_float (sfile, frame_buf, sfinfo->channels)) != sfinfo->channels ) 
		{
			fprintf(stderr, "Error: on sample frame %d\n", i);
			return false;
		}
	}
	printf("Wrote %ld frames\n", frames);
	return true;
}