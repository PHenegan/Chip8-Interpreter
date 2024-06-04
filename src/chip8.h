#ifndef CHIP8
#define CHIP8

#include <SDL2/SDL_mutex.h>

// number of bytes of memory
#define ADDRESS_COUNT 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
// number of times per second the timer will update
#define TIMER_FREQUENCY 60

// Constants related to display
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define FONT_HEIGHT 5
#define KEY_COUNT 16

// Constants related to memory addresses
#define PROGRAM_START 0x200
#define FONT_START 0x050

// Represents the state of a CHIP-8 process (Virtual CPU?) at any given point in time
struct Chip8 {
  unsigned char memory[ADDRESS_COUNT];
  unsigned char screen[DISPLAY_HEIGHT][DISPLAY_WIDTH];
  unsigned char V[REGISTER_COUNT]; // registers
  unsigned short I; // index register
  unsigned short pc; // program counter (instruction pointer)
  unsigned short stack[STACK_SIZE];
  unsigned short sp; // stack pointer
  SDL_mutex* timer_mutex;
  unsigned char delay_timer;
  unsigned char sound_timer;
  char sound_flag;
  unsigned char opcode;
  unsigned char key[KEY_COUNT];
  char displaying;
};

// Load the a program into memory from the given file, returning 0 if successful
// `chip8`: the CHIP-8 system to load the program into
// `file`: the file path of the binary program to load
int load_program(struct Chip8 *const chip8, char *const file);

// set values in the CHIP-8 system to an initial beginning state
// `chip8`: the CHIP-8 system to initialize
void initialize_system(struct Chip8 *const chip8);

// Load a system font into memory
// `chip8`: the CHIP-8 system to load the font into
void load_font(struct Chip8 *const chip8);

// Get the next instruction and increment the program_counter by two.
// Returns the next instruction.
// `chip8`: the CHIP-8 system to fetch an instruction for
unsigned short fetch_instruction(struct Chip8 *const chip8);

#endif
