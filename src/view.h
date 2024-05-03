#ifndef VIEW
#define VIEW

struct View;

struct View* view_init(int tiles_horiz, int tiles_vert, int tile_size, const char *title);

int view_draw(struct View *const view, unsigned char **screen);

void view_destroy(struct View *view);

#endif
