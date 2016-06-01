#ifndef _VOCODE_H
#define _VOCODE_H


void vocode(Buf *modulator, Buf *carrier, int bufSize, int hopSize, 
		int numBand, int numChan,int numFrames, Buf *obuf);


#endif /* _VOCODE_H */