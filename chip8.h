// number of bytes of memory
#define ADDRESS_COUNT 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define REGISTER_COUNT 16
#define STACK_SIZE 16
// number of times per second the timer will update
#define TIMER_FREQUENCY 60
#define KEY_COUNT 16
// number of instructions per second
#define INSTRUCTION_FREQUENCY 700

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

void load_program(struct Chip8* chip8, char* file, int size);

void load_font(struct Chip8*);
