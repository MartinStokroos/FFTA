# FFTA - Fastest(?) Fourier Transform for Arduino

# Introduction
This project is about a fast C implementation of the Discrete Fourier Transform[1],[2] for Arduino.
FFTA computes the DFT from *real* input data, for example data that comes from the (10-bit) ADC. The fastest time measured for an 8-point DFT with FFTA is 844μs.

With the 8-points data set example: *x=[0, 1, 0, 0, 0, 0, 0, 0]*, MATLAB's fft(x) returns:

*1.0000+0.0000i, 0.7071-0.7071i, 0.0000-1.0000i, -0.7071-0.7071i, -1.0000+0.0000i, -0.7071+0.7071i, 0.0000+1.0000i, 0.7071+0.7071i*

and the output from example *dft5.ino* is:

*1.00  0.00i, 0.71  -0.71i, 0.00  -1.00i, -0.71  -0.71i, -1.00  0.00i, -0.71  0.71i, 0.00  1.00i, 0.71  0.71i*

A very clear and non mathematical explanation about the FFT is given here:[3]. 
The DFT formula can be represented as two multipliers with two frequency stepping local oscillators, thereby averaging the output of each frequency bin in the frequency spectrum. An electronic device analogy of this process is a two-phase/vector lock-in amplifier with a stepped LO...

![DFT-formula](figures/DFT_formula.png  "DFT-formula")

The DFT-core implemented in the C-language looks like:

![DFT-loops](figures/dft-loop.png  "DFT-loops")

In FFTA, the trigonometric functions and the phase stepping is replaced for a Direct Digital Synthesis wave generator, using a look-up table[5], which is ultra fast on a micro-controller. Sketch *dft2*, with the 32-bit phase accumulator, has the highest frequency resolution and is more accurate when transforming larger input vectors.
There is no internal overflow check on numbers. Some comments on the data range are mentioned in the source files. The recommended input data types for each example, are given in the table below under *Sketches*.

# Sketches
Study Sketches | Remarks 
------ | -------
dft1.ino | DFT - floating point implementation using sin() and cos() functions. Input data type: *double* or *int*
dft2.ino | DFT with 32bit DDS, 10bit Look-up table (LUT). Input data is of type *int*
dft3.ino | DFT with 16bit DDS, 10bit LUT. Input data is of type *int*
dft4.ino | DFT with 16bit DDS, 8bit LUT. Input data is of type *char*
dft5.ino | DFT with 16bit DDS, 8bit LUT. ReX and ImX accumulators are reduced to type *int*. Input data is of type *char*. 
 **Application** |
spectrum.ino | Application example of DFT with 32 bit DDS, 10-bit LUT. Input data is from the ADC input A0. Apply AC voltage on 2.5V DC bias.<br />Defaults: 64 points DFT, fs=8ksps, Hanning windowing and log scale. SPECTRUM PLOTTING DOES NOT WORK IN THE ARDUINO MONITOR. Use a VT100 compatible terminal emulator like minicom or PuTTY. 

Example output for Udc = 2.5V + Uac 1V, f sinewave = 2kHz. (Spectrum is now greatly enhanced compared to picture below.)

![Spectrum](figures/spectrum.png  "Spectrum output")

# Benchmarks

The measured execution times in μs of the different FFT variants running on the Arduino UNO are shown in the table below. The times noted in the table, are for the full spectrum calculation (positive and negative frequencies). The data set that has been used for bench-marking is: *x = 0, 1, 0, 0, ...*  [4].

Implementation / N-points: | 8 | 16 | 32 | 64
-------------------------- | - | -- | -- | --
dft1 | 16436 | 66056 | 265968 | 1074168
dft2 | 2228 | 7692 | 28368 | 108520
dft3 | 1144 | 3428 | 11440 | 41052
dft4 | 844 | 2304 | 6922 | 23400
dft5 | 784 | 2096 | 6184 | 20236
EasyFFT [6] (measured time of FFT function, excluding local variable declarations and Serial.print functions) | 1624 | 3800 | 8628 | 19288

![Dft4 vs. EasyFFT](figures/dft4_vs_easyfft.png  "Dft4 vs. EasyFFT")

# Conclusion
The computing time of the *dft1 to dft5* programs do scale with N^2 as was expected. The computing time of the well known FFT algorithm[1] scales with N*log2(N) and is much faster for larger N values. The accuracy of the FFTA programs seems not bad, even with a 16bit DDS/8-bit LUT.
The FFT program for Arduino named EasyFFT by ABHILASH[6], outperforms FFTA for N>32. Beyond N=64 or N=128, the Arduino will sooner or later run out of memory for FFT programs achieving a comparable accuracy. So, for small processors and N up to 32, FFTA is a good alternative.

Using a DDS wave generator in place of angle stepped sin and cosine functions, is just an idea that I wanted to test because of its compact and elegant implementation form. This idea might be interesting for other applications.

[1]: https://en.wikipedia.org/wiki/Fast_Fourier_transform

[2]: http://www.fftw.org/

[3]: https://www.earlevel.com/main/2002/08/31/a-gentle-introduction-to-the-fft/

[4]: http://www.sccon.ca/sccon/fft/fft3.htm

[5]: https://github.com/MartinStokroos/NativeDDS

[6]: https://create.arduino.cc/projecthub/abhilashpatel121/easyfft-fast-fourier-transform-fft-for-arduino-9d2677
