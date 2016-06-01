#ifndef _FFT_H_
#define _FFT_H_

typedef float real;
typedef struct{real Re; real Im;} complex;

#ifndef PI
#define PI 3.14159265358979323846264338327950288
#endif

void fft( complex *v, int n, complex *tmp );
void ifft( complex *v, int n, complex *tmp );

#endif /* _FFT_H_ */
