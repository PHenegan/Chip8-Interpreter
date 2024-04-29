#ifndef CHIP8_TIMER
#define CHIP8_TIMER

#include <time.h>
// number of instructions per second
#define INSTRUCTION_FREQUENCY 700

// Decrement a given given timer for the chip8 system
void decrement_timer(
  unsigned char *timer,
  time_t *start,
  char* play_sound 
);

void instruction_sleep();

#endif
