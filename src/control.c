#include "control.h"
#include "chip8.h"
#include "stdio.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <time.h>

void log_message(char* message, const Chip8 *const chip8) {
  if (chip8->config.debug) {
    printf("%s\n", message);
  }
}

void exec_alu(Chip8 *const chip8, uint8 x, uint8 y, uint8 n) {
  short result;
  char* log_msg = NULL;
  switch (n) {
    case ALU_SET:
      asprintf(&log_msg, "V[%d] (%d) = V[%d] (%d)", 
               x, chip8->V[x], y, chip8->V[y]);
      chip8->V[x] = chip8->V[y];
      break;

    case ALU_OR:
      asprintf(&log_msg, "V[%d] = %d | %d = %d", 
               x, chip8->V[x], chip8->V[y], chip8->V[x] | chip8->V[y]);
      chip8->V[x] = chip8->V[x] | chip8->V[y];
      break;

    case ALU_AND:
      asprintf(&log_msg, "V[%d] = %d & %d = %d", 
               x, chip8->V[x], chip8->V[y], chip8->V[x] & chip8->V[y]);
      chip8->V[x] = chip8->V[x] & chip8->V[y];
      break;
    case ALU_XOR:
      asprintf(&log_msg, "V[%d] = %d ^ %d = %d", 
               x, chip8->V[x], chip8->V[y], chip8->V[x] ^ chip8->V[y]);
      chip8->V[x] = chip8->V[x] ^ chip8->V[y];
      break;
    case ALU_ADD:
      result = chip8->V[x] + chip8->V[y];
      asprintf(&log_msg, "V[%d] = %d + %d = %d, ovf = %d", 
               x, chip8->V[x], chip8->V[y], (uint8)result, result > 255);
      chip8->V[x] = (uint8)result;
      chip8->V[0xF] = result > 255;
      break;
    // SUB (VX - VY)
    case ALU_SUBY:
      result = chip8->V[x] - chip8->V[y];
      asprintf(&log_msg, "V[%d] = %d - %d = %d, ovf = %d", 
               x, chip8->V[x], chip8->V[y], (uint8)result, (uint8)result > chip8->V[y]); 
      chip8->V[x] = (uint8)result;
      chip8->V[0xF] = (uint8)result > chip8->V[x];
      break;
    case ALU_SRL:
      if (chip8->config.legacy_shift) {
        chip8->V[x] = chip8->V[y];
      }
      asprintf(&log_msg, "V[%d] = %d >> 1, ovf = %d", x, chip8->V[x], chip8->V[x] & 1);
      // shift VX right by 1, storing the shifted bit into VF
      chip8->V[0xF] = chip8->V[x] & 1; // isolate the last bit
      chip8->V[x] = chip8->V[x] >> 1;
      break;
    // SUB (VY - VX)
    case ALU_SUBX:
      asprintf(&log_msg, "V[%d] = %d | %d = %d", 
               x, chip8->V[x], chip8->V[y], chip8->V[x] | chip8->V[y]);
      chip8->V[x] = chip8->V[y] - chip8->V[x];
      chip8->V[0xF] = chip8->V[x] > chip8->V[y];
      break;
    case ALU_SLL:
      if (chip8->config.legacy_shift) {
        chip8->V[x] = chip8->V[y];
      }
      asprintf(&log_msg, "V[%d] = %d << 1, ovf = %d", x, chip8->V[x], chip8->V[x] & (uint8)0x8000);
      // shift VX left by 1, storing the shifted bit into VF
      chip8->V[0xF] = chip8->V[x] & (uint8)0x8000; // isolate the first bit
      chip8->V[x] = chip8->V[x] << 1;
      break;
  }
  if (log_msg != NULL) {
    log_message(log_msg, chip8);
    free(log_msg);
  }
}

void exec_display(struct Chip8 *const chip8, uint8 x, uint8 y, uint8 n) {
  uint8 x_pos = chip8->V[x] % DISPLAY_WIDTH;
  uint8 y_pos = chip8->V[y] % DISPLAY_HEIGHT;

  chip8->V[0xF] = 0; // reset flag register

  for (int row = 0; row < n && y_pos + row < DISPLAY_HEIGHT; row++) {
    uint8 draw_byte = chip8->memory[chip8->I + row];
    // 8 is hardcoded because bytes are used, so at most 8 pixels can be set.
    uint8 target_bit =  0x80;
    for (int col = 0; col < 8 && x_pos + col < DISPLAY_WIDTH; col++) {
      uint8* pixel = &chip8->screen[(y_pos + row) * DISPLAY_WIDTH + x_pos + col];
      uint8 new_value = draw_byte & target_bit;
      if (*pixel && !new_value) {
        chip8->V[0xF] = 1;
      }
      *pixel = new_value;
      target_bit = target_bit >> 1;
    }
  }
}

// Gets the first currently pressed key it can find, setting the out parameter
// to the first pressed key. NOTE - idk if this is a correct implementation?
char get_pressed_key(struct Chip8 *const chip8, char* key) {
  char found = 0;
  for (uint8 curr_key = 0; curr_key < KEY_COUNT; curr_key++) {
    if (chip8->key[curr_key]) {
      found = 1;
      *key = curr_key;
      break;
    }
  }
  return found;
}

void exec_io(struct Chip8 *const chip8, uint8 x, uint8 nn) {
  char result;
  char* log_msg = NULL;
  switch (nn) {
    case IO_LDTIME:
      SDL_LockMutex(chip8->timer_mutex);
      
      asprintf(&log_msg, "set register %d to timer value %d", x, chip8->delay_timer);
      chip8->V[x] = chip8->delay_timer;
      
      SDL_UnlockMutex(chip8->timer_mutex);
      break;

    case IO_SDTIME:
      asprintf(&log_msg, "set delay timer to value V[%d]: %d", x, chip8->V[x]);
      
      SDL_LockMutex(chip8->timer_mutex);
      chip8->delay_timer = chip8->V[x];
      SDL_UnlockMutex(chip8->timer_mutex);
      break;
    
    case IO_SSTIME:
      asprintf(&log_msg, "set sound timer to value V[%d]: %d", x, chip8->V[x]);
      
      SDL_LockMutex(chip8->timer_mutex);
      chip8->sound_timer = chip8->V[x];
      SDL_UnlockMutex(chip8->timer_mutex);
      break;
    
    case IO_ADD_IDX:
      asprintf(&log_msg, "I changed from %d - added %d, resulting in %d", 
               chip8->I, chip8->V[x], (chip8->I + chip8->V[x]) & 0x0FFF);
      chip8->I += chip8->V[x];
      // I should only take up 12 bits, anything else is treated as an overflow
      chip8->V[0xF] = chip8->I >= 0x1000;
      chip8->I = chip8->I & 0x0FFF;
      break;

    case IO_GET_KEY:
      if (!get_pressed_key(chip8, &result)) {
        log_msg = "no input pressed, instruction pointer decremented";
        chip8->sp -= 2;
      } else {
        asprintf(&log_msg, "received pressed key %x", result);
        // technically converts from char to uint8, but the keys go
        // from 0-16 so this isn't really an issue
        chip8->V[x] = result;
      }
      break;

    case IO_CHAR:
      // calculate font location of the specific character X in memory
      asprintf(&log_msg, "Changing index from %d to %d based on font character %x",
               chip8->I, FONT_START + x * FONT_HEIGHT, x);
      chip8->I = FONT_START + x * FONT_HEIGHT; 
      break;

    case IO_BIN_DEC:
      asprintf(&log_msg, "Setting memory locations %d, %d, %d to %d, %d, %d based on number %d",
               chip8->I, chip8->I + 1, chip8->I + 2, 
               chip8->V[x] / 100, (chip8->V[x] / 10) % 10, chip8->V[x] % 10, chip8->V[x]);

      // extract 3 decimal digits from a number and store them in memory
      chip8->memory[chip8->I] = chip8->V[x] / 100; // hundreds place
      chip8->memory[chip8->I + 1] = (chip8->V[x] / 10) % 10; // tens place
      chip8->memory[chip8->I + 2] = chip8->V[x] % 10; // ones place
      break;

    case IO_SMEM:
      // TODO - make toggle for incrementing I
      // Load all registers up to VX into memory starting at I
      for (int i = 0; i <= x; i++) {
        asprintf(&log_msg, "Filling memory location %d with value V[%d]: %d",
                 chip8->I + i, i, chip8->V[i]);
        log_message(log_msg, chip8);
        free(log_msg);

        chip8->memory[chip8->I + i] = chip8->V[i];
      }
      log_msg = NULL;
      break;

    case IO_LMEM:
      for (int i = 0; i <= x; i++) {
        asprintf(&log_msg, "Loading register V[%d] from memory location %d with value %d",
                 i, chip8->I + i, chip8->memory[chip8->I + i]);
        chip8->V[i] = chip8->memory[chip8->I + i];
        free(log_msg);
      }
      log_msg = NULL;
      break;
  }

  if (log_msg != NULL) {
    log_message(log_msg, chip8);
    free(log_msg);
  }
}

// Reset all pixels on a CHIP-8's screen to be blank
void clear_screen(struct Chip8 *const chip8) {
  
  log_message("clearing screen", chip8);

  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      chip8->screen[y * DISPLAY_WIDTH + x] = 0;
    }
  }
}

void exec_instruction(Chip8 *const chip8, uint16 instruction) {
  // OP (4 bits), x (4 bits), y (4 bits), n (4 bits)
  uint16 nnn = instruction & OP_NNN;
  uint8 nn = instruction & OP_NN;
  uint8 n = instruction & OP_N;
  uint8 x = (instruction & OP_X) >> 8;
  uint8 y = (instruction & OP_Y) >> 4;
  chip8->displaying = 0;

  switch (chip8->opcode) {
    case OP_SYS:
      if (instruction == OP_CLR_SCRN) {
        chip8->displaying = 1;
        clear_screen(chip8);
      }
      else if (instruction == OP_RET) {
        // NOTE - I don't think it's necessary to overwrite the stack value?
        chip8->pc = chip8->stack[chip8->sp];
        chip8->sp--;
      }
      break;
    case OP_JUMP:
      chip8->pc = nnn; 
      break;
    case OP_CALL:
      chip8->sp++;
      chip8->stack[chip8->sp] = chip8->pc;
      chip8->pc = nnn;
      break;
    case OP_BEQI:
      // skip 1 instruction if VX == NN
      if (chip8->V[x] == nn) {
        chip8->pc += 2;
      }
      break;
      // skip 1 instruction if VX != NN
    case OP_BNEI:
      if (chip8->V[x] != nn) {
        chip8->pc += 2;
      }
      break;
    case OP_BEQ:
      // skip 1 instruction if VX == VY
      if (chip8->V[x] == chip8->V[y]) {
        chip8->pc += 2;
      }
      break;
    case OP_BNE:
      // skip 1 instruction if VX != VY
      if (chip8->V[x] != chip8->V[y]) {
        chip8->pc += 2;
      }
      break;
    case OP_LI:
      chip8->V[x] = nn;
      break;
    case OP_ADDI:
      chip8->V[x] += nn; 
      break;
    case OP_ALU:
      exec_alu(chip8, x, y, n);
      break;
    case OP_SET_IDX:
      chip8->I = nnn;
      break;
    case OP_JO:
      // TODO: make bug configurable for compatability with SUPER-CHIP and CHIP-48 programs
      chip8->pc = nnn + chip8->V[0];
      break;
    case OP_RAND:
      // generate a random number, do a binary AND with NN, and load it into VX
      chip8->V[x]= nn & rand();
      break;
    case OP_DISPLAY:
      chip8->displaying = 1;
      exec_display(chip8, x, y, n);
      break;
    case OP_BKEY:
      // Skip 1 instruction if either "skip if pressed" or "skip if not pressed" are being used
      if ((nn == 0x9E && chip8->key[x]) || (nn == 0xA1 && !chip8->key[x])) {
        chip8->pc += 2;
      }
      break;
    case OP_IO:
      exec_io(chip8, x, nn);
      break;
  }
}

int exec_cycle(Chip8 *const chip8, struct View *const view) {
  // reset the play_sound flag
  int error = view_getInput(chip8->key, KEY_COUNT);
  if (error) {
    return error;
  }

  uint16 instruction = fetch_instruction(chip8);
  
  char* log_msg;
  asprintf(&log_msg, "fetched instruction %04x", instruction);
  log_message(log_msg, chip8);
  free(log_msg);

  exec_instruction(chip8, instruction);
  
  if (chip8->displaying) {
    view_draw(view, chip8->screen);
  }

  // Copy the flag to avoid doing anything which takes time while
  // the mutex lock is acquired.
  uint8 play_sound;
  error = SDL_LockMutex(chip8->timer_mutex);
  if (error) {
    fprintf(stderr, "error: Could not acquire timer mutex\n");
    return error;
  }
  play_sound = chip8->sound_flag;
  SDL_UnlockMutex(chip8->timer_mutex);
  
  if (play_sound) {
    // TODO - this doesn't work
    view_playSound();
    chip8->sound_flag = 0;
  }
  return 0;
}

Uint32 decrement_timers(Uint32 interval, void *params) {
  struct Chip8* chip8 = (struct Chip8*)params;
  int result = SDL_LockMutex(chip8->timer_mutex);
  
  // if there is an error, ignore timers and print an error message in the console
  if (result) {
    fprintf(stderr, "Error: unable to decrement timer\n");
    return interval;
  }
  if (chip8->sound_timer > 0) {
    chip8->sound_timer--;
    chip8->sound_flag = 1;
  }
  if (chip8->delay_timer > 0) {
    chip8->delay_timer--;
  }
  SDL_UnlockMutex(chip8->timer_mutex);

  return interval;
}

int exec_debug(Chip8 *const chip8, struct View *const view) {
  // Timers decrement every second, and the sound timer will update the chip8's
  // sound flag to indicate when a sound should be played
  SDL_TimerID timer = SDL_AddTimer(1000, decrement_timers, chip8);
  int manual = 1;
  while (chip8->pc < ADDRESS_COUNT) {
    exec_cycle(chip8, view);
    // calculate the timing to sleep in nanoseconds and wait that long before running the next instruction
    
    // using nanosleep because it can achieve a much better precision than SDL's delay function
    // (SDL2 has ~10ms precision, while nanosleep has around us precision), needs about ~1.3ms precision
    struct timespec sleep_val;
    sleep_val.tv_sec = 0;
    sleep_val.tv_nsec = (long)(1.0 / INSTRUCTION_FREQUENCY * 1000000000);
    nanosleep(&sleep_val, &sleep_val);

    int key_count;
    SDL_PumpEvents();
    const Uint8* keystate = SDL_GetKeyboardState(&key_count);
    while (!keystate[SDL_SCANCODE_N]) {
      SDL_PumpEvents();
      if (keystate[SDL_SCANCODE_ESCAPE]) {
        exit(0);
      }
      if (keystate[SDL_SCANCODE_RETURN]) {
        manual = !manual;
        SDL_Delay(500);
        break;
      }
      if (!manual) {
        break;
      }
    }
    if (manual) {
      SDL_Delay(1000);
    }
  }

  SDL_RemoveTimer(timer);

  fprintf(stderr, "Error: this program is not complete\n");
  return -1;
}

int exec_program(Chip8 *const chip8, struct View *const view) {
  // Timers decrement every second, and the sound timer will update the chip8's
  // sound flag to indicate when a sound should be played
  SDL_TimerID timer = SDL_AddTimer(1000, decrement_timers, chip8);

  int manual = 1; // flag for manually stepping through instructions in debug mode
  
  while (chip8->pc < ADDRESS_COUNT) {
    exec_cycle(chip8, view);
    // calculate the timing to sleep in nanoseconds and wait that long before running the next instruction
    
    // using nanosleep because it can achieve a much better precision than SDL's delay function
    // (SDL2 has ~10ms precision, while nanosleep has around us precision), needs about ~1.3ms precision
    struct timespec sleep_val;
    sleep_val.tv_sec = 0;
    sleep_val.tv_nsec = (long)(1.0 / INSTRUCTION_FREQUENCY * 1000000000);
    nanosleep(&sleep_val, &sleep_val); 

    // additional debug step for manually stepping through instructions
    if (chip8->config.debug) {
      int key_count;
      SDL_PumpEvents();
      const Uint8* keystate = SDL_GetKeyboardState(&key_count);
      while (!keystate[SDL_SCANCODE_N]) {
        SDL_PumpEvents();
        if (keystate[SDL_SCANCODE_RETURN]) {
          manual = !manual;
          SDL_Delay(500);
          break;
        }
        if (!manual) {
          break;
        }
      }
      if (manual) {
        SDL_Delay(1000);
      }
    }
  }

  SDL_RemoveTimer(timer);

  fprintf(stderr, "Error: this program is not complete\n");
  return -1;
}

