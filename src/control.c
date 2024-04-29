#include "control.h"
#include <stdio.h>
#include <stdlib.h>

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
  unsigned char x_pos = chip8->V[x] % DISPLAY_WIDTH;
  unsigned char y_pos = chip8->V[y] % DISPLAY_HEIGHT;

  chip8->V[0xF] = 0; // reset flag register

  for (int row = 0; row < n && y_pos + row < DISPLAY_HEIGHT; row++) {
    unsigned char draw_byte = chip8->memory[chip8->I + row];
    // 8 is hardcoded because bytes are used, so at most 8 pixels can be set.
    unsigned char target_bit =  1;
    for (int col = 0; col < 8 && x_pos + col < DISPLAY_WIDTH; col++) {
      unsigned char* pixel = &chip8->screen[y_pos + row][x_pos + col];
      unsigned char new_value = draw_byte & target_bit; 
      if (*pixel && !new_value) {
        chip8->V[0xF] = 1;
      }
      *pixel = new_value;
      target_bit = target_bit << 1;
    }
  }
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

void clear_screen(struct Chip8 *const chip8) {
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      chip8->screen[y][x] = 0;
    }
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
        clear_screen(chip8);
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
      // Skip 1 instruction if either "skip if pressed" or "skip if not pressed" are being used
      if ((nn == 0x9E && chip8->key[x]) || (nn == 0xA1 && !chip8->key[x])) {
        chip8->pc += 2;
      }
      break;
    case OP_IO:
      exec_io(chip8, x, nn);
      break;
  }
}

int exec_cycle(struct Chip8 *const chip8, time_t* start) {
  char play_sound = 0;

  // TODO - maybe make this loop separately on a different thread,
  // and add a mutex for each timer?
  decrement_timer(&chip8->delay_timer, start, NULL);
  decrement_timer(&chip8->sound_timer, start, &play_sound);

  unsigned short instruction = fetch_instruction(chip8);

  // TODO - get inputs here
  exec_instruction(chip8, instruction);
  
  // TODO - draw display here
  // Alternatively - put display drawing in another separate thread,
  // update at 60 fps
  return 0;
}

int exec_program(struct Chip8 *const chip8) {
  // NOTE - this isn't complete
  time_t start = time(NULL);

  while (chip8->pc < ADDRESS_COUNT) {
    exec_cycle(chip8, &start);
    break;
  }

  fprintf(stderr, "Error: this program is not complete\n");
  return -1;
}
