# Digital_Waveguides
A simple toolbox providing DSP building blocks, wires to connect them, direct .wav output, and simulation setups.  
Should be easier than python/matlab. All libraries are header only because I'm lazy.  
Each new module type should be placed in its own header file.  
By default the processing datatype is double, cast to whatever you want for output at the end.
  
## Simulation Theory
Every block connects via a network of "wires" to other blocks. As in reality and HDL a wire should  
only have a single driver, though an error will not be thrown if you violate this.  
Each "tick" of the simulation represents some rising edge event by which each block will read  
its inputs, and perform some task. Synchronous (default) modules will wait until the falling edge  
of the clock to update its output, async (can be configured per block) will push to output as soon as a task  
completes. The simulation loop runs for 2*tick_max in order to allow falling edges, which may be  
useful for future DDR blocks.  
  
### Block and Wire Makeup
- Wires
	- One driver
	- Infinite fanout

- Blocks
	- 2 inputs
	- 2 outputs
	- I/O are single ended, though certain modules can be made to be differential

- Controller
	- Manages datastructures

  
## Short Term Feature Roadmap:
- Cleaner simulation (maybe automatic loop fill w funct pointers idk yet)
- Syntax cleanup for addressing
- Better docs
- FM, AM, PCM, step, tri virtual signal sources
- 2 port impedance mismatch network
- True MAC
- Secondary and Tertiary block levels built from primary level
  
## Available Blocks
- Signal Sources
	- Sine(wt + phi)
- Maths
	- Add (Y = A + B)
	- Sub (Y = A - B)
	- Mul (Y = A * B)
	- Div (Y = A / B)
- DSP-y Things
	- Delay Line (of arbitrary N length)
- Output
	- 16-bit WAVE (via easyWAVE.h)
	- .txt, .csv, or other standard ascii files
