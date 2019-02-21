# ChannelVocoder
Final project for C programming class - a channel vocoder

### INFO
Author: Shaw Lu
May 5, 2016

### Directory and file structure

The codes are all put into the 'source' folder.

Code Files(8):
	test.c fft.c  vocode.c
	test.h fft.h  vocode.h  sndfile.h
	makefile

Audio Files(4):
	mod_testing.wav 	(mono, 22050Hz)
	car_synth.wav 		(mono, 22050Hz)
	mod_HBFS.wav 		(stereo, 44100Hz)
	car_HBFS.wav 		(stereo, 44100Hz)

Text File (1): 
	README.txt

### Installing libsndfile

On PC/Windows 7:
	Run Cygwin setup program, search for “libsndfile” and install 
 
On Mac OS X:
	Open Terminal window and “cd” to the current directory.  

Install homebrew 
	Go to brew.sh and follow instructions under “Install Homebrew”  

Then execute in terminal:
	brew install libsndfile 


### Compile and link the project code

There's a makefile already created. So simply go to the directory of the source code
and type in 'make' in the Terminal.
