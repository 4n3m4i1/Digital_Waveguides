#ifndef WAVEBLOCK_IO_h
#define WAVEBLOCK_IO_h

#include "Waveblock_Base.h"

enum WAVEIO_PARAMS {
    RUN_AT_0        = 0,
    RUN_TILL_END    = UINT64_MAX
};

int FILE_Listener(FILE *fp, uint8_t clk, wg_data_t *port_2_sniff, uint64_t start_tick, uint64_t stop_tick){
    static uint64_t tick_ct = 0;
    if((tick_ct >= start_tick) && (tick_ct <= stop_tick) && clk){
        fprintf(fp, "%.14lf\n", *port_2_sniff);
        tick_ct++;
    }
}

#endif