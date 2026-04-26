import numpy as np
import soundfile as sf
import fir1

np.seterr(all='raise')

# Read the file to clean
with sf.SoundFile('Vibraphone.wav', 'r') as wf:
        wet = wf.read()
        print(f'Wet file. Samples: {wet.shape[0]}, max/min normalised samples: {max(wet)}, {min(wet)}')

# Read the dry version of the sound to be removed
with sf.SoundFile('Trumpet.wav', 'r') as df:
        dry = df.read()
        print(f'Dry file. Samples: {dry.shape[0]}, max/min normalised samples: {max(dry)}, {min(dry)}')

sr = 96000

# FIR Length in seconds
fl = 0.3

# Skip for the learning phase to igonre dead time at start of file
ss  = 4.9

# Set the filter learning rate
lrate = 0.002

nc = int(fl * sr)
fir = fir1.Fir1(nc)
fir.setLearningRate(lrate)
ns = int(sr*ss)
training = dry[ns:]
src = wet[ns:]
msi = np.mean(np.square(src))
fnum= 1

if len(training) != len(src) :
        raise 'interference and source samples are different lengths'

# Run a couple of seconds through the filter so it's started to learn.
trial = 2*sr

print('Pre-training...')
for i in range(trial if trial < len(src) else len(src)) :
        fir.lms_update(src[i]-fir.filter(training[i]))

print('Processing')
error = np.zeros(len(training))
for i in range(len(training)) : 
        error[i] = src[i] - fir.filter(training[i])
        fir.lms_update(error[i])

print(f'Power gain: {np.mean(np.square(error))/msi}')
sf.write(f'lr={lrate};fl={fl}s.wav', error, sr, 'PCM_24')
