#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>
#include "view.h"
#include "key-bindings.h"

struct View {
  struct SDL_Window* window;
  struct SDL_Renderer* renderer;
  int tile_size;
  int tiles_width;
  int tiles_height;
  SDL_AudioSpec sound;
  int sample_count;
  bool playing_sound;
};

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
  Sint16 *buffer = (Sint16*)raw_buffer;
  int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
  int *sample_count = (int*)user_data; 

  for(int i = 0; i < length; i++, (*sample_count)++) {
    double time = (double)*sample_count / (double)SAMPLE_RATE;
    buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 440.0f * time)); // render 441 HZ sine wave
  }
}

View* view_init(int tiles_horiz, int tiles_vert, int tile_size, const char *title) {
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
  view->playing_sound = false;
  
  // setup the data for SDL audio to play
  int sample_count = 0;
  view->sound.freq = SAMPLE_RATE;
  view->sound.format = AUDIO_S16SYS;
  view->sound.channels = 1;
  view->sound.samples = 2048;
  view->sound.callback = audio_callback;
  view->sound.userdata = &view->sample_count;
  SDL_OpenAudio(&view->sound, NULL);

  return view;
}

int view_set_sound(View *const view, bool enable) {
  if (view->playing_sound && !enable) {
    SDL_PauseAudio(1);
  }
  if (!view->playing_sound && enable) {
    // Set the sound information for the beeping noise
    view->sample_count = 0;
    SDL_PauseAudio(0);
  }
  view->playing_sound = enable;
  return 0;
}

int view_get_input(unsigned char* const keys, const int key_count) {
  SDL_PumpEvents();
  const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
  
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT) {
    return 1; 
  }

  // Loop through keys and update them based on the state of
  // the keys on the keyboard
  // see key-bindings.h for CHIP-8 keybindings
  for (int i = 0; i < key_count; i++) {
    keys[i] = keyboard_state[BINDINGS[i]];
  }

  bool quit = QUIT_SIGNAL;
  for (int i = 0; i < EXIT_SIZE; i++) {
    quit = quit && keyboard_state[EXIT_HOTKEY[i]];
  }
  if (quit) {
    printf("escape key pressed, exiting...\n");
  }

  return quit;
}

int view_draw(View *const view, unsigned char *const screen) {
  // set color to black and clear the screen
  SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255);
  SDL_RenderClear(view->renderer);
  // set color to white for drawing the pixels

  for (int row = 0; row < view->tiles_height; row++) {
    for (int col = 0; col < view->tiles_width; col++) {
      // if the pixel in the screen is on, render a white square in the correct location
      if (screen[row * view->tiles_width + col]) {
        SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
        SDL_Rect rect;
        rect.x = col * view->tile_size;
        rect.y = row * view->tile_size;
        rect.w = view->tile_size;
        rect.h = view->tile_size;
        SDL_RenderFillRect(view->renderer, &rect);
      }
      else {
        // I added a kind of dot here to create a grid-effect, not strictly necessary but
        // it helped me confirm my rendering logic is correct
        SDL_SetRenderDrawColor(view->renderer, 50, 50, 50, 255);
        SDL_Rect rect;
        rect.x = col * view->tile_size;
        rect.y = row * view->tile_size;
        rect.w = (int)view->tile_size * 0.1;
        rect.h = (int)view->tile_size * 0.1;
        SDL_RenderFillRect(view->renderer, &rect);
      }
    }
  }
  SDL_RenderPresent(view->renderer);
  return 0;
}

void view_destroy(View *view) {
  SDL_DestroyRenderer(view->renderer);
  SDL_DestroyWindow(view->window);
  SDL_CloseAudio();
  free(view);
}
