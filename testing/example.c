#include <stdio.h>
#include "Waveblocks.h"



int main(int argc, char **argv){
    struct Waveguide_Controller Control;
    // Initiate Control Struct, 44.1ksps, 5 seconds runtime
    Init_Controller(&Control, 44100, 5);
    printf("Controller Initiated.\n");

    // Define some functional blocks
    Block_t delay_0     = Add_Block(&Control, "Delay Line 0");
    Block_t WAVE_out    = Add_Block(&Control, "WAVE File Output");
    Block_t SIN_OSC     = Add_Block(&Control, "Sine Source");
    Block_t SIN_OSC2    = Add_Block(&Control, "Sine Source 2");
    Block_t Adder       = Add_Block(&Control, "Terminal Summer");
    Block_t Adder2      = Add_Block(&Control, "Signal Summer");
    Block_t MUL_0       = Add_Block(&Control, "Mod Amp");
    Block_t MUL_1       = Add_Block(&Control, "AM Apply");
    printf("Blocks Created Successfully!\n");

    printf("Cleaning Up...\n");
    struct Waveguide_Block **block = Pre_Run_Cleanup(&Control);
    printf("Squeaky Clean!\n");

    // Create the delay line on the delay_0 block, 50 samples deep
    Init_Delay_Line(block[delay_0], 50);
    // Create WAVE output module
    Init_WAV_Output(block[WAVE_out], Control.runtime, Control.clk_hz);
    // Create Sine wave source, Fs = clkHz, freq = 500hz, 0 deg phase shift
    Init_Sine_OSC(block[SIN_OSC], Control.clk_hz, 500, 0);
    Init_Sine_OSC(block[SIN_OSC2], Control.clk_hz, 20, 0);
    // Initiate an adder
    Init_Simple_Math_Block(block[Adder], B_ADD);            // Can do either
    Init_Simple_Math_Block(Control.blocks[Adder2], B_ADD);  //  block addressing scheme!
    Init_Simple_Math_Block(Control.blocks[MUL_0], B_MUL);  //  block addressing scheme!
    Init_Simple_Math_Block(Control.blocks[MUL_1], B_MUL);  //  block addressing scheme!
    printf("All Blocks Initiated.\n");


    // Wire Modules Together
    // Bi-directional wires, shown are short and long addressing schemes
    Wire_Blocks(&Control, &block[SIN_OSC]->d_out_0,    &block[delay_0]->d_in_0);
    Wire_Blocks(&Control, &Control.blocks[SIN_OSC]->d_out_0,    &Control.blocks[Adder2]->d_in_0);
    Wire_Blocks(&Control, &Control.blocks[SIN_OSC2]->d_out_0,   &Control.blocks[Adder2]->d_in_1);
    Wire_Blocks(&Control, &Control.blocks[Adder2]->d_out_0,     &Control.blocks[Adder]->d_in_0);
    Wire_Blocks(&Control, &Control.blocks[delay_0]->d_out_0,    &Control.blocks[Adder]->d_in_1);
    Wire_Blocks(&Control, &Control.blocks[Adder]->d_out_0,      &Control.blocks[WAVE_out]->d_in_0);
    printf("%u Wires Placed.\n", Control.num_wires);

    FILE *fp = fopen("Output.txt", "w");
    printf("File Opened!\n");

    uint8_t clk = 0;

    printf("Running Sim for %u ticks\n", Control.samples_2_run);
    for(uint64_t n = 0; n < Control.samples_2_run * 2; ++n){
        //printf("CLK %c @ tick %u\n", (clk) ? '1' : '0', n);
        WGB_Sine_OSC(block[SIN_OSC], clk);                          // Short address
        WGB_Sine_OSC(Control.blocks[SIN_OSC2], clk);                //  Long address
        Delay_Line(Control.blocks[delay_0], clk);                   //
        Simple_ALU(Control.blocks[Adder], clk);                     //
        Simple_ALU(Control.blocks[Adder2], clk);                    // Add on rising, out falling
        WAV_Listener(Control.blocks[WAVE_out], clk);                // Sneaks a peak every RISING
        
        FILE_Listener(fp, clk, block[WAVE_out]->d_in_0, RUN_AT_0, RUN_TILL_END);

        clk = Waveblock_Clock();
    }
    printf("Sim Done!\n");

    // Create output file :)
    WAV_Create_File(Control.blocks[WAVE_out], "CoolWAVE.wav", Control.clk_hz, 1);
    fclose(fp);
    printf("Files Closed!\n");

    // Destroy the functional block components
    Destroy_Delay_Line(Control.blocks[delay_0]);
    Destroy_WAV_Output(Control.blocks[WAVE_out]);
    Destroy_Sine_OSC(Control.blocks[SIN_OSC]);
    Destroy_Sine_OSC(Control.blocks[SIN_OSC2]);
    Destroy_Simple_Math_Block(Control.blocks[Adder]);
    Destroy_Simple_Math_Block(Control.blocks[Adder2]);
    printf("Destroyed Blocks.\n");

    // Destroy Entire block structure
    Destroy_Blocks(&Control);
    printf("Destroyed Wires.\n");
    printf("Destroyed Controller.\n");

    return EXIT_SUCCESS;
}