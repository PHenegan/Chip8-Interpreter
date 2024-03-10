#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>

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

short fetch_instruction(struct Chip8 *chip8) {
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

int exec_cycle(struct Chip8 *chip8) {
  // NOTE (obvious) - this isn't complete
  while (1) {
    fetch_instruction(chip8);
    break;
  }

  return -1;
}
