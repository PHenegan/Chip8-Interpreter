#include "control.h"
#include "chip8-timer.h"
#include "chip8.h"
#include "stdio.h"
#include "view.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <time.h>

const int CATEGORY = SDL_LOG_CATEGORY_APPLICATION;

void exec_alu(Chip8 *const chip8, uint8_t x, uint8_t y, uint8_t n) {
  short result;
  char* log_msg = NULL;
  switch (n) {
    case ALU_SET:
      SDL_LogDebug(CATEGORY, "V[%d] (%d) = V[%d] (%d)", x, chip8->V[x], y, chip8->V[y]);
      chip8->V[x] = chip8->V[y];
      break;

    case ALU_OR:
      SDL_LogDebug(CATEGORY, "V[%d] = %d | %d = %d", 
          x, chip8->V[x], chip8->V[y], chip8->V[x] | chip8->V[y]);
      chip8->V[x] = chip8->V[x] | chip8->V[y];
      break;

    case ALU_AND:
      SDL_LogDebug(CATEGORY, "V[%d] = %d & %d = %d", 
          x, chip8->V[x], chip8->V[y], chip8->V[x] & chip8->V[y]);
      chip8->V[x] = chip8->V[x] & chip8->V[y];
      break;

    case ALU_XOR:
      SDL_LogDebug(CATEGORY, "V[%d] = %d ^ %d = %d", 
          x, chip8->V[x], chip8->V[y], chip8->V[x] ^ chip8->V[y]);
      chip8->V[x] = chip8->V[x] ^ chip8->V[y];
      break;

    case ALU_ADD:
      result = chip8->V[x] + chip8->V[y];
      SDL_LogDebug(CATEGORY, "V[%d] = %d + %d = %d, ovf = %d", 
          x, chip8->V[x], chip8->V[y], (uint8_t)result, result > 255);
      chip8->V[x] = (uint8_t)result;
      chip8->V[0xF] = result > 255;
      break;

    // SUB (VX - VY)
    case ALU_SUBY:
      result = chip8->V[x] - chip8->V[y];
      SDL_LogDebug(CATEGORY, "V[%d] = %d - %d = %d, ovf = %d", 
          x, chip8->V[x], chip8->V[y], (uint8_t)result, (uint8_t)result > chip8->V[x]); 
      // The overflow flag for subtraction is actually the opposite of what you expect
      chip8->V[0xF] = chip8->V[y] <= chip8->V[x];
      chip8->V[x] = (uint8_t)result;
      break;

    case ALU_SRL:
      if (chip8->config.legacy_shift) {
        chip8->V[x] = chip8->V[y];
      }
      SDL_LogDebug(CATEGORY, "V[%d] = %d >> 1, ovf = %d", x, chip8->V[x], chip8->V[x] & 1);
      // shift VX right by 1, storing the shifted bit into VF
      chip8->V[0xF] = chip8->V[x] & 1; // isolate the last bit
      chip8->V[x] = chip8->V[x] >> 1;
      break;

    // SUB (VY - VX)
    case ALU_SUBX:
      result = chip8->V[y] - chip8->V[x];
      SDL_LogDebug(CATEGORY, "V[%d] = %d - %d = %d, ovf = %d", 
          x, chip8->V[y], chip8->V[x], (uint8_t)result, result > 255);
      // The overflow flag for subtraction is actually the opposite of what you expect
      chip8->V[0xF] = chip8->V[x] <= chip8->V[y];
      chip8->V[x] = (uint8_t)result;
      break;

    case ALU_SLL:
      if (chip8->config.legacy_shift) {
        chip8->V[x] = chip8->V[y];
      }

      SDL_LogDebug(CATEGORY, "V[%d] = %d << 1 = %d, ovf = %d",
          x, chip8->V[x], (uint8_t)(chip8->V[x] << 1), (chip8->V[x] & 0x80) != 0);
      // shift VX left by 1, storing the shifted bit into VF
      chip8->V[0xF] = (chip8->V[x] & 0x80) != 0; // isolate the first bit
      chip8->V[x] = chip8->V[x] << 1;
      break;
  }
}

void exec_display(struct Chip8 *const chip8, uint8_t x, uint8_t y, uint8_t n) {
  uint8_t x_pos = chip8->V[x] % DISPLAY_WIDTH;
  uint8_t y_pos = chip8->V[y] % DISPLAY_HEIGHT;

  chip8->V[0xF] = 0; // reset flag register

  for (int row = 0; row < n && y_pos + row < DISPLAY_HEIGHT; row++) {
    uint8_t draw_byte = chip8->memory[chip8->I + row];
    // 8 is hardcoded because bytes are used, so at most 8 pixels can be set.
    uint8_t target_bit =  0x80;
    for (int col = 0; col < 8 && x_pos + col < DISPLAY_WIDTH; col++) {
      uint8_t* pixel = &chip8->screen[(y_pos + row) * DISPLAY_WIDTH + x_pos + col];
      uint8_t flip_bit = (draw_byte & target_bit);

      // If a bit gets turned off by the draw byte, the flag register gets set to 1
      chip8->V[0xF] = chip8->V[0xF] || (flip_bit) && *pixel;
      *pixel = flip_bit ? !*pixel : *pixel; 
      target_bit = target_bit >> 1;
    }
  }

  SDL_LogDebug(CATEGORY, 
      "Display called: V[%d] = %d, V[%d] = %d, n = %d, V[0xF] = %d after instruction",
      x, chip8->V[x], y, chip8->V[y], n, chip8->V[0xF]);
}

// Gets the first currently pressed key it can find, setting the out parameter
// to the first pressed key. NOTE - idk if this is a correct implementation?
char get_pressed_key(struct Chip8 *const chip8, char* key) {
  char found = 0;
  for (uint8_t curr_key = 0; curr_key < KEY_COUNT; curr_key++) {
    if (chip8->key[curr_key]) {
      found = 1;
      *key = curr_key;
      break;
    }
  }
  return found;
}

void exec_io(struct Chip8 *const chip8, uint8_t x, uint8_t nn) {
  char result;
  switch (nn) {
    case IO_LDTIME:
      SDL_LogDebug(CATEGORY, "set register %d to timer value %d", x, chip8->delay_timer);
      chip8->V[x] = chip8->delay_timer;
      break;

    case IO_SDTIME:
      SDL_LogDebug(CATEGORY, "set delay timer to value V[%d]: %d", x, chip8->V[x]);
      chip8->delay_timer = chip8->V[x];
      break;
    
    case IO_SSTIME:
      SDL_LogDebug(CATEGORY, "set sound timer to value V[%d]: %d", x, chip8->V[x]);
      chip8->sound_timer = chip8->V[x];
      break;
    
    case IO_ADD_IDX:
      SDL_LogDebug(CATEGORY, "I changed from %d - added %d, resulting in %d",
          chip8->I, chip8->V[x], (chip8->I + chip8->V[x]) & 0x0FFF);
      chip8->I += chip8->V[x];
      // I should only take up 12 bits, anything else is treated as an overflow
      chip8->V[0xF] = chip8->I >= 0x1000;
      chip8->I = chip8->I & 0x0FFF;
      break;

    case IO_GET_KEY:
      if (!get_pressed_key(chip8, &result)) {
        SDL_LogDebug(CATEGORY, "no input pressed, instruction pointer decremented");
        chip8->sp -= 2;
      } else {
        SDL_LogDebug(CATEGORY, "received pressed key %x", result);
        // technically converts from char to uint8, but the keys go
        // from 0-16 so this isn't really an issue
        chip8->V[x] = result;
      }
      break;

    case IO_CHAR:
      // calculate font location of the specific character X in memory
      SDL_LogDebug(CATEGORY, "Changing index from %d to %d based on font character %x",
          chip8->I, FONT_START + chip8->V[x] * FONT_HEIGHT, chip8->V[x]);
      chip8->I = FONT_START + chip8->V[x] * FONT_HEIGHT; 
      break;

    case IO_BIN_DEC:
      SDL_LogDebug(CATEGORY, "Setting memory locations %d, %d, %d to %d, %d, %d based on number %d",
          chip8->I, chip8->I + 1, chip8->I + 2, 
          chip8->V[x] / 100, (chip8->V[x] / 10) % 10, chip8->V[x] % 10, chip8->V[x]);

      // extract 3 decimal digits from a number and store them in memory
      chip8->memory[chip8->I] = chip8->V[x] / 100; // hundreds place
      chip8->memory[chip8->I + 1] = (chip8->V[x] / 10) % 10; // tens place
      chip8->memory[chip8->I + 2] = chip8->V[x] % 10; // ones place
      break;

    case IO_SMEM:
      // Load all registers up to VX into memory starting at I
      for (int i = 0; i <= x; i++) {
        SDL_LogDebug(CATEGORY, "Filling memory location %d with value V[%d]: %d",
            chip8->I + i, i, chip8->V[i]);

        chip8->memory[chip8->I + i] = chip8->V[i];
      }

      // On the original CHIP-8 systems, I gets incremented for each value it loads in
      if (chip8->config.legacy_indexing) {
        chip8->I += x;
        SDL_LogDebug(CATEGORY, 
            "legacy indexing flag present, incrementing index by %d (new val = %d)", x, chip8->I);
      }
      break;

    case IO_LMEM:
      for (int i = 0; i <= x; i++) {
        SDL_LogDebug(CATEGORY, "Loading register V[%d] from memory location %d with value %d",
            i, chip8->I + i, chip8->memory[chip8->I + i]);
        chip8->V[i] = chip8->memory[chip8->I + i];
      }
      break;
  }
}

// Reset all pixels on a CHIP-8's screen to be blank
void clear_screen(struct Chip8 *const chip8) {
  
  SDL_LogDebug(CATEGORY, "clearing screen");

  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      chip8->screen[y * DISPLAY_WIDTH + x] = 0;
    }
  }
}

void exec_instruction(Chip8 *const chip8, uint16_t instruction) {
  // OP (4 bits), x (4 bits), y (4 bits), n (4 bits)
  uint16_t nnn = instruction & OP_NNN;
  uint8_t nn = instruction & OP_NN;
  uint8_t n = instruction & OP_N;
  uint8_t x = (instruction & OP_X) >> 8;
  uint8_t y = (instruction & OP_Y) >> 4;
  chip8->display_flag = 0;
  char* branch_msg;
  char* ext_msg;

  switch (chip8->opcode) {
    case OP_SYS:
      if (instruction == OP_CLR_SCRN) {
        chip8->display_flag = 1;
        clear_screen(chip8);
      } else if (instruction == OP_RET) {
        // NOTE - I don't think it's necessary to overwrite the stack value?
        SDL_LogDebug(CATEGORY, "Return reached, setting pc to %d and decrementing sp to %d", 
            chip8->stack[chip8->sp], chip8->sp - 1);

        chip8->pc = chip8->stack[chip8->sp];
        chip8->sp--;
      }
      break;

    case OP_JUMP:
      SDL_LogDebug(CATEGORY, "Jump reached, setting pc to %d", nnn);
      chip8->pc = nnn; 
      break;

    case OP_CALL:
      SDL_LogDebug(CATEGORY, "Call reached, adding pc (%d) to the stack and setting pc to %d",
          chip8->pc, nnn);
      chip8->sp++;
      chip8->stack[chip8->sp] = chip8->pc;
      chip8->pc = nnn;
      break;

    case OP_BEQI:
      branch_msg = "sides not equal, did not branch";
      // skip 1 instruction if VX == NN 
      if (chip8->V[x] == nn) {
        branch_msg = "sides equal, branching";
        chip8->pc += 2;
      }
      
      SDL_LogDebug(CATEGORY, "BEQI - Comparing %d with %d: %s, pc: %d",
          chip8->V[x], nn, branch_msg, chip8->pc);
      break;

      // skip 1 instruction if VX != NN
    case OP_BNEI:
      branch_msg = "sides equal, did not branch";
      if (chip8->V[x] != nn) {
        branch_msg = "sides not equal, branching";
        chip8->pc += 2;
      }
      SDL_LogDebug(CATEGORY, "BNEI - Comparing %d with %d: %s, pc: %d",
          chip8->V[x], nn, branch_msg, chip8->pc);
      break;
    
    case OP_BEQ:
      branch_msg = "sides not equal, did not branch";
      // skip 1 instruction if VX == VY
      if (chip8->V[x] == chip8->V[y]) {
        branch_msg = "sides equal, branching";
        chip8->pc += 2;
      }
      SDL_LogDebug(CATEGORY, "BEQ - Comparing %d with %d: %s, pc: %d",
          chip8->V[x], chip8->V[y], branch_msg, chip8->pc);
      break;
    
    case OP_BNE:
      branch_msg = "sides equal, did not branch";
      // skip 1 instruction if VX != VY
      if (chip8->V[x] != chip8->V[y]) {
        branch_msg = "sides not equal, branching";
        chip8->pc += 2;
      }
      SDL_LogDebug(CATEGORY, "BNE - Comparing %d with %d: %s, pc: %d",
          chip8->V[x], chip8->V[y], branch_msg, chip8->pc);
      break;
    
    case OP_LI:
      SDL_LogDebug(CATEGORY, "LI - Setting V[%d] to immediate value %d", x, nn);
      chip8->V[x] = nn;
      break;
    
    case OP_ADDI:
      SDL_LogDebug(CATEGORY, "ADDI - V[%d] = %d + %d (%d)", x, chip8->V[x], nn, chip8->V[x] + nn);
      chip8->V[x] += nn; 
      break;

    case OP_ALU:
      exec_alu(chip8, x, y, n);
      break;

    case OP_SET_IDX:
      SDL_LogDebug(CATEGORY, "setting I register (%d) to %d", chip8->I, nnn);
      chip8->I = nnn;
      break;

    case OP_JO:
      // A side effect introduced in CHIP-48 and SUPER-CHIP systems that was likely a bug
      if (chip8->config.jump_quirk) {
        SDL_LogDebug(CATEGORY, "Jump w/ Offset (w/ quirk) - setting pc to %d + V[%d] (%d) = %d",
            nnn, x, chip8->V[x], nnn + chip8->V[x]);
        chip8->pc = nnn + chip8->V[x];
      } else {
        SDL_LogDebug(CATEGORY, "Jump w/ Offset - setting pc to %d + %d (%d)",
            nnn, chip8->V[0], nnn + chip8->V[0]);
        chip8->pc = nnn + chip8->V[0];
      }
      break;

    case OP_RAND:
      
      // generate a random number, do a binary AND with NN, and load it into VX
      // NOTE - this is bad but I don't want to make a new variable
      n = rand();
      SDL_LogDebug(CATEGORY, "RAND - setting V[%d] to %d (rand) & %d", x, n, nn);
      chip8->V[x]= nn & n;
      break;
    
    case OP_DISPLAY:
      chip8->display_flag = 1;
      exec_display(chip8, x, y, n);
      break;
    
    case OP_BKEY:
      branch_msg = "condition not met, not branching";
      // Skip 1 instruction if either "skip if pressed" or "skip if not pressed" are being used
      if ((nn == BK_P && chip8->key[chip8->V[x]]) || (nn == BK_NP && !chip8->key[chip8->V[x]])) {
        branch_msg = "condition met, branching";
        chip8->pc += 2;
      }
      if (nn == BK_P) {
        ext_msg = "BKEY";
      } else if (nn == BK_NP) {
        ext_msg = "BNKEY";
      } else {
        ext_msg = "Bad instruction";
      }
      SDL_LogDebug(CATEGORY, "%s - Checking if key %x is pressed: %s, pc: %d",
          ext_msg, chip8->V[x], branch_msg, chip8->pc);
      break;
    
    case OP_IO:
      exec_io(chip8, x, nn);
      break;
  }

}

int exec_cycle(Chip8 *const chip8, struct View *const view) {
  // reset the play_sound flag

  // NOTE - get_input technically doesn't error here, but if a quit signal is pressed
  // then the program should stop running
  int error = view_get_input(chip8->key, KEY_COUNT);
  if (error) {
    return error;
  }

  uint16_t instruction = fetch_instruction(chip8);
  SDL_LogDebug(CATEGORY, "fetched instruction %04x at address %d", instruction, chip8->pc - 2);

  exec_instruction(chip8, instruction);
  
  if (chip8->display_flag) {
    view_draw(view, chip8->screen);
  }

  view_set_sound(view, chip8->sound_flag);

  return 0;
}

int exec_program(Chip8 *const chip8, struct View *const view) {
  // Timers decrement every second, and the sound timer will update the chip8's
  // sound flag to indicate when a sound should be played
  uint64_t last_time = SDL_GetTicks64();
  int manual = 1; // flag for manually stepping through instructions in debug mode
  
  while (chip8->pc < ADDRESS_COUNT) {
    uint64_t current_time = SDL_GetTicks64();
    if (current_time > last_time + (int)(1.0 / TIMER_FREQUENCY * 1000)) {
      chip8_decrement_timers(chip8);
      last_time = current_time;
    }

    int result = exec_cycle(chip8, view);
    if (result) {
      return result;
    }
    // calculate the timing to sleep in nanoseconds and wait that long
    // before running the next instruction
    precise_sleep((long)(1.0 / INSTRUCTION_FREQUENCY * 1000000000));

    // additional debug step for manually stepping through instructions
    if (chip8->config.debug) {
      int key_count;
      SDL_PumpEvents();
      const uint8_t* keystate = SDL_GetKeyboardState(&key_count);
      bool paused = true;
      while (paused) {
        SDL_PumpEvents();
        if (keystate[SDL_SCANCODE_RETURN]) {
          manual = !manual;
          SDL_Delay(200);
          break;
        }
        if (!manual) {
          break;
        }
        view_set_sound(view, 0);
        paused = !keystate[SDL_SCANCODE_N] && manual;
      }
      view_set_sound(view, chip8->sound_flag);
      if (manual) {
        SDL_Delay(250);
      }
    }
  }

  return 0;
}

