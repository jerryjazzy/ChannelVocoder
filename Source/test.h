

#ifndef _TEST_H
#define _TEST_H

#define PI  					3.14159265358979323846264338327950288
#define SAMPLE_RATE 			44100
#define FRAMES_PER_BUFFER 		1024
#define MAX_CHN					2
#define NUM_BAND				32
#define BUFFER_SIZE				1024	
#define HOP_SIZE				512

struct BUF_tag {
	float *buf[MAX_CHN];
};

typedef struct BUF_tag Buf;

#endif /* _TEST_H */
