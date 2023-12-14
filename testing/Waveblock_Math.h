#ifndef WAVEBLOCK_MATH_h
#define WAVEBLOCK_MATH_h

#include "Waveblock_Base.h"

struct Waveblock_Simple_Math_Block {
    wg_data_t y;
};

wg_data_t WGB_ADD(struct Waveguide_Block *wgb){
    return Read_Wire(wgb->d_in_0) + Read_Wire(wgb->d_in_1);
}

wg_data_t WGB_SUB(struct Waveguide_Block *wgb){
    return Read_Wire(wgb->d_in_0) - Read_Wire(wgb->d_in_1);
}

wg_data_t WGB_MUL(struct Waveguide_Block *wgb){
    return Read_Wire(wgb->d_in_0) * Read_Wire(wgb->d_in_1);
}

wg_data_t WGB_DIV(struct Waveguide_Block *wgb){
    return Read_Wire(wgb->d_in_0) / Read_Wire(wgb->d_in_1);
}

int Init_Simple_Math_Block(struct Waveguide_Block *wgb, uint32_t btype){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_Simple_Math_Block *ptt;
        ptt = (struct Waveblock_Simple_Math_Block *)
                    malloc(sizeof(struct Waveblock_Simple_Math_Block));
        
        ptt->y                      = 0;
        wgb->block_function_data    = (void *)ptt;
        wgb->type                   = btype;
        switch(btype){
            case B_ADD: 
                wgb->block_function = WGB_ADD;
                retval              = EXIT_SUCCESS;
            break;
            case B_MUL: 
                wgb->block_function = WGB_SUB;
                retval              = EXIT_SUCCESS;
            break;
            case B_SUB: 
                wgb->block_function = WGB_MUL;
                retval              = EXIT_SUCCESS;
            break;
            case B_DIV: 
                wgb->block_function = WGB_DIV;
                retval              = EXIT_SUCCESS;
            break;
            default:
                retval = EXIT_FAILURE;
            break;
        }
    }
    return retval;
}

void Simple_ALU(struct Waveguide_Block *wgb, uint8_t wgb_clock){
    struct Waveblock_Simple_Math_Block *pt;
    pt = (struct Waveblock_Simple_Math_Block *)wgb->block_function_data;

    if(wgb_clock){
        pt->y = (*wgb->block_function)(wgb);
        if(wgb->synchronous == FULLY_ASYNC) Write_Wire(wgb->d_out_0, pt->y);
    } else {
        if(wgb->synchronous == FULLY_SYNCHRONOOUS) Write_Wire(wgb->d_out_0, pt->y);
    }
}

int Destroy_Simple_Math_Block(struct Waveguide_Block *wgb){
    int retval = EXIT_FAILURE;
    if(wgb){
        struct Waveblock_Simple_Math_Block *pt;
        pt = (struct Waveblock_Simple_Math_Block *)wgb->block_function_data;
        free(pt);

        retval = EXIT_SUCCESS;
    }
    return retval;
}

#endif