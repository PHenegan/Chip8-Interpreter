#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
  // using calloc to make sure everything is 0-initialized
  struct Chip8 *chip8 = calloc(1, sizeof(struct Chip8));
  srand(time(NULL));
  // ensure a file was passed in
  if (argc < 2) {
    fprintf(stderr, "Unable to load program - no file passed in");
    exit(-1);
  }

  // try to load the file in
  if (load_program(chip8, argv[1]) == -1) {
    fprintf(stderr, "Unable to load program - error loading file");
    exit(-1);
  }

  load_font(chip8);
  
  int result = exec_program(chip8);
  
  free(chip8);
  return result;
}
