/*
* File: dft6.ino
* Purpose: DFT with 16bit DDS and 8bit LUT
* Version: 1.0.0
* Date: 24-08-2020
* 
* Created by: Martin Stokroos
* URL: https://github.com/MartinStokroos/FFTA
*
* MIT License
*
* Copyright (c) 2020 Martin Stokroos
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#define NSAMPL 8
//int x[] = { 1, 0, 0, 0, 0, 0, 0, 0 };
int x[] = { 0, 1, 0, 0, 0, 0, 0, 0 };

//#define NSAMPL 16
//int x[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//#define NSAMPL 32
//int x[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//#define NSAMPL 64
//int x[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


#define RANGE 65536  //phase accumulator range = 2^16.
#define PHASE_OFFSET_90 16384


const int sin_lut256[] PROGMEM = {
  0, 3, 6, 9, 12, 16, 19, 22,
  25, 28, 31, 34, 37, 40, 43, 46,
  49, 51, 54, 57, 60, 63, 65, 68,
  71, 73, 76, 78, 81, 83, 85, 88,
  90, 92, 94, 96, 98, 100, 102, 104,
  106, 107, 109, 111, 112, 113, 115, 116,
  117, 118, 120, 121, 122, 122, 123, 124,
  125, 125, 126, 126, 126, 127, 127, 127,
  127, 127, 127, 127, 126, 126, 126, 125,
  125, 124, 123, 122, 122, 121, 120, 118,
  117, 116, 115, 113, 112, 111, 109, 107,
  106, 104, 102, 100, 98, 96, 94, 92,
  90, 88, 85, 83, 81, 78, 76, 73, 71,
  68, 65, 63, 60, 57, 54, 51, 49, 46,
  43, 40, 37, 34, 31, 28, 25, 22, 19,
  16, 12, 9, 6, 3, 0, -3, -6,
  -9, -12, -16, -19, -22, -25, -28, -31,
  -34, -37, -40, -43, -46, -49, -51, -54,
  -57, -60, -63, -65, -68, -71, -73, -76,
  -78, -81, -83, -85, -88, -90, -92, -94,
  -96, -98, -100, -102, -104, -106, -107, -109,
  -111, -112, -113, -115, -116, -117, -118, -120,
  -121, -122, -122, -123, -124, -125, -125, -126,
  -126, -126, -127, -127, -127, -127, -127, -127,
  -127, -126, -126, -126, -125, -125, -124, -123,
  -122, -122, -121, -120, -118, -117, -116, -115,
  -113, -112, -111, -109, -107, -106, -104, -102,
  -100, -98, -96, -94, -92, -90, -88, -85,
  -83, -81, -78, -76, -73, -71, -68, -65,
  -63, -60, -57, -54, -51, -49, -46, -43,
  -40, -37, -34, -31, -28, -25, -22, -19,
  -16, -12, -9, -6, -3
};


int NS = NSAMPL;
int ReX_tmp; //data type reduced to int
int ImX_tmp; //data type reduced to int
double ReX[NSAMPL];
double ImX[NSAMPL];

int L, K, N;
double DC, M;
unsigned long t;

unsigned int deltaPhase;
unsigned int phaseAccu;
unsigned int phaseIdxI;
unsigned int phaseIdxQ;


void setup() {
  Serial.begin(115200);
}


void loop() {
  //L = NS-1; //full DFT
  L = (int)(NS/2); //single sided spectrum

  Serial.print("\nDFT t(us)= ");  
  t = micros();
  for(K=0; K<=L; K++)
    {
    ReX_tmp = 0;
    ImX_tmp = 0;
    deltaPhase = (unsigned int)(RANGE/NS)*K;
    phaseIdxI = 0;
    phaseIdxQ = PHASE_OFFSET_90>>8;

    for(N=0; N<NS; N++)
      {
      //Serial.println(phaseAccu);
      ReX_tmp += x[N] * (int)pgm_read_word(sin_lut256+phaseIdxQ); //cos multiplier
      ImX_tmp += x[N] * (int)pgm_read_word(sin_lut256+phaseIdxI); //sin multiplier
      //phaseAccu += deltaPhase; //negative frequencies first
      phaseAccu -= deltaPhase; //positive frequencies first
      phaseIdxI = phaseAccu>>8;
      phaseIdxQ = (phaseAccu+PHASE_OFFSET_90)>>8; //adding a fixed phase offset for cos.
      }
  ReX[K] = (double)ReX_tmp/127; //rescaling
  ImX[K] = (double)ImX_tmp/127;
  }
  Serial.println(micros()-t);

  for(K=0; K<=L; K++)
    {
    Serial.print(ReX[K], 2);
    //Serial.print(ReX[K]/NS, 2);
    Serial.print("\t");
    Serial.print(ImX[K], 2);
    //Serial.print(ImX[K]/NS, 2);
    Serial.println("i");   
    }

  Serial.println(); 
  DC = ReX[0]/NS;
  Serial.print("DC= ");
  Serial.println(DC, 3);

  Serial.println();
  Serial.println("magnitudes: ");
  for(K=1; K<=L; K++)
    {
    M = sqrt( sq(ImX[K]) + sq(ReX[K]) ) / NS;  
    Serial.println(M, 3);
    }

  while(1) {};
}
