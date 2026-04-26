# LMS Adaptive Audio Filtering

Real-time audio source separation using a Least Mean Squares (LMS) 
adaptive FIR filter, implemented in both C++ and Python. Removes 
unwanted trumpet bleed from a vibraphone recording by learning the 
acoustic impulse response between the two signals.

## Overview

In multi-instrument recordings, sound bleed between microphones 
is a common problem. This project addresses a real-world case: 
trumpet sound bleeding into a vibraphone track intended for CD 
production. An LMS adaptive FIR filter learns the impulse response 
of the interference path and subtracts it from the mixed signal, 
recovering a clean vibraphone output.

The algorithm is implemented twice — first in Python as a reference, 
then reimplemented in C++ using the SoX audio library and a custom 
FIR filter class for improved computational efficiency.

## How It Works

1. Both audio files are read and normalised
2. First 4.9 seconds are skipped (dead time at file start)
3. FIR filter pre-trains for 2 seconds on the reference signal
4. LMS algorithm iteratively updates filter coefficients to 
   minimise error between filtered trumpet and mixed signal
5. Error signal (cleaned vibraphone) is written to output WAV

## Key Parameters

| Parameter | Value | Rationale |
|---|---|---|
| Sample rate | 96,000 Hz | CD-quality audio |
| FIR filter length | 0.3s (28,800 taps) | Captures full room impulse response |
| Learning rate | 0.002 | Balances convergence speed against stability |
| Skip duration | 4.9s | Avoids dead time at file start |
| Pre-training | 2s | Allows filter to begin converging before output |

## C++ vs Python Implementation

| Aspect | Python | C++ |
|---|---|---|
| Audio I/O | soundfile library | SoX library |
| FIR filter | fir1 module | Custom Fir1 class |
| Performance | Rapid prototyping | Improved computational efficiency |
| Output | lr=0.002;fl=0.3s.wav | output.wav |

Both implementations use identical LMS algorithm logic and 
produce comparable filtered outputs.

## Files

| File | Description |
|---|---|
| main.cpp | C++ implementation |
| srcsep.py | Python reference implementation |
| audioio.cxx | C++ audio I/O implementation |
| audioio.h | Audio I/O header |
| Fir1.h | FIR filter class header |
| sox.h | SoX library header |
| Trumpet.wav | Dry reference signal (trumpet) |
| Vibraphone.wav | Wet mixed signal (vibraphone + trumpet bleed) |
| output-filtered.wav | Cleaned vibraphone output |
| LMS-Adaptive-Filtering-Report.pdf | Full technical report |

## Build and Run (C++)

Requires SoX library installed on your system:

```bash
# Install SoX (macOS)
brew install sox

# Compile
g++ -g main.cpp audioio.cxx -o lab2 -lsox

# Run
./lab2
```

## Run (Python)

```bash
pip install soundfile numpy
python3 srcsep.py
```

## Results

The LMS adaptive filter successfully reduces trumpet bleed from 
the vibraphone recording. Power gain output indicates the 
proportion of interference removed relative to the original 
mixed signal energy.

## Technologies

C++, Python, SoX Audio Library, LMS Adaptive Filtering, 
FIR Filter Design, Digital Signal Processing

## Author

**Adeem Azad**  
BEng Mechatronics, University of Glasgow   
[LinkedIn](https://www.linkedin.com/in/adeem-azad)
