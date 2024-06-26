#ifndef CHIP8_TIMER
#define CHIP8_TIMER

#include "chip8.h"
#include <time.h>
// number of instructions per second

// NOTE - this file and implementation is kind of deprecated at the moment
// I am not using it, but *may* try to move some of the SDL timer logic into this
// area, since the other timer code is platform dependent, so it may be beneficial
// to make all of the code modular/in its own file

// Decrement a given given timer for the chip8 system

// Decrement the timers using the given timestamps to determine if sufficient timer time has passed,
//
// `chip8` - the CHIP-8 system to update the timers for
// `last_update` - the timestamp of the previous timer update
// `current` - the current timestamp
//
// return 1 if the timer has been updated, 0 otherwise

// Sleep for the given amount of microseconds
void precise_sleep(long us);

#endif
