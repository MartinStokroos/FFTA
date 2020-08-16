# FFTA - Fastest(?) Fourier Transform for Arduino

# Introduction
This project is about a fast C implementation of the Discrete Fourier Transform[1], [2] for the Arduino. FFTA computes the DFT from *real* input data, for example data that comes from the (10-bit) ADC.
The fastest time of an 8-point DFT with FFTA measured is 304μs.

With the 8-points data set example: x=[0, 1, 0, 0, 0, 0, 0, 0], the MATLAB function fft(x) returns:

*1.0000+0.0000i   0.7071-0.7071i   0.0000-1.0000i  -0.7071-0.7071i  -1.0000+0.0000i  -0.7071+0.7071i   0.0000+1.0000i   0.7071+0.7071i*

the *dft4* sketch, for example, returns:

*1.00,	0.00*
*0.71,	-0.71*
*0.00,	-1.00*
*-0.71,	-0.71*
*-1.00,	0.00*
*-0.71,	0.71*
*0.00,	1.00*
*0.71,	0.71*

Basically, the DFT formula is nothing more than two wave generators stepping in frequency and calculating the 'power' in each frequency bin from the frequency spectrum.

![DFT-formula](figures/DFT_formula.png  "DFT-formula")

The DFT-core implemented in the C-language looks like:

![DFT-loops](figures/dft-loop.png  "DFT-loops")

For FFTA the trigonometric functions and the angle stepping are replaced by Direct Digital Synthesis wave generators using a look-up table[4], which is super fast on an 8-bit micro-controller.
Sketch *dft2* with the 32-bit phase accumulator has a higher frequency step resolution and is because of this more accurate when transforming larger input vectors (to be confirmed).

There is no internal overflow check on numbers. Input data range must be verified. The recommended input data types are given in the table below under Sketches.

# Sketches
Sketch | purpose
------ | -------
dft1.ino | DFT - floating point implementation using sin() and cos() functions. Input data type: double or int
dft2.ino | DFT with 32bit DDS, 10bit Look-up table (LUT). Input data size is of type int
dft3.ino | DFT with 16bit DDS, 10bit LUT. Input data size is of type int
dft4.ino | DFT with 16bit DDS, 8bit LUT. Input data size is of type char
dft5.ino | DFT with 16bit DDS, 8bit LUT giving positive frequencies only. Internal ReX and ImX accumulators reduced to type int. Input data size is of type char

# Benchmarks
The measured execution times in μs of the different FFTA variants running on the Arduino UNO are shown in the table below. The times mentioned under *dft1 - dft4* are for the full spectrum calculation (positive and negative frequencies).
The data set that was used for bench-marking is: x = 0, 1, 0, 0, ...  [3].

N-points: / Implementation: | dft1 | dft2 | dft3 | dft4 | dft5
--------------------------- | ---- | ---- | ---- | ---- | ----
8 | 16436 | 1996 | 968 | 720 | 304
16 | 66040 | 7552 | 3500 | 2568 | 992
32 | 265880 | 29636 | 13548 | 9812 | 3420
64 | 1073796 | 117644 | 53540 | 38600 | 12868

### Conclusion
The computing time of *dft1 - dft4* scales with N^2 as expected. The speed ratio of dft1:dft2:dft3:dft4 is about: 26:3:1.5:1.
The well known FFT algorithm scales with N*log2(N) [1]. An integer math implementation of the FFT on the Arduino might be faster than the method proposed here, but this needs to be confirmed. In the end, for many data points, FFT wins.

# Library
The FFTA library including an example for an audio spectrum analyzer.

- under construction


[1]: https://en.wikipedia.org/wiki/Fast_Fourier_transform

[2]: http://www.fftw.org/

[3]: http://www.sccon.ca/sccon/fft/fft3.htm

[4]: https://github.com/MartinStokroos/NativeDDS
