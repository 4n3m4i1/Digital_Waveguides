#ifndef WAVEGUIDE_DELAYLINE_h
#define WAVEGUIDE_DELAYLINE_h

#include "Waveblock_Base.h"

struct Waveblock_Delay_Line {
    uint32_t    stages;
    wg_data_t   *delay_line;
};

int Init_Delay_Line(struct Waveguide_Block *wgb, uint32_t delay_stages){
    int retval;
    if(!wgb || !delay_stages) retval = EXIT_FAILURE;
    else {
        struct Waveblock_Delay_Line *ptt;
        ptt = (struct Waveblock_Delay_Line *)malloc(sizeof(struct Waveblock_Delay_Line));
        wg_data_t *pt;
        pt = (wg_data_t *)calloc(delay_stages, sizeof(wg_data_t));
        ptt->stages = delay_stages;
        ptt->delay_line = pt;
        wgb->block_function_data    = (void *)ptt;
        wgb->block_function         = nullptr;
        wgb->type                   = B_DELAY;
        retval = EXIT_SUCCESS;
    }
    return retval;
}



void Delay_Line(struct Waveguide_Block *wgb, uint8_t wgb_clock){
    struct Waveblock_Delay_Line *pt;
    pt = (struct Waveblock_Delay_Line *)wgb->block_function_data;

    uint32_t arrmax = pt->stages - 1;

    if(wgb_clock){
        // Make this circular buffers ffs...
        for(uint32_t n = arrmax; n != 0; --n){
            pt->delay_line[n] = pt->delay_line[n - 1];
        }
        pt->delay_line[0] = Read_Wire(wgb->d_in_0);
        //Write_Wire(wgb->d_out_0, pt->delay_line[arrmax]);
    } else {
        // Output data on low clock
        Write_Wire(wgb->d_out_0, pt->delay_line[arrmax]);
    }
}

int Destroy_Delay_Line(struct Waveguide_Block *wgb){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_Delay_Line *ptt;
        ptt = (struct Waveblock_Delay_Line *)wgb->block_function_data;
        free(ptt->delay_line);
        free(ptt);
        retval = EXIT_SUCCESS;
    }
    return retval;
}


#endif