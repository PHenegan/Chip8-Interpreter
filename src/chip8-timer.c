#include "chip8-timer.h"

void precise_sleep(long ns) {
    // using nanosleep because it can achieve a much better precision than SDL's delay function
    // (SDL2 has ~10ms precision, while nanosleep has around us precision),
    // needs about ~1.3ms precision
    struct timespec sleep_val;
    sleep_val.tv_sec = 0;
    sleep_val.tv_nsec = ns; 
    nanosleep(&sleep_val, &sleep_val); 
}
