#ifndef WAVEBLOCK_WAV_OUTPUT_h
#define WAVEBLOCK_WAV_OUTPUT_h

#include "Waveblock_Base.h"
#include "ez_wav.h"

enum WGB_WAVE_PRINTSTAT {
    WGB_WAVE_NO_OUTPUT,
    WGB_WAVE_OUTPUT
};

struct Waveblock_WAV_Output {
    uint32_t    max_samples;
    uint32_t    curr_ptr;
    int16_t     *data;
};

int Init_WAV_Output(struct Waveguide_Block *wgb, double runtime, uint64_t fs){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_WAV_Output *ptt;
        ptt = (struct Waveblock_WAV_Output *)malloc(sizeof(struct Waveblock_WAV_Output));
        
        if(ptt){
            ptt->max_samples = (uint32_t)((runtime * (double)fs));
            ptt->curr_ptr = 0;
            ptt->data = (int16_t *)malloc(ptt->max_samples * sizeof(int16_t));

            if(ptt->data){
                wgb->block_function_data = (void *)ptt;
                retval = EXIT_SUCCESS;
            }
        }
    }
    return retval;
}

void WAV_Listener(struct Waveguide_Block *wgb, uint8_t wgb_clock){
    struct Waveblock_WAV_Output *pt;
    pt = (struct Waveblock_WAV_Output *)wgb->block_function_data;

    if(wgb_clock){
        pt->data[pt->curr_ptr++] = (int16_t)(((double)INT16_MAX / 4) * Read_Wire(wgb->d_in_0));
    }
}

int WAV_Create_File(struct Waveguide_Block *wgb, char *filename, uint32_t fs, uint8_t printstats){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_WAV_Output *pt;
        pt = (struct Waveblock_WAV_Output *)wgb->block_function_data;
        
        struct WAVE_Header whdr;
        retval = output_wav(&whdr, filename, pt->data, 16, pt->max_samples, fs, 1);

        if(!retval && printstats) print_wave_header(&whdr);
    }
    return retval;
}

int Destroy_WAV_Output(struct Waveguide_Block *wgb){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_WAV_Output *pt;
        pt = (struct Waveblock_WAV_Output *)wgb->block_function_data;

        free(pt->data);
        free(pt);
        retval = EXIT_SUCCESS;
    }
    return retval;
}

#endif