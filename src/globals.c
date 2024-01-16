#include "globals.h"

char *buffer;
int cursor_position = 0;
int cursor_left_padding = 0;
int cursor_row = 0;

int screen_width = SCREEN_WIDTH;
int screen_height = SCREEN_HEIGHT;

enum Mode mode = MODE_NORMAL;

int Init_Globals(void) {
  buffer = malloc(sizeof(char));
  if (buffer == NULL) {
    return -1;
  }

  buffer[0] = '\0';

  return 0;
}

void Free_Globals(void) { free(buffer); }
