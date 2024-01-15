#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CHAR_HEIGHT 20
#define CHAR_WIDTH 12

#define FONT_PATH "assets/fonts/GoMonoNerdFont-Regular.ttf"

extern char *buffer;
extern int cursor_position;

extern int screen_width;
extern int screen_height;

enum Mode {
  MODE_NORMAL,
  MODE_INSERT,
};

extern enum Mode mode;

#endif /* GLOBALS_H_INCLUDED */
