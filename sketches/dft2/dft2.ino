/*
*
* File: dft2.ino
* Purpose: DFT with 32bit DDS and 10bit LUT
* Version: 1.0.4
* Date: 13-08-2020
* Modified: 19-02-2022
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


#define RANGE 4294967296UL  //phase accumulator range = 2^32.
#define PHASE_OFFSET_90 1073741824UL

const int sin_lut1024[] PROGMEM = {
  0, 3, 6, 9, 13, 16, 19, 22,
  25, 28, 31, 34, 38, 41, 44, 47,
  50, 53, 56, 59, 63, 66, 69, 72,
  75, 78, 81, 84, 87, 90, 94, 97,
  100, 103, 106, 109, 112, 115, 118, 121,
  124, 127, 130, 133, 136, 139, 142, 145,
  148, 151, 154, 157, 160, 163, 166, 169,
  172, 175, 178, 181, 184, 187, 190, 193,
  196, 198, 201, 204, 207, 210, 213, 216,
  218, 221, 224, 227, 230, 233, 235, 238,
  241, 244, 246, 249, 252, 255, 257, 260,
  263, 265, 268, 271, 273, 276, 279, 281,
  284, 286, 289, 292, 294, 297, 299, 302,
  304, 307, 309, 312, 314, 317, 319, 322,
  324, 327, 329, 331, 334, 336, 338, 341,
  343, 345, 348, 350, 352, 355, 357, 359,
  361, 364, 366, 368, 370, 372, 374, 377,
  379, 381, 383, 385, 387, 389, 391, 393,
  395, 397, 399, 401, 403, 405, 407, 409,
  410, 412, 414, 416, 418, 420, 421, 423,
  425, 427, 428, 430, 432, 433, 435, 437,
  438, 440, 441, 443, 445, 446, 448, 449,
  451, 452, 454, 455, 456, 458, 459, 461,
  462, 463, 465, 466, 467, 468, 470, 471,
  472, 473, 474, 476, 477, 478, 479, 480,
  481, 482, 483, 484, 485, 486, 487, 488,
  489, 490, 491, 492, 492, 493, 494, 495,
  496, 496, 497, 498, 499, 499, 500, 501,
  501, 502, 502, 503, 503, 504, 505, 505,
  505, 506, 506, 507, 507, 508, 508, 508,
  509, 509, 509, 509, 510, 510, 510, 510,
  510, 511, 511, 511, 511, 511, 511, 511,
  511, 511, 511, 511, 511, 511, 511, 511,
  510, 510, 510, 510, 510, 509, 509, 509,
  509, 508, 508, 508, 507, 507, 506, 506,
  505, 505, 505, 504, 503, 503, 502, 502,
  501, 501, 500, 499, 499, 498, 497, 496,
  496, 495, 494, 493, 492, 492, 491, 490,
  489, 488, 487, 486, 485, 484, 483, 482,
  481, 480, 479, 478, 477, 476, 474, 473,
  472, 471, 470, 468, 467, 466, 465, 463,
  462, 461, 459, 458, 456, 455, 454, 452,
  451, 449, 448, 446, 445, 443, 441, 440,
  438, 437, 435, 433, 432, 430, 428, 427,
  425, 423, 421, 420, 418, 416, 414, 412, 
  410, 409, 407, 405, 403, 401, 399, 397,
  395, 393, 391, 389, 387, 385, 383, 381,
  379, 377, 374, 372, 370, 368, 366, 364,
  361, 359, 357, 355, 352, 350, 348, 345,
  343, 341, 338, 336, 334, 331, 329, 327,
  324, 322, 319, 317, 314, 312, 309, 307,
  304, 302, 299, 297, 294, 292, 289, 286,
  284, 281, 279, 276, 273, 271, 268, 265,
  263, 260, 257, 255, 252, 249, 246, 244,
  241, 238, 235, 233, 230, 227, 224, 221,
  218, 216, 213, 210, 207, 204, 201, 198,
  196, 193, 190, 187, 184, 181, 178, 175,
  172, 169, 166, 163, 160, 157, 154, 151,
  148, 145, 142, 139, 136, 133, 130, 127,
  124, 121, 118, 115, 112, 109, 106, 103,
  100, 97, 94, 90, 87, 84, 81, 78, 
  75, 72, 69, 66, 63, 59, 56, 53,
  50, 47, 44, 41, 38, 34, 31, 28,
  25, 22, 19, 16, 13, 9, 6, 3,
  0, -3, -6, -9, -13, -16, -19, -22,
  -25, -28, -31, -34, -38, -41, -44, -47,
  -50, -53, -56, -59, -63, -66, -69, -72,
  -75, -78, -81, -84, -87, -90, -94, -97,
  -100, -103, -106, -109, -112, -115, -118, -121,
  -124, -127, -130, -133, -136, -139, -142, -145,
  -148, -151, -154, -157, -160, -163, -166, -169,
  -172, -175, -178, -181, -184, -187, -190, -193,
  -196, -198, -201, -204, -207, -210, -213, -216, 
  -218, -221, -224, -227, -230, -233, -235, -238,
  -241, -244, -246, -249, -252, -255, -257, -260,
  -263, -265, -268, -271, -273, -276, -279, -281,
  -284, -286, -289, -292, -294, -297, -299, -302,
  -304, -307, -309, -312, -314, -317, -319, -322,
  -324, -327, -329, -331, -334, -336, -338, -341,
  -343, -345, -348, -350, -352, -355, -357, -359, 
  -361, -364, -366, -368, -370, -372, -374, -377,
  -379, -381, -383, -385, -387, -389, -391, -393,
  -395, -397, -399, -401, -403, -405, -407, -409,
  -410, -412, -414, -416, -418, -420, -421, -423,
  -425, -427, -428, -430, -432, -433, -435, -437,
  -438, -440, -441, -443, -445, -446, -448, -449,
  -451, -452, -454, -455, -456, -458, -459, -461, 
  -462, -463, -465, -466, -467, -468, -470, -471,
  -472, -473, -474, -476, -477, -478, -479, -480,
  -481, -482, -483, -484, -485, -486, -487, -488,
  -489, -490, -491, -492, -492, -493, -494, -495,
  -496, -496, -497, -498, -499, -499, -500, -501,
  -501, -502, -502, -503, -503, -504, -505, -505,
  -505, -506, -506, -507, -507, -508, -508, -508, 
  -509, -509, -509, -509, -510, -510, -510, -510,
  -510, -511, -511, -511, -511, -511, -511, -511,
  -511, -511, -511, -511, -511, -511, -511, -511,
  -510, -510, -510, -510, -510, -509, -509, -509,
  -509, -508, -508, -508, -507, -507, -506, -506,
  -505, -505, -505, -504, -503, -503, -502, -502,
  -501, -501, -500, -499, -499, -498, -497, -496, 
  -496, -495, -494, -493, -492, -492, -491, -490,
  -489, -488, -487, -486, -485, -484, -483, -482,
  -481, -480, -479, -478, -477, -476, -474, -473,
  -472, -471, -470, -468, -467, -466, -465, -463,
  -462, -461, -459, -458, -456, -455, -454, -452,
  -451, -449, -448, -446, -445, -443, -441, -440,
  -438, -437, -435, -433, -432, -430, -428, -427, 
  -425, -423, -421, -420, -418, -416, -414, -412,
  -410, -409, -407, -405, -403, -401, -399, -397,
  -395, -393, -391, -389, -387, -385, -383, -381,
  -379, -377, -374, -372, -370, -368, -366, -364,
  -361, -359, -357, -355, -352, -350, -348, -345,
  -343, -341, -338, -336, -334, -331, -329, -327,
  -324, -322, -319, -317, -314, -312, -309, -307, 
  -304, -302, -299, -297, -294, -292, -289, -286,
  -284, -281, -279, -276, -273, -271, -268, -265,
  -263, -260, -257, -255, -252, -249, -246, -244,
  -241, -238, -235, -233, -230, -227, -224, -221,
  -218, -216, -213, -210, -207, -204, -201, -198,
  -196, -193, -190, -187, -184, -181, -178, -175,
  -172, -169, -166, -163, -160, -157, -154, -151, 
  -148, -145, -142, -139, -136, -133, -130, -127,
  -124, -121, -118, -115, -112, -109, -106, -103,
  -100, -97, -94, -90, -87, -84, -81, -78,
  -75, -72, -69, -66, -63, -59, -56, -53,
  -50, -47, -44, -41, -38, -34, -31, -28,
  -25, -22, -19, -16, -13, -9, -6, -3
};



int NS = NSAMPL;
long ReX_tmp;
long ImX_tmp;
double ReX[NSAMPL];
double ImX[NSAMPL];

int L, K, N;
double DC, M;
unsigned long t;

unsigned long deltaPhase;
unsigned long phaseAccu;
unsigned int phaseIdxI;
unsigned int phaseIdxQ;


void setup() {
  Serial.begin(115200);
  
  L = NS-1; //length for full DFT
  //L = (int)(NS/2); //single sided spectrum          
}


void loop() {
  Serial.print("\nDFT t(us)= ");  
  t = micros();
  for(K=0; K<=L; K++)
    {
    ReX_tmp = 0;
    ImX_tmp = 0;
    deltaPhase = (unsigned long)(RANGE/NS)*K;
    phaseIdxI=0;
    phaseIdxQ = PHASE_OFFSET_90>>22; //adding a fixed phase offset for cos.

    for(N=0; N<NS; N++)
      {
      //Serial.println(phaseIdxI);
      //Serial.println((int)(pgm_read_word(sin_lut1024+phaseIdxQ));
	  
	  //Input data range is limited between -63, .. +64 (multiplication exceeds size of int.)
      ReX_tmp += x[N] * (int)pgm_read_word(sin_lut1024+phaseIdxQ); //cos multiplier
      ImX_tmp += x[N] * (int)pgm_read_word(sin_lut1024+phaseIdxI); //sin multiplier
	  
	  //Use this for full int data range. Takes just a little longer.
	  //ReX_tmp += (long)x[N] * (int)pgm_read_word(sin_lut1024+phaseIdxQ); //cos multiplier.
      //ImX_tmp += (long)x[N] * (int)pgm_read_word(sin_lut1024+phaseIdxI); //sin multiplier
	  
      //phaseAccu += deltaPhase; //negative frequencies first
      phaseAccu -= deltaPhase; //positive frequencies first
      phaseIdxI = phaseAccu>>22;
      phaseIdxQ = (phaseAccu+PHASE_OFFSET_90)>>22; //adding a fixed phase offset for cos.
      }

  //ReX[K] = ReX_tmp >> 9; //rescaling. Use if output >> 1, for input data > 1. 
  //ImX[K] = ImX_tmp >> 9;	  
  ReX[K] = (double)ReX_tmp/511; //rescaling
  ImX[K] = (double)ImX_tmp/511;
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
  Serial.println(DC, 2);

  Serial.println();
  Serial.println("magnitudes: ");
  for(K=1; K<=L; K++)
    {
    M = sqrt( sq(ImX[K]) + sq(ReX[K]) ) / NS;  
    Serial.println(M, 2);
    }

  while(1) {};
}
