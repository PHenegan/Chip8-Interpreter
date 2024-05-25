#ifndef CONTROL
#define CONTROL

#include "chip8.h"
#include "view.h"

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
// `chip8`: the chip8 processor on which the alu instruction will be executed
// TODO - this can be generalized more - all of the instructions
// store results in VX, maybe this can be changed into a function that just takes in the ALU op
// and params, and outputs the result (maybe also take in an `ovf` pointer)
// `chip8`: the chip8 processor on which the ALU operation will be executed
// `x`: the 4-bit number taken from the 2nd hex digit of the instruction
// `y`: the 4-bit number taken from the 3rd hex digit of the instruction
// `n`: the 4-bit number taken from the 4th (last) hex digit of the instruction
void exec_alu(struct Chip8 *const chip8, unsigned char x, unsigned char y, unsigned char n);

// Execute the display instruction for the CHIP-8.
// NOTE: This does not actually draw and pixels through SDL. It will update the
// draw frame (buffer?) and the draw flag of the CHIP-8. This allows more flexibility
// and separation between control logic and view logic.
// `chip8`: the chip8 processor on which the display instruction will be executed
// `x`: the 4-bit number taken from the 2nd hex digit of the instruction
// `y`: the 4-bit number taken from the 3rd hex digit of the instruction
// `n`: the 4-bit number taken from the 4th (last) hex digit of the instruction
void exec_display(struct Chip8 *const chip8, unsigned char x, unsigned char y, unsigned char n);

// Execute an IO instruction for the CHIP-8.
// `x`: the 4-bit number taken from the 2nd hex digit of the instruction
// `nn`: the 8-bit number taken from the 3rd and 4th (last 2) hex digits of the instruction
void exec_io(struct Chip8 *const chip8, unsigned char x, unsigned char nn);

// Execute a single fetch-decode-execute cycle for an instruction on the CHIP-8 system
// `chip8`: the chip8 processor on which a cycle will be executed 
// `view`: the object used to display the state of the CHIP-8 to the user
int exec_cycle(struct Chip8 *const chip8, struct View *const view);

// Execute the program currently stored in the CHIP-8's memory
// `chip8`: the chip8 processor to load the program from
// `view`: the object used to display the state of the CHIP-8 to the user
int exec_program(struct Chip8 *chip8, struct View *const view);

#endif
