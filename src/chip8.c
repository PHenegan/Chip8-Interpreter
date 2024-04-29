#include "chip8.h"
#include <fcntl.h>
#include <time.h>
#include <unistd.h>


void initialize_system(struct Chip8 *chip8) {
  chip8->pc = PROGRAM_START;
  chip8->I = 0;
  chip8->sp = 0;
  chip8->opcode = 0;
  chip8->displaying = 0;
  chip8->sound_flag = 0;

  // Initialize all addresses in memory to 0
  for (int addr = 0; addr < ADDRESS_COUNT; addr++) {
    chip8->memory[addr] = 0;
  }
  // Initialize all registers to 0
  for (int reg = 0; reg < REGISTER_COUNT; reg++) {
    chip8->V[reg] = 0;
  }
  // Initialize the stack elements to 0
  for (int i = 0; i < STACK_SIZE; i++) {
    chip8->stack[i] = 0;
  }
  // Initialize all keys to 0 (unpressed)
  for (int key = 0; key < KEY_COUNT; key++) {
    chip8->key[key] = 0;
  }

  // Initialize each pixel in the screen to 0
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      chip8->screen[y][x] = 0;
    }
  }

  load_font(chip8);
}

int load_program(struct Chip8 *chip8, char *file) {
  // get the file descriptor of the program to load
  int fd = open(file, O_RDONLY);
  
  // NOTE - I saw something suggesting to use fopen
  // but I think this should also work?

  // read the program into the fist section of the chip8's memory,
  // going up to the end of memory
  int program_size = ADDRESS_COUNT - PROGRAM_START;
  int count = read(fd, chip8->memory + PROGRAM_START, program_size);
  
  close(fd);
  return count;
}

void load_font(struct Chip8 *chip8) {
  // each row is a bitmap describing each commented symbol
  // e.g. 0 will look like
  // "11111"
  // "10001"
  // "10001"
  // "10001"
  // "11111"
  // which draws the shape of a 0 (note the second bit is completely ignored and always 0)
  char font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 1
    0x20, 0x60, 0x20, 0x20, 0x70, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };
  int total_size = FONT_HEIGHT * KEY_COUNT;
  
  // load the font into memory
  for (int i = 0; i < total_size; ++i) {
    chip8->memory[FONT_START + i] = font[i];
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


