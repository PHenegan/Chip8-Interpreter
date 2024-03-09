// number of bytes of memory
#define ADDRESS_COUNT 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
// number of times per second the timer will update
#define TIMER_FREQUENCY 60
// number of instructions per second
#define INSTRUCTION_FREQUENCY 700

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
  unsigned short screen[DISPLAY_HEIGHT][DISPLAY_WIDTH];
  unsigned char V[REGISTER_COUNT]; // registers
  unsigned short I; // instruction pointer
  unsigned short pc; // program counter
  unsigned short stack[STACK_SIZE];
  unsigned short sp; // stack pointer
  unsigned char delay_timer;
  unsigned char sound_timer;
  unsigned char opcode;
  unsigned char key[KEY_COUNT];
};

// Load the a program into memory from the given file
int load_program(struct Chip8 *chip8, char *file);

// Load a system font into memory
void load_font(struct Chip8 *chip8);
