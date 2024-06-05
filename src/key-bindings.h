#ifndef KEYBINDINGS
#define KEYBINDINGS

#include <SDL2/SDL_scancode.h>

// This layout mimics the layout of the numpad present on the COSMIAC VIP
// on a 60%+ QWERTY keyboard. The original numpad looks like this:
//
// 1 2 3 C
// 4 5 6 D
// 7 8 9 E
// A 0 B F
//
// The 16 elements in this array represent the physical keybinding for the numbers
// 0-15 (0x0-0xF) on a CHIP-8 system
const SDL_Scancode BINDINGS[] = {
  SDL_SCANCODE_X, // 0
  SDL_SCANCODE_1, // 1
  SDL_SCANCODE_2, // 2
  SDL_SCANCODE_3, // 3
  SDL_SCANCODE_Q, // 4
  SDL_SCANCODE_W, // 5
  SDL_SCANCODE_E, // 6
  SDL_SCANCODE_A, // 7
  SDL_SCANCODE_S, // 8
  SDL_SCANCODE_D, // 9
  SDL_SCANCODE_Z, // A
  SDL_SCANCODE_C, // B
  SDL_SCANCODE_4, // C
  SDL_SCANCODE_R, // D
  SDL_SCANCODE_F, // E
  SDL_SCANCODE_V, // F
};

#define EXIT_SIZE 2
const SDL_Scancode EXIT_HOTKEY[] = {
  SDL_SCANCODE_LCTRL,
  SDL_SCANCODE_C
};

#endif
