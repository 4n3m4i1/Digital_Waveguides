#ifndef WAVEBLOCK_OSC_h
#define WAVEBLOCK_OSC_h

#include "Waveblock_Base.h"
#include <math.h>

/*
    Table of Contents:
        - Sine
        - Periodic Pulse Train
        - Conditional Pulse Train / Step
        - PCM
*/

//////////////////////////////////////////////////////
//      Sine Oscillator
//////////////////////////////////////////////////////

struct Waveblock_Sine_OSC {
    double      Fs;
    double      omega;  // rad/s
    double      phi;    // phase shift
    uint64_t    ctt;    // Sample counter
};

int Init_Sine_OSC(struct Waveguide_Block *wgb, uint64_t Fs, uint64_t freq_hz, double phase_deg){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_Sine_OSC *pt;
        pt = (struct Waveblock_Sine_OSC *)malloc(sizeof(struct Waveblock_Sine_OSC));
        pt->ctt = 0;
        pt->Fs = (double)Fs;
        pt->omega = 2.0 * M_PI * freq_hz;
        pt->phi = M_PI * phase_deg / 180.0;

        wgb->block_function_data    = (void *)pt;
        wgb->block_function         = nullptr;
        wgb->type                   = B_SIG_SRC;
        
        retval = EXIT_SUCCESS;
    }
    return retval;
}

void WGB_Sine_OSC(struct Waveguide_Block *wgb, uint8_t wgb_clock){
    if(!wgb_clock) {
        struct Waveblock_Sine_OSC *pt;
        pt = (struct Waveblock_Sine_OSC *)wgb->block_function_data;
        wg_data_t val = sin((pt->omega * ((double)pt->ctt / pt->Fs)) + pt->phi);
        pt->ctt++;
        Write_Wire(wgb->d_out_0, val);
    }
}

int Destroy_Sine_OSC(struct Waveguide_Block *wgb){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_Sine_OSC *pt;
        pt = (struct Waveblock_Sine_OSC *)wgb->block_function_data;
        free(pt);
        retval = EXIT_SUCCESS;
    }
    return retval;
}

//////////////////////////////////////////////////////
//      Pulse Train
//////////////////////////////////////////////////////

enum WGB_PULSE_PARAMS {
    NO_PULSES,
    SINGLE_PULSE,
    INFINITE_PULSES = UINT64_MAX
};

struct Waveblock_Pulse_OSC {
    uint64_t    num_pulses;
    uint64_t    pulse_period;   // in clks
    uint64_t    pulse_width;    // in clks
};


#endif