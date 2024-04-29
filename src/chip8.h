#ifndef CHIP8
#define CHIP8

#include <time.h>

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
  unsigned char delay_timer;
  unsigned char sound_timer;
  unsigned char opcode;
  unsigned char key[KEY_COUNT];
  char displaying;
  char sound_flag;
};

// Load the a program into memory from the given file
int load_program(struct Chip8 *const chip8, char *const file);

// set values in the CHIP-8 system to an initial beginning state
void initialize_system(struct Chip8 *const chip8);

// Load a system font into memory
void load_font(struct Chip8 *const chip8);

// Get the next instruction and increment the program_counter by two
unsigned short fetch_instruction(struct Chip8 *const chip8);



// Execute a single fetch-decode-execute cycle of the CHIP-8 system
// `chip8`: the chip8 processor on which a cycle will be executed 
// `start`: a timestamp used for updating the chip8's timers
int exec_cycle(struct Chip8 *chip8, time_t* start);

// Execute the program currently stored in the CHIP-8's memory
// `chip8`: the chip8 processor to load the program from
int exec_program(struct Chip8 *chip8);

#endif
