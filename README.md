# FFTA - Fastest(?) Fourier Transform for Arduino

# Introduction
This project is about a fast C implementation of the Discrete Fourier Transform[1],[2] for Arduino.
FFTA computes the DFT from *real* input data, for example data that comes from the (10-bit) ADC. The fastest time of an 8-point DFT with FFTA measured is 304μs.

With the 8-points data set example: x=[0, 1, 0, 0, 0, 0, 0, 0], the MATLAB function fft(x) returns:

*1.0000+0.0000i   0.7071-0.7071i   0.0000-1.0000i  -0.7071-0.7071i  -1.0000+0.0000i  -0.7071+0.7071i   0.0000+1.0000i   0.7071+0.7071i*

the *dft4* sketch for example, returns:

*1.00  0.00i*
*0.71  -0.71i*
*0.00  -1.00i*
*-0.71  -0.71i*
*-1.00  0.00i*
*-0.71  0.71i*
*0.00  1.00i*
*0.71  0.71i*

A very clear, not mathematical explanation of the FFT is given here[3].
Basically, the DFT formula can be seen as two multipliers and two wave generators stepping in frequency and thereby averaging the outcome for each frequency bin from the frequency spectrum. An electronic device analogy of this is a two phase/vector-lock-in-amplifier with a sweeped LO for the one who knows...

![DFT-formula](figures/DFT_formula.png  "DFT-formula")

The DFT-core implemented in the C-language looks like:

![DFT-loops](figures/dft-loop.png  "DFT-loops")

For FFTA the trigonometric functions and the angle stepping are replaced by Direct Digital Synthesis wave generators using a look-up table[5], which is super fast on an 8-bit micro-controller.
Sketch *dft2* with the 32-bit phase accumulator has a higher frequency step resolution and is because of this more accurate when transforming larger input vectors (to be confirmed).

There is no internal overflow check on numbers. Input data range must be verified. The recommended input data types are given in the table below under Sketches.

# Sketches
Sketch | purpose
------ | -------
dft1.ino | DFT - floating point implementation using sin() and cos() functions. Input data type: *double* or *int*
dft2.ino | DFT with 32bit DDS, 10bit Look-up table (LUT). Input data is of type *int*
dft3.ino | DFT with 16bit DDS, 10bit LUT. Input data is of type *int*
dft4.ino | DFT with 16bit DDS, 8bit LUT. Input data is of type *char*
dft5.ino | DFT with 16bit DDS, 8bit LUT, only calculates the positive frequencies. Internal ReX and ImX accumulators are reduced to type of *int*. Input data is of type *char*

# Benchmarks
The measured execution times in μs of the different FFTA variants running on the Arduino UNO are shown in the table below. The times mentioned under *dft1 - dft4* are for the full spectrum calculation (positive and negative frequencies).
The data set that was used for bench-marking is: x = 0, 1, 0, 0, ...  [4].

N-points: / Implementation: | dft1 | dft2 | dft3 | dft4 | dft5
--------------------------- | ---- | ---- | ---- | ---- | ----
8 | 16436 | 1996 | 968 | 720 | 304
16 | 66040 | 7552 | 3500 | 2568 | 992
32 | 265880 | 29636 | 13548 | 9812 | 3420
64 | 1073796 | 117644 | 53540 | 38600 | 12868

### Conclusion
The computing time of *dft1 - dft4* scales with N^2 as expected.
The computing time of the well known FFT algorithm scales with N*log2(N) [1] and is much faster for larger values of N. A FFT for Arduino named EasyFFT and implemented by ABHILASH[6],  is faster because of this scaling law. The output from FFTA seems to be more accurate.
Using DDS wave generators for the sin and cosine multipliers is just an idea that I wanted to test. It might be interesting with other applications, for example in detecting a singe signal frequency.

[1]: https://en.wikipedia.org/wiki/Fast_Fourier_transform

[2]: http://www.fftw.org/

[3]: https://www.earlevel.com/main/2002/08/31/a-gentle-introduction-to-the-fft/

[4]: http://www.sccon.ca/sccon/fft/fft3.htm

[5]: https://github.com/MartinStokroos/NativeDDS

[6]: https://create.arduino.cc/projecthub/abhilashpatel121/easyfft-fast-fourier-transform-fft-for-arduino-9d2677
