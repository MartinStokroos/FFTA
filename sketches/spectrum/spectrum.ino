/*
* File: spectrum.ino
* Purpose: DFT Spectrum analyzer
* Version: 1.0.0
* Date: 24-02-2022
* Modified:
* 
* Created by: Martin Stokroos
* 
* URL: https://github.com/MartinStokroos/FFTA
*
* MIT License
*
* Copyright (c) 2022 Martin Stokroos
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
* - The ascii terminal output of this program works best with a VT100 compatible terminal emulator like minicom or putty.
* - Apply a DC+AC voltage (0 <= U <= 5V) to input A0.
* - Monitor the sample clock rate (2*fs) on pin 9.
*
*/

#define ADC_CH0 0   //MUX3:0: Analog Channel Selection Bits
#define PIN_LED 13
#define PIN_SCLK 9  //sampling clock output
#define RANGE 4294967296UL  //phase accumulator range = 2^32.
#define PHASE_OFFSET_90 1073741824UL

//DFT
#define CR 1000 //fs = 8000000/CR = 8kHz 
#define WINDOWING // comment/uncomment to select
#define LOG // comment/uncomment to select lin/log scale
const byte DFT_BITS  =  6;  //DFT size, number of bits.
const size_t NS = 1 << DFT_BITS; //Number of Samples
const byte DFT_SCALING = 9 + DFT_BITS;

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

int K, N;
int L = (NS >> 1);
long ReX[NS];
long ImX[NS];
int X[(NS >> 1) + 1];
unsigned long winAcc;
unsigned long winStep = (unsigned long)( (RANGE >> 1) / (NS - 1) ); //step size for cosine window table.
unsigned int winIdx;
unsigned long wtmp;
unsigned long deltaPhase;
unsigned long phaseAccu;
unsigned int phaseIdxI;
unsigned int phaseIdxQ;
volatile int samplIdx = 0;
volatile int adcInBuff[NS];
volatile bool adcRdy = false;

struct Polar {
  int radius;
  unsigned char angle;
};
struct Polar ans;



/*
 * Setup
 */
void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  Serial.begin(115200);
  cli(); // disable interrupts
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * ADC initialization
   * ADC triggered by timer1
   * ADC sample clock prescaling=128
   * ADC clock runs at 16MHz/128=125kHz
   * One conversion takes 13 clock periods. conversion time=13*8us=104us.
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  DIDR0 = 0x3F; // digital inputs disabled for ADC0D to ADC5D
  bitSet(ADMUX, REFS0); //Select Vcc=5V as the ADC reference voltage
  bitClear(ADMUX, REFS1); //Set to 1, together with REFS0, for internal Vref=1.1V.
  ADMUX = (ADMUX & B11110000) | ADC_CH0; //select the ADC channel.
  bitSet(ADCSRA, ADEN);  // AD-converter enabled
  bitSet(ADCSRA, ADATE); // auto-trigger enabled
  bitSet(ADCSRA, ADIE);  // ADC interrupt enabled
  bitSet(ADCSRA, ADPS0);
  bitSet(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS2);
  // set trigger source to 'Timer 1 overflow' to sync ADC to Timer 1
  bitClear(ADCSRB, ADTS0);
  bitSet(ADCSRB, ADTS1);
  bitSet(ADCSRB, ADTS2);
  // start conversion
  bitSet(ADCSRA, ADSC);
  /*
   * TIMER1 initialization for mode 14.
   * Timer 1 generates the sampling clock
   * prescaling=1, f=fclk/Ns, OCRoffset=Ns/2 for 50% duty cycle on pin OC1A
   * output on OC1A (pin 9)
   */
  TCCR1A = 0b10000010; //fast PWM, TOP=ICR1, Clear OC1A on compare match (set output to low).
  TCCR1B = 0b00011001; //no pre scaling, CTC
  ICR1 = CR - 1 ; // sample period pre divider
  OCR1A = (CR >> 1);
  bitSet(TIMSK1, TOIE1); // enable Timer1 Interrupt
  sei(); // enable all interrupts

  Serial.print("\x1B[2J"); //clear terminal screen.
}



/*
 * Main loop
 */
void loop() {
  if (adcRdy)
  {
#ifdef WINDOWING
    //Cosine window
    winIdx = 0;
    winAcc = 0;
    for (N = 0; N < NS; N++) {
      //Serial.print(adcInBuff[N]);
      //Serial.print("\t");
      wtmp = ( (long)adcInBuff[N] * (int)pgm_read_word(sin_lut1024 + winIdx) );
      winAcc += winStep;
      winIdx = winAcc >> 22;
      adcInBuff[N] = wtmp >> 9;
      //Serial.println(adcInBuff[N]);
    }
#endif

    digitalWrite(PIN_LED, true);
    // start the DFT
    for (K = 0; K <= L; K++)
    {
      ReX[K] = 0;
      ImX[K] = 0;
      deltaPhase = (unsigned long)(RANGE / NS) * K;
      phaseIdxI = 0;
      phaseIdxQ = PHASE_OFFSET_90 >> 22; //adding a fixed phase offset for cos.

      for (N = 0; N < NS; N++)
      {
        //Serial.println(phaseIdxI);
        //Serial.println((int)(pgm_read_word(sin_lut1024 + phaseIdxQ));
        // DFT of ADC data:
        ReX[K] += (long)adcInBuff[N] * (int)pgm_read_word(sin_lut1024 + phaseIdxQ); //cos multiplier
        ImX[K] += (long)adcInBuff[N] * (int)pgm_read_word(sin_lut1024 + phaseIdxI); //sin multiplier
        //phaseAccu += deltaPhase; //negative frequencies first
        phaseAccu -= deltaPhase; //positive frequencies first
        phaseIdxI = phaseAccu >> 22;
        phaseIdxQ = (phaseAccu + PHASE_OFFSET_90) >> 22; //adding a fixed phase offset for cos.
      }
      ReX[K] = ReX[K] >> DFT_SCALING; //scale back to 10 bits max.
      ImX[K] = ImX[K] >> DFT_SCALING;

      ans = cordic( (int)ReX[K], (int)ImX[K] );
      //Serial.print(ans.radius);
#ifdef LOG
      X[K] = Log2( (unsigned long)ans.radius + 1 ); //calculate 100*ld(x)
#endif
#ifndef LOG
      X[K] = ans.radius;
#endif
    }// end of DFT
    digitalWrite(PIN_LED, false);

    for (K = 0; K <= L; K++)
    {
      //Serial.print(ReX[K]);
      //Serial.print("\t");
      //Serial.print(ImX[K]);
      //Serial.print("i");
      //Serial.print("\t");
      //Serial.print( sqrt(sq(ImX[K]) + sq(ReX[K])) ); //alternative way to calculate the absolute values.

      //plot spectrum vertical
      Serial.print("\x1B[0K"); //clear line
      for (N = 0; N <= (int)(X[K] / 50); N++) {
        Serial.print("*");
      }
      Serial.println(); //LF+CR
    }
    Serial.print("\x1B[H"); //go to home position
    adcRdy = false;
  }
}



/*
 *  8-bit CORDIC routine
 *  Calculates the angle and radius of the vector pointing towards (x,y)
 *  The angle is returned as an unsigned char. 256 means 2pi or 360 degrees.
 *  radius is the length of the vector as int.
 *
 *  The cordic function used here is based on the CORDIC method that was provided with
 *  the course material about 'Software Defined Radio with AVR' [Ossmann 2012].
 *
 *  Software Defined Radio met AVR, Deel 4: Digitale radio-ontvangst,
 *  DCF77, weerdienst en meer..., Martin Ossmann, Elektor(NL) June 2012.
 */
const unsigned int sin_lut[7] = {12540, 6393, 3212, 1608, 804, 402, 201};
const unsigned int cos_lut[7] = {30274, 32138, 32610, 32729, 32758, 32766, 32767};

struct Polar cordic(int x0, int y0) {
  unsigned char angle = 0, phi, currentAngle, k;
  bool sign = false;
  unsigned int radius;
  int temp;
  long x1, y1;

  if (y0 < 0) {
    x0 = -x0;  //rotate 128 cw
    y0 = -y0;
    angle = 128;
  }
  if (x0 < 0) {
    temp = x0;  //rotate 64 cw
    x0 = y0;
    y0 = -temp;
    angle += 64;
  }
  if (y0 > x0) {
    sign = true;
    angle += 64;
    temp = x0;
    x0 = y0;
    y0 = temp;
  }
  currentAngle = 16; phi = 0; k = 0;
  while (k <= 4) {
    x1 = (long)cos_lut[k] * (long)x0;
    x1 += (long)sin_lut[k] * (long)y0;
    y1 = -(long)sin_lut[k] * (long)x0;
    y1 += (long)cos_lut[k] * (long)y0;
    if (y1 >= 0) {
      x0 = x1 >> 15;
      y0 = y1 >> 15;
      phi += currentAngle;
    }
    currentAngle = currentAngle >> 1; //means: currAngle=currAngle/2;
    k++;
  }
  angle += ((sign) ? -phi : +phi); // angle 2pi = 256, size of char
  Polar result = {x0, angle};
  return result;
}



/*
 *  LOG2 function
 *  
 *  The log2 function used here, was taken from the course material about
 *  'Software Defined Radio with AVR' [Ossmann 2012].
 *
 *  Software Defined Radio met AVR, Deel 4: Digitale radio-ontvangst,
 *  DCF77, weerdienst en meer..., Martin Ossmann, Elektor(NL) June 2012.
 */
const unsigned char LogTab[16] = { 0, 9, 17, 25, 32, 39, 46, 52, 58, 64, 70, 75, 81, 86, 91, 95 };

unsigned int Log2(unsigned long x) {
  // compute 100*ld(x) , so Log2(1024) = 1000
  unsigned int v = 3100;
  // shift until a bit in MSB-byte is set, each byte shift results in log += 800
  while ( (x & 0xff000000L ) == 0 ) {
    x = x << 8;
    v -= 800;
  }
  // shift until most significant bit is set, each bit-shift results in log += 100
  while ( (x & 0x80000000L ) == 0 ) {
    x = x << 1;
    v -= 100;
  }
  // x has now form 1iii ixxx xxxx xxxx xxxx xxxx xxxx xxxx
  // get the next 4 bits =iiii and address table with it
  unsigned char i = (x >> 27) & 0xf;
  v += LogTab[i];
  return v;
}



/*
 *  ISR ADC
 *  The conversion is triggered by Timer1
 */
ISR(ADC_vect) {
  if (!adcRdy)
  {
    // read the selected ADC input channel
    adcInBuff[samplIdx] = ADCL; // store low byte;
    adcInBuff[samplIdx] += ADCH << 8; // store high byte
    samplIdx++;

    if (samplIdx >= NS) {
      adcRdy = true;
      samplIdx = 0;
    }
  }
}



/*
 *  ISR Timer1
 */
ISR(TIMER1_OVF_vect) {
  //empty ISR. Only used to trigger the ADC.
}
