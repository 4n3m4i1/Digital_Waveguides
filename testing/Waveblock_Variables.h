#ifndef WAVEBLOCK_VARS_h
#define WAVEBLOCK_VARS_h

#include "Waveblock_Base.h"

struct Waveblock_Variable {
    wg_data_t   data;
};

enum WAVEGUIDE_VARVAL {
    WGB_DFL_VAR
};

int Init_WGB_Var(struct Waveguide_Block *wgb, wg_data_t initial_value){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_Variable *pt;
        pt = (struct Waveblock_Variable *)malloc(sizeof(struct Waveblock_Variable));
        pt->data = initial_value;
        wgb->block_function_data = (void *)pt;
        wgb->block_function = nullptr;
        wgb->type = B_SINGLE_VAR;
        wgb->d_out_0 = &pt->data;
        wgb->d_out_1 = &pt->data;
        retval = EXIT_SUCCESS;
    }
    return retval;
}

void Update_WGB_Var(struct Waveguide_Block *wgb, wg_data_t value){
    if(wgb){
        Write_Wire(wgb->d_out_0, value);
    }
}

int Destroy_WGB_Var(struct Waveguide_Block *wgb){
    int retval = EXIT_FAILURE;
    if(wgb){
        free(wgb->block_function_data);
        retval = EXIT_SUCCESS;
    }
    return retval;
}

#endif