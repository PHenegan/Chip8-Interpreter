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

// Instruction decoding bitmasks
#define OP_MASK 0xF000
#define OP_N 0x000F
#define OP_NN 0x00FF
#define OP_NNN 0x0FFF
#define OP_X 0x0F00
#define OP_Y 0x00F0

// Specific Instructions
#define OP_SYS 0x0
#define OP_CLR_SCRN 0x00E0
#define OP_RET 0x00EE
#define OP_JUMP 0x1
#define OP_CALL 0x2
#define OP_BEQI 0x3
#define OP_BNEI 0x4
#define OP_BEQ 0x5
#define OP_BNE 0x9
#define OP_LI 0x6
#define OP_ADDI 0x7
#define OP_ALU 0x8
#define OP_SET_IDX 0xA // NOTE - I don't know what I (index) is used for
#define OP_JO 0xB // Jump w/ OFFSET
#define OP_RAND 0xC
#define OP_DISPLAY 0xD
#define OP_BKEY 0xE
#define OP_IO 0xF

// ALU instructions
#define ALU_SET 0x0
#define ALU_OR 0x1
#define ALU_AND 0x2
#define ALU_XOR 0x3
#define ALU_ADD 0x4
#define ALU_SUBY 0x5
#define ALU_SRL 0x6
#define ALU_SUBX 0x7
#define ALU_SLL 0xE

// Branch keyboard instructions
#define BK_P 0x9E // skip when a key is pressed
#define BK_NP 0xA1 // skip when a key is not pressed

// IO instructions (begin with 0xF)
#define IO_LDTIME 0x07
#define IO_SDTIME 0x15
#define IO_SSTIME 0x18
#define IO_ADD_IDX 0x1E
#define IO_GET_KEY 0x0A
#define IO_CHAR 0x29
#define IO_BIN_DEC 0x33
#define IO_SMEM 0x55
#define IO_LMEM 0x65

// Represents the state of a CHIP-8 process (Virtual CPU?) at any given point in time
struct Chip8 {
  unsigned char memory[ADDRESS_COUNT];
  unsigned short screen[DISPLAY_HEIGHT][DISPLAY_WIDTH];
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
};

// Load the a program into memory from the given file
int load_program(struct Chip8 *chip8, char *file);

// set values in the CHIP-8 system to an initial beginning state
void initialize_system(struct Chip8 *chip8);

// Load a system font into memory
void load_font(struct Chip8 *chip8);

// Execute a single fetch-decode-execute cycle of the CHIP-8 system
int exec_cycle(struct Chip8 *chip8);
