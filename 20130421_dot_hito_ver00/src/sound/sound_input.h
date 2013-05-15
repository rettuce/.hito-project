//
//  sound_input.h
//  SoundInput
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#ifndef __SoundInput__sound_input__
#define __SoundInput__sound_input__

#include <iostream>

#ifndef M_PI
#define M_PI  3.14159265358979323846  /* pi */
#endif

class fft {

public:

    fft();
    ~fft();

    /* Calculate the power spectrum */
    void powerSpectrum(int start, int half, float *data, int windowSize,float *magnitude,float *phase, float *power, float *avg_power);
    /* ... the inverse */
    void inversePowerSpectrum(int start, int half, int windowSize, float *finalOut,float *magnitude,float *phase);

};


#endif /* defined(__SoundInput__sound_input__) */
