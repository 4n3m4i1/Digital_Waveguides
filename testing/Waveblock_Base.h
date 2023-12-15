#ifndef WAVEBLOCKS_BASE_h
#define WAVEBLOCKS_BASE_h

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

typedef double  wg_data_t;
typedef int     Block_t;

#ifndef nullptr
#define nullptr 0
#endif

#define DFL_CLK_HZ  48000000

struct Waveguide_Wire {
    wg_data_t   wire;

    struct Waveguide_Wire   *next;  // vv
};


struct Waveguide_Block {
    char        *ascii_name;
    uint32_t    type;
    uint32_t    uid;
    uint8_t     synchronous;

    wg_data_t   (*block_function)(struct Waveguide_Block *); // Actual jump table function ptr
    void        *block_function_data;   // Unique per block function, could be buffers or whatever
    wg_data_t   *d_in_0;
    wg_data_t   *d_in_1;

    wg_data_t   *d_out_0;
    wg_data_t   *d_out_1;

    struct Waveguide_Block  *next;  // Linked List for initial setup and dyn alloc
};


struct Waveguide_Controller {
    uint32_t    num_blocks;
    uint32_t    num_wires;

    uint64_t    clk_hz;
    double      runtime;
    uint64_t    samples_2_run;

    struct Waveguide_Wire   *linked_wires;
    struct Waveguide_Block  *linked_blocks;

    struct Waveguide_Block  **blocks;   // array of pointers, crazy concept
};


enum EP_CONN {
    CONN_NOTHING,
    CONN_IN_0,
    CONN_IN_1,
    CONN_OUT_0,
    CONN_OUT_1,
    CONN_NO_CHG
};

enum BLOCK_TYPES {
    RESERVED,
    GND_NODE,
    B_MAC,
    B_ADD,
    B_MUL,
    B_SUB,
    B_DIV,
    B_DELAY,
    B_MOD,
    B_LPF,
    B_HPF,
    B_BPF,
    B_OUTPUT,
    B_SIG_SRC,
    B_SINGLE_VAR
};

enum TIMINGREQS {
    FULLY_ASYNC,        // Push to output when done
    FULLY_SYNCHRONOOUS  // Push on clk lo
};

uint8_t Waveblock_Clock(){
    static uint8_t ck = 0;
    return ck = ~ck;
}

wg_data_t Read_Wire(wg_data_t *a){
    return *a;
}

void Write_Wire(wg_data_t *a, wg_data_t dat){
    //printf("%8X -> %f pe= %f\n", a, dat, *a);
    *a = dat;
    //printf("Written\n");
}


int Init_Controller(struct Waveguide_Controller *wgc, uint64_t clk_hz, double runtime){
    int retval = EXIT_FAILURE;
    if(wgc){
        wgc->blocks         = nullptr;
        wgc->linked_blocks  = nullptr;
        wgc->linked_wires   = nullptr;
        wgc->num_blocks     = 0;
        wgc->num_wires      = 0;
        wgc->clk_hz         = clk_hz;
        wgc->runtime        = runtime;
        wgc->samples_2_run  = (uint64_t)((double)clk_hz * runtime);

        retval = EXIT_SUCCESS;
    }
    return retval;
}

// Add a block to the system
int Add_Block(struct Waveguide_Controller *wgc, char *new_block_name){
    int retval = EXIT_FAILURE;
    if(!wgc || !new_block_name) retval = EXIT_FAILURE;
    else {
        struct Waveguide_Block *pt;
        pt = (struct Waveguide_Block *)malloc(sizeof(struct Waveguide_Block));
        if(pt){
            if(wgc->linked_blocks){
                struct Waveguide_Block *ptt;
                ptt = wgc->linked_blocks;
                while(ptt->next) ptt = ptt->next;
                
                ptt->next = pt;
                //for(uint32_t n = 0; n < wgc->num_blocks; ++n){
                //    if(ptt->next) ptt = ptt->next;
                //    else {
                //        ptt->next = pt;
                //        break;
                //    }    
                //}
            } else {
                wgc->linked_blocks = pt;
            }
        }
        

        pt->ascii_name = new_block_name;
        pt->d_in_0 = nullptr;
        pt->d_in_1 = nullptr;
        pt->d_out_0 = nullptr;
        pt->d_out_1 = nullptr;
        pt->block_function = nullptr;
        pt->block_function_data = nullptr;
        pt->synchronous = FULLY_SYNCHRONOOUS;

        pt->uid = wgc->num_blocks++;
        retval = pt->uid;
    }
    return retval;
}

int Destroy_Blocks(struct Waveguide_Controller *wgc){
    int retval = EXIT_FAILURE;
    if(wgc){
        struct Waveguide_Wire *ptw;
        struct Waveguide_Wire *ptw_prev;
        
        for(uint32_t n = 0; n < wgc->num_wires - 1; ++n){
            ptw = wgc->linked_wires;
            while(ptw->next){
                ptw_prev = ptw;
                ptw = ptw->next;
            }
            ptw_prev->next = nullptr;
            free(ptw);
        }
        free(wgc->linked_wires);
        
        for(uint32_t n = 0; n < wgc->num_blocks; ++n){
            free(wgc->blocks[n]);
        }
        free(wgc->blocks);
        
    }
    return retval;
}

// Connect endpoints within a block together
int Wire_Blocks(struct Waveguide_Controller *wgc, wg_data_t **ep0_0, wg_data_t **ep0_1){
    int retval;
    if(!wgc) retval = EXIT_FAILURE;
    else {
        struct Waveguide_Wire *pt;
        pt = (struct Waveguide_Wire *)malloc(sizeof(struct Waveguide_Wire));
        if(pt){
            if(wgc->linked_wires){
                struct Waveguide_Wire *ptt;
                ptt = wgc->linked_wires;
                while(ptt->next) ptt = ptt->next;
                ptt->next = pt;
            } else {
                wgc->linked_wires = pt;
            }
            
            pt->next = nullptr;
            pt->wire = 0;           // Doesn't really matter I guess..
            *ep0_0 = &pt->wire;  // Connect wire ends, bidirectional as a real wire is
            *ep0_1 = &pt->wire;
            //printf("Wire %u @ %8X\n", wgc->num_wires, &pt->wire);
            wgc->num_wires++;
            retval = EXIT_SUCCESS;
        } else {
            retval = EXIT_FAILURE;
        }
    }
    return retval;
}

struct Waveguide_Block **Pre_Run_Cleanup(struct Waveguide_Controller *wgc){
    struct Waveguide_Block **retval;
    if(wgc){
        wgc->blocks = (struct Waveguide_Block **)malloc(wgc->num_blocks * sizeof(struct Waveguide_Block *));
        if(wgc->blocks){
            struct Waveguide_Block *pt;
            pt = wgc->linked_blocks;
            for(uint32_t n = 0; n < wgc->num_blocks; ++n){
                wgc->blocks[n] = pt;
                pt = pt->next;
            }
            retval = wgc->blocks;
        } else {
            retval = nullptr;
        }
    }
    return retval;
}

#endif