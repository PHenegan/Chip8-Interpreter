#ifndef CONTROL
#define CONTROL

#include "chip8.h"

// number of instructions per second
#define INSTRUCTION_FREQUENCY 700

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

// Execute an ALU operation using the CHIP-8
// TODO - this can be generalized more - all of the instructions
// store results in VX, maybe this can be changed into a function that just takes in the ALU op
// and params, and outputs the result (maybe also take in an `ovf` pointer)
void exec_alu(struct Chip8 *const chip8, unsigned char x, unsigned char y, unsigned char n);

int exec_cycle(struct Chip8 *const chip8, time_t* start);

#endif
