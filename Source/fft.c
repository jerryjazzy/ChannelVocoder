
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "fft.h"

/* Multiply two complex numbers */
// complex complex_mult(complex a, complex b)
// {
//   complex c;
//   c.Re =  (a.Re * b.Re) + (a.Im * b.Im * -1);
//   c.Im =  a.Re * b.Im + a.Im * b.Re;
//   return c;
// }

/* 
   fft(v,N):
   [0] If N==1 then return.
   [1] For k = 0 to N/2-1, let ve[k] = v[2*k]
   [2] Compute fft(ve, N/2);
   [3] For k = 0 to N/2-1, let vo[k] = v[2*k+1]
   [4] Compute fft(vo, N/2);
   [5] For m = 0 to N/2-1, do [6] through [9]
   [6]   Let w.re = cos(2*PI*m/N)
   [7]   Let w.im = -sin(2*PI*m/N)
   [8]   Let v[m] = ve[m] + w*vo[m]
   [9]   Let v[m+N/2] = ve[m] - w*vo[m]
 */

void fft( complex *v, int n, complex *tmp )
{
  if(n > 1) {			/* otherwise, do nothing and return */
    int k,m;    
    complex z, w, *vo, *ve;
    ve = tmp; 
    vo = tmp + n/2;
    for(k = 0; k < n/2; k++) {
      ve[k] = v[2*k];
      vo[k] = v[2*k+1];
    }
    fft(ve, n/2, v);		/* FFT on even-indexed elements of v[] */
    fft(vo, n/2, v);		/* FFT on odd-indexed elements of v[] */
    for(m=0; m<n/2; m++) {
      w.Re = cos(2 * PI * m/(double)n);
      w.Im = -sin(2 * PI * m/(double)n);
      z.Re = w.Re*vo[m].Re - w.Im*vo[m].Im;	/* Re(w*vo[m]) */
      z.Im = w.Re*vo[m].Im + w.Im*vo[m].Re;	/* Im(w*vo[m]) */
      v[m].Re = ve[m].Re + z.Re;
      v[m].Im = ve[m].Im + z.Im;
      v[m+n/2].Re = ve[m].Re - z.Re;
      v[m+n/2].Im = ve[m].Im - z.Im;
    }
  }
  return;
}

/* 
   ifft(v,N):
   [0] If N == 1 then return.
   [1] For k = 0 to N/2-1, let ve[k] = v[2*k]
   [2] Compute ifft(ve, N/2);
   [3] For k = 0 to N/2-1, let vo[k] = v[2*k+1]
   [4] Compute ifft(vo, N/2);
   [5] For m = 0 to N/2-1, do [6] through [9]
   [6]   Let w.re = cos(2*PI*m/N)
   [7]   Let w.im = sin(2*PI*m/N)
   [8]   Let v[m] = ve[m] + w*vo[m]
   [9]   Let v[m+N/2] = ve[m] - w*vo[m]
 */
void ifft(complex *v, int n, complex *tmp)
{
  if(n > 1) {			/* otherwise, do nothing and return */
    int k, m;    
    complex z, w, *vo, *ve;
    ve = tmp; vo = tmp + n/2;
    for(k = 0; k < n/2; k++) {
      ve[k] = v[2*k];
      vo[k] = v[2*k+1];
    }
    ifft(ve, n/2, v);		/* FFT on even-indexed elements of v[] */
    ifft(vo, n/2, v);		/* FFT on odd-indexed elements of v[] */
    for(m = 0; m < n/2; m++) {
      w.Re = cos(2 * PI * m/(double)n);
      w.Im = sin(2 * PI * m/(double)n);
      z.Re = w.Re*vo[m].Re - w.Im*vo[m].Im;	/* Re(w*vo[m]) */
      z.Im = w.Re*vo[m].Im + w.Im*vo[m].Re;	/* Im(w*vo[m]) */
      v[m].Re = ve[m].Re + z.Re;
      v[m].Im = ve[m].Im + z.Im;
      v[m+n/2].Re = ve[m].Re - z.Re;
      v[m+n/2].Im = ve[m].Im - z.Im;
    }
  }
  return;
}
