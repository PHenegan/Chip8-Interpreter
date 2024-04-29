#include "chip8-timer.h"

void decrement_timer(
                     unsigned char *timer,
                     time_t *start,
                     char* play_sound) {
  time_t current = time(NULL);
  // decrement the timer if more than 1 second has passed if the timer is above 0
  if (*timer > 0 && current - *start > 0) {
    *timer -= 1;

    *start = time(NULL);

    if (play_sound != NULL) {
      *play_sound = 1;
    } 
  }
}

void instruction_sleep() {
    struct timespec sleep_val;
    sleep_val.tv_sec = 0;
    sleep_val.tv_nsec = (long)(1.0 / INSTRUCTION_FREQUENCY * 1000000000);
    nanosleep(&sleep_val, &sleep_val); 
}
