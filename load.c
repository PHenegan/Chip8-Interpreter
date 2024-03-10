#include "chip8.h"
#include <fcntl.h>
#include <unistd.h>

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

void initialize_system(struct Chip8 *chip8) {
  chip8->pc = PROGRAM_START;
  chip8->I = 0;
  chip8->sp = 0;
  chip8->opcode = 0;

  load_font(chip8);
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
