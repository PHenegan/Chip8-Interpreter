#include "chip8.h"
#include "control.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <time.h>
#include <unistd.h>
#include "view.h"

static inline int old_shift(char* str) {
  // TODO - work out parsing for shorthand notation (-s)
  return strncmp(str, "--old-shift", 12) == 0;
}

static inline int jump_quirk(char* str) {
  // TODO - work out parsing for shorthand notation (-j)
  return strncmp(str, "--jump-quirk", 13) == 0;
}

static inline int old_indexing(char* str) {
  // TODO - work out parsing for shorthand notation (-i)
  return strncmp(str, "--old-index", 12) == 0;
}

static inline int debug(char* str) {
  // TODO - work out parsing for shorthand notation (-d)
  return strncmp(str, "--debug", 8) == 0;
}

void free_memory(Chip8* chip8, int flags) {
  chip8_destroy(chip8);
  SDL_QuitSubSystem(flags);
  SDL_Quit();
}

void help_menu() {
  printf("Usage: chip8 [...options] [rom-filepath]\n");
  printf("Options:\t\tDescription\n");
  printf("--debug\t\tRun the program in debug mode, stepping through instructions 1-by-1\n");
  printf("--old-shift\tIf enabled, copy VY into VX before doing bit shifts\n");
  printf("--jump-quirk\tIf enabled, use VX instead of V0 in 0xBNNN instruction\n");
  printf("--old-index\tIf enabled, increment index register when loading/storing memory\n");
}

int main(int argc, char* argv[]) {
  int sdl_flags = SDL_INIT_AUDIO | SDL_INIT_TIMER;
  SDL_Init(sdl_flags);
  
  // using calloc to make sure everything is 0-initialized
  Chip8 *chip8 = chip8_init();

  srand(time(NULL));

  char* filepath = NULL;
  for (int i = 1; i < argc; i++) {
    if (debug(argv[i])) {
      chip8->config.debug = 1;
      SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    } else if (old_shift(argv[i])) {
      chip8->config.legacy_shift = 1;
    } else if (jump_quirk(argv[i])) {
      chip8->config.jump_quirk = 1;
    }  else if (old_indexing(argv[i])) {
      chip8->config.legacy_indexing = 1;
    } else {
      filepath = argv[i];
    }
  }

  // try to load the file in
  if (filepath == NULL || load_program(chip8, filepath) == -1) {
    fprintf(stderr, "Unable to load program - error loading file");
    help_menu();
    free_memory(chip8, sdl_flags);
    exit(-1);
  }

  View *view = view_init(DISPLAY_WIDTH, DISPLAY_HEIGHT, 15, "CHIP-8 Interpreter");
  
  int result = exec_program(chip8, view);

  // A quit signal should be a successful result, that just indicates the user
  // closed the program
  result = result == QUIT_SIGNAL ? 0 : result;
 
  view_destroy(view);
  free_memory(chip8, sdl_flags);
  return result;
}
