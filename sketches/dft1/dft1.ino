/*
*
* File: dft1.ino
* Purpose: DFT - floating point implementation using sin() and cos() functions.
* Version: 1.0.2
* Date: 13-08-2020
* Modified: 24-08-2020
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

//#define NSAMPL 8
//int x[] = { 1, 0, 0, 0, 0, 0, 0, 0 };
//int x[] = { 0, 1, 0, 0, 0, 0, 0, 0 };

#define NSAMPL 16
int x[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//#define NSAMPL 32
//int x[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//#define NSAMPL 64
//int x[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int NS = NSAMPL;
double ReX[NSAMPL];
double ImX[NSAMPL];

int L, K, N;
double angle, delta;
double DC, M;
unsigned long t;


void setup() {
  Serial.begin(115200);
}


void loop() {
  L = NS-1; //full DFT
  //L = (int)(NS/2); //single sided spectrum

  Serial.print("DFT t(us)= ");  
  t = micros();
  for(K=0; K<=L; K++)
    {
    ReX[K] = 0;
    ImX[K] = 0;
    angle = 0;
    delta = 2*M_PI * K/NS;

    for(N=0; N<NS; N++)
      {
      //Serial.println(angle*360/(2*PI), 2);
      ReX[K] += x[N] * cos(angle);
      ImX[K] += x[N] * sin(angle);
      //angle += delta; //negative frequencies first
      angle -= delta; //positive frequencies first
      }
  //Serial.println();
  }
  Serial.println(micros()-t);


  for(K=0; K<=L; K++)
    {
    //Serial.print(ReX[K]/NS, 2);
    Serial.print(ReX[K], 2);
    Serial.print("\t");
    //Serial.print(ImX[K]/NS, 2);
    Serial.print(ImX[K], 2);
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
