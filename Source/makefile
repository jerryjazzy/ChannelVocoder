# Makefile
# Program to run a vocoder
#

CC      = gcc
CFLAGS  = -g -std=c99 -Wall -I.
LIBS    = -L/usr/local/lib -lsndfile

EXE  	= vocoder
SRCS 	= test.c fft.c vocode.c
OBJS 	= test.o fft.o vocode.o
DEPS 	= test.h fft.h vocode.h

#compile *.c to *.o
# the -o $@ says to put the output of the compilation in the file named 
# 	on the left side of the :
# the $< is the first item in the dependencies list, 
#
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

clean:
	rm -f *~ core $(EXE) *.o
	rm -rf *.dSYM
