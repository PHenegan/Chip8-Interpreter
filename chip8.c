#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
  // using calloc to make sure everything is 0-initialized
  struct Chip8 *chip8 = calloc(1, sizeof(struct Chip8));
  srand(time(NULL));
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

unsigned short fetch_instruction(struct Chip8 *const chip8) {
  if (chip8->pc + 1 >= ADDRESS_COUNT) {
    return -1;
  }
  // update the opcode, which is the first 4 bits of the instruction and the next address in memory
  chip8->opcode = chip8->memory[chip8->pc] >> 1;

  // combine the next two addresses in memory into the full instruction
  // using bitshifting and a bitwise or
  unsigned short result = chip8->opcode << 8 | chip8->memory[chip8->pc + 1];
   
  chip8->pc += 2;
  return result;
}

void exec_alu(struct Chip8 *const chip8, unsigned char x, unsigned char y, unsigned char n) {
  short result;
  switch (n) {
    case ALU_SET:
      chip8->V[x] = chip8->V[y];
      break;
    case ALU_OR:
      chip8->V[x] = chip8->V[x] | chip8->V[y];
      break;
    case ALU_AND:
      chip8->V[x] = chip8->V[x] & chip8->V[y];
      break;
    case ALU_XOR:
      chip8->V[x] = chip8->V[x] ^ chip8->V[y];
      break;
    // ADD
    case ALU_ADD:
      result = chip8->V[x] + chip8->V[y];
      chip8->V[x] = (unsigned char)result;
      chip8->V[0xF] = result > 255;
      break;
    // SUB (VX - VY)
    case ALU_SUBY:
      result = chip8->V[x] - chip8->V[y];
      chip8->V[x] = (unsigned char)result;
      chip8->V[0xF] = result > chip8->V[x];
      break;
    case ALU_SRL:
      // TODO - make configurable with a flag for SUPER-CHIP and CHIP-48 programs
      // shift VX right by 1, storing the shifted bit into VF
      chip8->V[0xF] = chip8->V[x] & 1; // isolate the last bit
      chip8->V[x] = chip8->V[x] >> 1;
      break;
    // SUB (VY - VX)
    case ALU_SUBX:
      chip8->V[x] = chip8->V[y] - chip8->V[x];
      chip8->V[0xF] = chip8->V[x] > chip8->V[y];
      break;
    case ALU_SLL:
      // TODO - make configurable with a flag for SUPER-CHIP and CHIP-48 programs
      // shift VX left by 1, storing the shifted bit into VF
      chip8->V[0xF] = chip8->V[x] & 0x8000; // isolate the first bit
      chip8->V[x] = chip8->V[x] << 1; 
      break;
  }
}

void exec_display(struct Chip8 *const chip8, unsigned char x, unsigned char y, unsigned char n) {

}

void exec_io(struct Chip8 *const chip8, unsigned char x, unsigned char nn) {
  char result;
  switch (nn) {
    case IO_LDTIME:
      chip8->V[x] = chip8->delay_timer;
      break;
    case IO_SDTIME:
      chip8->delay_timer = chip8->V[x];
      break;
    case IO_SSTIME:
      chip8->sound_timer = chip8->V[x];
      break;
    case IO_ADD_IDX:
      chip8->I += chip8->V[x];
      // I should only take up 12 bits, anything else is treated as an overflow
      chip8->V[0xF] = chip8->I >= 0x1000;
      chip8->I = chip8->I & 0x0FFF;
      break;
    case IO_GET_KEY:
      // TODO - implement
      break;
    case IO_CHAR:
      // calculate font location of the specific character X in memory
      chip8->I = FONT_START + x * FONT_HEIGHT; 
      break;
    case IO_BIN_DEC: 
      // extract 3 decimal digits from a number and store them in memory
      chip8->memory[chip8->I] = chip8->V[x] / 100; // hundreds place
      chip8->memory[chip8->I + 1] = (chip8->V[x] / 10) % 10; // tens place
      chip8->memory[chip8->I + 2] = chip8->V[x] % 10; // ones place
      break;
    case IO_SMEM:
      // TODO - make toggle for incrementing I
      // Load all registers up to VX into memory starting at I
      for (int i = 0; i <= x; i++) {
        chip8->memory[chip8->I + i] = chip8->V[i];
      }
      break;
    case IO_LMEM:
      for (int i = 0; i <= x; i++) {
        chip8->V[i] = chip8->memory[chip8->I + i];
      }
      break;
  }
}

void exec_instruction(struct Chip8 *const chip8, unsigned short instruction) {
  unsigned short nnn = instruction & OP_NNN;
  unsigned char nn = instruction & OP_NN;
  unsigned char n = instruction & OP_N;
  unsigned char x = (instruction & OP_X) >> 2;
  unsigned char y = (instruction & OP_Y) >> 1;
  chip8->displaying = 0;

  switch (chip8->opcode) {
    case OP_SYS:
      if (instruction == OP_CLR_SCRN) {
        chip8->displaying = 1;
        // clear the screen
      }
      else if (instruction == OP_RET) {
        // NOTE - I don't think it's necessary to overwrite the stack value?
        chip8->pc = chip8->stack[chip8->sp];
        chip8->sp--;
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
      // skip 1 instruction if VX != NN
    case OP_BNEI:
      if (chip8->V[x] != nn) {
        chip8->pc += 2;
      }
      break;
    case OP_BEQ:
      // skip 1 instruction if VX == VY
      if (chip8->V[x] == chip8->V[y]) {
        chip8->pc += 2;
      }
      break;
    case OP_BNE:
      // skip 1 instruction if VX != VY
      if (chip8->V[x] != chip8->V[y]) {
        chip8->pc += 2;
      }
      break;
    case OP_LI:
      chip8->V[x] = nn;
      break;
    case OP_ADDI:
      chip8->V[x] += nn; 
      break;
    case OP_ALU:
      exec_alu(chip8, x, y, n);
      break;
    case OP_SET_IDX:
      chip8->I = nnn;
      break;
    case OP_JO:
      // TODO: make bug configurable for compatability with SUPER-CHIP and CHIP-48 programs
      chip8->pc = nnn + chip8->V[0];
      break;
    case OP_RAND:
      // generate a random number, do a binary AND with NN, and load it into VX
      chip8->V[x]= nn & rand();
      break;
    case OP_DISPLAY:
      chip8->displaying = 1;
      exec_display(chip8, x, y, n);
      break;
    case OP_BKEY:
      // TODO: handle these operators
      break;
    case OP_IO:
      exec_io(chip8, x, nn);
      break;
  }
}

int exec_cycle(struct Chip8 *const chip8) {
  // NOTE - this isn't complete
  time_t start = time(NULL);

  while (chip8->pc < ADDRESS_COUNT) {
    decrement_timer(&chip8->delay_timer, &start, 0);
    decrement_timer(&chip8->sound_timer, &start, 1);
    
    unsigned short instruction = fetch_instruction(chip8);

    exec_instruction(chip8, instruction);
    
    // TODO - get keypress here
    break;
  }

  return -1;
}
