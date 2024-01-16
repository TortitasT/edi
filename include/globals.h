#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CHAR_HEIGHT 20
#define CHAR_WIDTH 12

#define FONT_PATH "assets/fonts/GoMonoNerdFont-Regular.ttf"

extern char *buffer;
extern int cursor_position;
extern int cursor_left_padding;

extern int screen_width;
extern int screen_height;

enum Mode {
  MODE_NORMAL,
  MODE_INSERT,
};

extern enum Mode mode;

int Init_Globals(void);
void Free_Globals(void);

#endif /* GLOBALS_H_INCLUDED */
