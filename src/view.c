#include <SDL2/SDL.h>
#include <stdlib.h>
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include "view.h"

struct View {
  struct SDL_Window* window;
  struct SDL_Renderer* renderer;
  int tile_size;
  int tiles_width;
  int tiles_height;
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
  view->tile_size = tile_size;
  view->tiles_width = tiles_horiz;
  view->tiles_height = tiles_vert;

  return view;
}

int view_playSound() {
  // TODO - implement
  return -1;
}

int view_getInput() {
  // TODO - implement
  return -1;
}

int view_draw(struct View *const view, unsigned char **screen) {
  // set color to black and clear the screen
  SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255);
  SDL_RenderClear(view->renderer);
  // set color to white for drawing the pixels
  SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);


  for (int row = 0; row < view->tiles_height; row++) {
    for (int col = 0; col < view->tiles_width; col++) {
      // if the pixel in the screen is on, render a white square in the correct location
      if (screen[row][col]) {
        SDL_Rect rect;
        rect.x = col * view->tile_size;
        rect.y = row * view->tile_size;
        rect.w = view->tile_size;
        rect.h = view->tile_size;
        SDL_RenderFillRect(view->renderer, &rect);
      }
    }
  }
  SDL_RenderPresent(view->renderer);
  return 0;
}

void view_destroy(struct View *view) {
  SDL_DestroyWindow(view->window);
  SDL_DestroyRenderer(view->renderer);
}
