#ifndef VIEW
#define VIEW

struct View;

// Initialize a View renderer, returning it upon completion.
// NOTE: This function uses memory allocation. It is expected that `view_destroy` will be called
// when the program is finished in order to free that memory.
//
// `tiles_horiz`: the width of the CHIP-8 screen
// `tiles_vert`: the height of the CHIP-8 screen
// `tile_size`: the scaling factor between the CHIP-8 screen and the computer screen
// `title`: a title for the window being created
struct View* view_init(int tiles_horiz, int tiles_vert, int tile_size, const char *title);

// Render a CHIP-8's screen to a GUI window.
// `view`: the struct storing internal view information
// `screen`: a 2D grid representing the state (on or off) of each pixel
//           in the CHIP-8.
int view_draw(struct View *const view, unsigned char **const screen);

// TODO - implement and fix parameters
int view_playSound();

// Get input from the user, populating the provided character array
// with the states of the keys. The value of each key will be non-zero if the key is being pressed,
// and 0 if the key is not being pressed.
//
// The function will return 0 if it is successful, or 1 otherwise.
// 
// `keys`: an array indicating whether each key is currently being pressed
// `key_count`: the number of elements in `keys`.
int view_getInput(unsigned char* const keys, const int key_count);

// Deconstruct the view struct, freeing the resources used by the view and cleaning up
// and GUI library resources.
// `view`: the view to destroy
void view_destroy(struct View *view);

#endif
