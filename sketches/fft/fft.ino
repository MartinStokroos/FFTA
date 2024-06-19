/*

  File: fft.ino
  Purpose: FFT with 32bit DDS and 10bit LUT, real+imag doubles input
  Version: 1.0.0
  Date: 17-06-2024
  Modified: -

  Created by: Martin Stokroos
  URL: https://github.com/MartinStokroos/FFTA

  MIT License

  Copyright (c) 2024 Martin Stokroos

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#define NUM_SAMPLES 8           // the number of samples
#define NUM_SAMPLES_M_1 NUM_SAMPLES-1       // the number of samples minus one
#define LOG2_NUM_SAMPLES    3   // log2 of the number of gathered samples
#define SHIFT_AMOUNT       13   // length of short (16 bits) minus log2 of number of samples

// input arrays
double fr[NUM_SAMPLES] = { 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // array for real part of samples
//double fr[NUM_SAMPLES] = { 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 }; // array for real part of samples
//double fr[NUM_SAMPLES] = { 4.4, 2.3, -3.5, -1.2, -2.8, 2.0, 1.6, 2.5 }; // array for real part of samples
double fi[NUM_SAMPLES] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // array for imaginary part of samples


// proto types
void bitreversal(double *, double *);
void fft(double *, double *);


int s;
unsigned long t;

void setup() {
  Serial.begin(115200);
}


void loop() {
  t = micros();

  bitreversal(fr, fi);
  fft(fr, fi);

  Serial.print("\nFFT t(us)= ");
  Serial.println(micros() - t);

  for (s = 0; s < NUM_SAMPLES; s++)
  {
    Serial.print(fr[s], 2);
    Serial.print("\t");
    Serial.println(fi[s], 2);
  }

  while (1) {};
}



//////////////////////////////////////////////////////////////////////////
////////////////////////// BIT REVERSAL //////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Modified for Arduino based on: https://vanhunteradams.com/FFT/FFT.html
// which is based on code found here:
// https://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
//////////////////////////////////////////////////////////////////////////
void bitreversal(double *fr, double *fi) {
  unsigned int m;   // one of the indices being swapped
  unsigned int mr; // the other index being swapped (r for reversed)
  double rtemp; // for temporary storage while swapping
  double itemp;

  for (m = 1; m < NUM_SAMPLES_M_1; m++)
  {
    // swap odd and even bits
    mr = ((m >> 1) & 0x5555) | ((m & 0x5555) << 1);
    // swap consecutive pairs
    mr = ((mr >> 2) & 0x3333) | ((mr & 0x3333) << 2);
    // swap nibbles ...
    mr = ((mr >> 4) & 0x0F0F) | ((mr & 0x0F0F) << 4);
    // swap bytes
    mr = ((mr >> 8) & 0x00FF) | ((mr & 0x00FF) << 8);
    // shift down mr
    mr >>= SHIFT_AMOUNT;
    // don't swap that which has already been swapped
    if (mr <= m) continue;

    // swap the bit-reveresed indices
    rtemp = fr[m];
    fr[m] = fr[mr];
    fr[mr] = rtemp;
    itemp = fi[m];
    fi[m] = fi[mr];
    fi[mr] = itemp;
  }
}


//////////////////////////////////////////////////////////////////////////
////////////////////////// Danielson-Lanczos FFT//////////////////////////
//////////////////////////////////////////////////////////////////////////
// Modified code for Arduino from: https://vanhunteradams.com/FFT/FFT.html
// based on code from:
// Tom Roberts 11/8/89 and Malcolm Slaney 12/15/94 malcolm@interval.com
//////////////////////////////////////////////////////////////////////////
void fft(double *fr, double *fi)
{
  int L; // length of the FFT's being combined (starts at 1)
  int istep ; // length of the FFT which results from combining two FFT's
  int k, m;
  int i, j ; // indices being combined in Danielson-Lanczos part of the algorithm
  double tr, ti;
  double phase, wr, wi ; // trigonometric values
  double qr, qi ; // temporary variables used during DL part of the algorithm

  // Length of the FFT's being combined (starts at 1)
  L = 1 ;
  // Log2 of number of samples, minus 1
  k = LOG2_NUM_SAMPLES - 1 ;
  // While the length of the FFT's being combined is less than the number
  // of gathered samples . . .
  while (L < NUM_SAMPLES) {
    // Determine the length of the FFT which will result from combining two FFT's
    istep = L << 1;

    // For each element in the FFT's that are being combined . . .
    for (m = 0; m < L; ++m) {
      // Lookup the trig values for that element
      j = m << k;                         // index of the sine table

      phase = TWO_PI * j / NUM_SAMPLES;
      wr = cos(phase); // cos(2pi m/N)
      wi = -sin(phase); // sin(2pi m/N)
      //Serial.print(wr, 2);
      //Serial.print("\t");
      //Serial.println(wi, 2);

      // i gets the index of one of the FFT elements being combined
      for (i = m; i < NUM_SAMPLES; i += istep) {
        // j gets the index of the FFT element being combined with i
        j = i + L;

        // compute the trig terms (bottom half of the above matrix)
        tr = wr * fr[j] - wi * fi[j];
        ti = wr * fi[j] + wi * fr[j];

        qr = fr[i];
        qi = fi[i];

        // compute the new values at each index in place
        fr[j] = qr - tr;
        fi[j] = qi - ti;
        fr[i] = qr + tr;
        fi[i] = qi + ti;
      }
    }
    --k;
    L = istep;
  }
  // end of fft
}
