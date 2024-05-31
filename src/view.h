#ifndef VIEW
#define VIEW

struct View;

struct View* view_init(int tiles_horiz, int tiles_vert, int tile_size, const char *title);

int view_draw(struct View *const view, unsigned char **screen);

// TODO - implement and fix parameters
int view_playSound();

// TODO - implement and fix parameters
int view_getInput();

void view_destroy(struct View *view);

#endif
