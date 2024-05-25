#ifndef CHIP8_TIMER
#define CHIP8_TIMER

#include <time.h>
// number of instructions per second
#define INSTRUCTION_FREQUENCY 700

// NOTE - this file and implementation is kind of deprecated at the moment
// I am not using it, but *may* try to move some of the SDL timer logic into this
// area, since the other timer code is platform dependent, so it may be beneficial
// to make all of the code modular/in its own file

// Decrement a given given timer for the chip8 system
void decrement_timer(
  unsigned char *timer,
  time_t *start,
  char* play_sound 
);

void instruction_sleep();

#endif
