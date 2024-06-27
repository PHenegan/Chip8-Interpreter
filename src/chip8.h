#ifndef CHIP8
#define CHIP8

#include <SDL2/SDL_mutex.h>
#include <stdbool.h>
#include <stdint.h>

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

typedef struct ConfigFlags {
  int debug;
  int legacy_shift;
  int jump_quirk;
  int legacy_indexing;
} ConfigFlags;

// Represents the state of a CHIP-8 process (Virtual CPU?) at any given point in time
typedef struct Chip8 {
  ConfigFlags config;
  uint8_t memory[ADDRESS_COUNT];
  uint8_t screen[DISPLAY_HEIGHT * DISPLAY_WIDTH];
  uint8_t V[REGISTER_COUNT]; // registers
  uint16_t I; // index register
  uint16_t pc; // program counter (instruction pointer)
  uint16_t stack[STACK_SIZE];
  uint16_t sp; // stack pointer
  uint8_t delay_timer;
  uint8_t sound_timer;
  bool sound_flag;
  uint8_t opcode;
  uint8_t key[KEY_COUNT];
  bool display_flag;
} Chip8;

// set values in the CHIP-8 system to an initial beginning state
Chip8* chip8_init();

// free all memory taken up by a CHIP-8 system
// `chip8`: the CHIP-8 system to free
void chip8_destroy(Chip8* chip8);

// Load a system font into memory
// `chip8`: the CHIP-8 system to load the font into
void load_font(Chip8 *const chip8);

// Load the a program into memory from the given file, returning 0 if successful
// `chip8`: the CHIP-8 system to load the program into
// `file`: the file path of the binary program to load
int load_program(Chip8 *const chip8, char *const file);

// Decrement the two timers present on CHIP-8 systems
// `chip8`: the CHIP-8 system whose timers should be decremented
void chip8_decrement_timers(Chip8 *const chip8);

// Get the next instruction and increment the program_counter by two.
// Returns the next instruction.
// `chip8`: the CHIP-8 system to fetch an instruction for
uint16_t fetch_instruction(Chip8 *const chip8);

#endif
