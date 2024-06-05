#include "chip8.h"
#include "control.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <time.h>
#include <unistd.h>
#include "view.h"

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER);
  // using calloc to make sure everything is 0-initialized
  struct Chip8 *chip8 = calloc(1, sizeof(struct Chip8));

  struct View *view = view_init(DISPLAY_WIDTH, DISPLAY_HEIGHT, 15, "CHIP-8 Interpreter");

  srand(time(NULL));
  // ensure a file was passed in
  if (argc < 2) {
    fprintf(stderr, "Unable to load program - no file passed in");
    exit(-1);
  }

  initialize_system(chip8);

  // try to load the file in
  if (load_program(chip8, argv[1]) == -1) {
    fprintf(stderr, "Unable to load program - error loading file");
    exit(-1);
  }

  int result = exec_program(chip8, view, 0);
  
  free(chip8);
  return result;
}
