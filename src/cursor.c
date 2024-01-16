#include "cursor.h"
#include "globals.h"

void Move_Cursor(int new_position) {
  int len = strlen(buffer) - 1;
  int new_cursor_position = 0;

  new_cursor_position = MAX(0, MIN(len + 1, new_position));

  for (int i = 0; i < new_cursor_position; i++) {
    if (buffer[i] == '\n' || i == 0) {
      cursor_left_padding = new_cursor_position - i;
    }
  }

  cursor_position = new_cursor_position;
}

int Render_Cursor(SDL_Renderer *renderer, TTF_Font *font, char character, int x,
                  int y) {
  SDL_Color color = {0x00, 0x00, 0x00, 0xFF};
  SDL_Color background_color = {0xFF, 0xFF, 0xFF, 0xFF};

  if (character == '\0' || character == '\n') {
    character = ' ';
  }

  // Every 500ms, the cursor will blink
  if (SDL_GetTicks64() % 1000 < 500) {
    background_color = (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF};
  } else {
    background_color = (SDL_Color){0x00, 0x00, 0x00, 0xFF};
    color = (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF};
  }

  char *character_string = malloc(2);
  character_string[0] = character;
  character_string[1] = '\0';

  Render_Text(renderer, font, character_string, color, background_color, x, y);

  free(character_string);
  return 1;
}
