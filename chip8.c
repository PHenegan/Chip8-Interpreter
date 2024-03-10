#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
  struct Chip8 *chip8 = calloc(1, sizeof(struct Chip8));
  // ensure a file was passed in
  if (argc < 2) {
    fprintf(stderr, "Unable to load program - no file passed in");
    exit(-1);
  }

  // try to load the file in
  if (load_program(chip8, argv[1]) == -1) {
    fprintf(stderr, "Unable to load program - error loading file");
    exit(-1);
  }
  load_font(chip8);
  
  free(chip8);
  return 0;
}

void decrement_timer(char *timer, time_t *start, short play_sound) {
  time_t current = time(NULL);
  // decrement the timer if more than 1 second has passed if the timer is above 0
  if (*timer > 0 && current - *start > 0) {
    *timer -= 1;

    if (play_sound) {
      // TODO - play a sound if the flag is set
    }
  }
}

short fetch_instruction(struct Chip8 *const chip8) {
  if (chip8->pc + 1 >= ADDRESS_COUNT) {
    return -1;
  }
  // update the opcode, which is the first half of the instruction and the next address in memory
  chip8->opcode = chip8->memory[chip8->pc];

  // combine the next two addresses in memory into the full instruction
  // using bitshifting and a bitwise or
  short result = chip8->opcode << 8 | chip8->memory[chip8->pc + 1];
   
  chip8->pc += 2;
  return 0;
}

void exec_instruction(struct Chip8 *const chip8, short instruction) {
    switch (chip8->opcode & OP_MASK) {
  }
}

int exec_cycle(struct Chip8 *const chip8) {
  // NOTE - this isn't complete
  time_t start = time(NULL);

  while (1) {
    // TODO - decrement timers here
    
    short instruction = fetch_instruction(chip8);

    exec_instruction(chip8, instruction);
    
    // TODO - get keypress here
    break;
  }

  return -1;
}
