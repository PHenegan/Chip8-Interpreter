#include <SDL2/SDL.h>
#include <stdlib.h>
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include "view.h"

struct View {
  struct SDL_Window* window;
  struct SDL_Renderer* renderer;
};

struct View* view_init(int tiles_horiz, int tiles_vert, int tile_size, const char *title) {
  struct View *view = malloc(sizeof(struct View));

  // calculate dimensions using the count and size of tiles, which are way smaller
  int width = tiles_horiz * tile_size;
  int height = tiles_vert * tile_size;

  view->window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    width, height,
    SDL_WINDOW_SHOWN
  );
  view->renderer = SDL_CreateRenderer(view->window, -1, 0);

  return view;
}

int view_draw(struct View *const view, unsigned char **screen) {
  
  return -1;
}

void view_destroy(struct View *view) {
  SDL_DestroyWindow(view->window);
  SDL_DestroyRenderer(view->renderer);
}
