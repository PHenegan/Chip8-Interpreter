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

void decrement_timer(unsigned char *timer, time_t *start, short play_sound) {
  time_t current = time(NULL);
  // decrement the timer if more than 1 second has passed if the timer is above 0
  if (*timer > 0 && current - *start > 0) {
    *timer -= 1;

    *start = time(NULL);

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

void exec_arithmetic(struct Chip8 *const chip8, short instr) {
  unsigned char x = (unsigned char)((instr & OP_X) >> 2);
  unsigned char y = (unsigned char)((instr & OP_Y) >> 1);
  short result;
  switch (instr & OP_N) {
    // Set operator
    case 0:
      chip8->V[x] = chip8->V[y];
      break;
    // Binary OR
    case 1:
      chip8->V[x] = chip8->V[x] | chip8->V[y];
      break;
    // Binary AND
    case 2:
      chip8->V[x] = chip8->V[x] & chip8->V[y];
      break;
    // Logical XOR
    case 3:
      chip8->V[x] = chip8->V[x] ^ chip8->V[y];
      break;
    // ADD
    case 4:
      result = chip8->V[x] + chip8->V[y];
      chip8->V[x] = (unsigned char)result;
      chip8->V[0xF] = result > 255;
      break;
    // SUB (VX - VY)
    case 5:
      result = chip8->V[x] - chip8->V[y];
      chip8->V[x] = (unsigned char)result;
      chip8->V[0xF] = result > chip8->V[x];
      break;
    // SRA
    case 6:
      // shift VX right by 1, storing the shifted bit into VF
      chip8->V[0xF] = chip8->V[x] & 1; // isolate the last bit
      chip8->V[x] = chip8->V[x] >> 1;
      break;
    // SUB (VY - VX)
    case 7:
      chip8->V[x] = chip8->V[y] - chip8->V[x];
      chip8->V[0xF] = chip8->V[x] > chip8->V[y];
      break;
    // SLA
    case 0xE:
      // shift VX left by 1, storing the shifted bit into VF
      chip8->V[0xF] = chip8->V[x] & 0x8000; // isolate the first bit
      chip8->V[x] = chip8->V[x] << 1; 
      break;
  }
}

void exec_instruction(struct Chip8 *const chip8, short instruction) {
  unsigned short nnn = instruction & OP_NNN;
  unsigned char nn = instruction & OP_NN;
  unsigned char n = instruction & OP_N;
  unsigned char x = (instruction & OP_X) >> 2;
  unsigned char y = (instruction & OP_Y) >> 1;

  switch (instruction >> 3) {
    case OP_SYS:
      if (instruction == OP_CLR_SCRN) {
        // clear the screen
      }
      else if (instruction == OP_RET) {
        // call return
      }
      break;
    case OP_JUMP:
      chip8->pc = nnn; 
      break;
    case OP_CALL:
      chip8->sp++;
      chip8->stack[chip8->sp] = chip8->pc;
      chip8->pc = nnn;
      break;
    case OP_BEQI:
      // skip 1 instruction if VX == NN
      if (chip8->V[x] == nn) {
        chip8->pc += 2;
      }
      break;
    case OP_BNEI:
      if (chip8->V[x] != nn) {
        chip8->pc += 2;
      }
      break;
    case OP_BEQ:
      if (chip8->V[x] == chip8->V[y]) {
        chip8->pc += 2;
      }
      break;
    case OP_BNE:
      if (chip8->V[x] != chip8->V[y]) {
        chip8->pc += 2;
      }
      break;
    case OP_MATH:
      exec_arithmetic(chip8, instruction);
      break;
  }
}

int exec_cycle(struct Chip8 *const chip8) {
  // NOTE - this isn't complete
  time_t start = time(NULL);

  while (1) {
    decrement_timer(&chip8->delay_timer, &start, 0);
    decrement_timer(&chip8->sound_timer, &start, 1);
    
    short instruction = fetch_instruction(chip8);

    exec_instruction(chip8, instruction);
    
    // TODO - get keypress here
    break;
  }

  return -1;
}
