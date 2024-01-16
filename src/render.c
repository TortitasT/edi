#include "render.h"
#include "globals.h"
#include "utils.h"
#include <string.h>

int Render_Text(SDL_Renderer *renderer, TTF_Font *font, const char *wanted_text,
                SDL_Color text_color, SDL_Color text_background_color, int x,
                int y) {
  SDL_Texture *text = NULL;
  SDL_Rect text_rect;

  SDL_Surface *text_surface = TTF_RenderText_Shaded(
      font, wanted_text, text_color, text_background_color);
  if (!text_surface) {
    Panic(1, "Unable to render text surface!\nSDL2_ttf Error: %s\n",
          TTF_GetError());
  }

  text = SDL_CreateTextureFromSurface(renderer, text_surface);
  if (!text) {
    Panic(1, "Unable to create texture from rendered text!\nSDL2 Error: %s\n",
          SDL_GetError());
  }

  text_rect.w = text_surface->w;
  text_rect.h = text_surface->h;

  SDL_FreeSurface(text_surface);

  text_rect.x = x;
  text_rect.y = y;

  SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
  SDL_RenderCopy(renderer, text, NULL, &text_rect);

  SDL_DestroyTexture(text);
  return 1;
}

int Render_Buffer(SDL_Renderer *renderer, TTF_Font *font) {
  SDL_Color text_color = {0x00, 0x00, 0x00, 0xFF};
  SDL_Color text_background_color = {0xFF, 0xFF, 0xFF, 0xFF};

  int max_lines = (screen_height / CHAR_HEIGHT) - 3;
  int left_padding = CHAR_WIDTH;

  int line_number = 0;
  int line_column = 0;

  char *current_line = malloc(sizeof(char));
  current_line[0] = '\0';

  char current_character;
  int is_cursor = 0;

  int start_position = 0;
  int desired_line = MAX(0, cursor_row - max_lines + 8);

  for (int i = 0; i < (int)strlen(buffer); i++) {
    if (buffer[i] == '\n') {
      desired_line--;
    }

    if (desired_line == 0) {
      start_position = i + 1;
      break;
    }
  }

  for (int i = start_position; i < (int)strlen(buffer) + 1; i++) {
    current_character = buffer[i];
    is_cursor = i == cursor_position;

    if (line_number >= max_lines) {
      break;
    }

    // If we are at the cursor position, render the current line and the cursor
    // and advance the column
    if (is_cursor) {
      if (strlen(current_line) > 0) {
        Render_Text(
            renderer, font, current_line, text_color, text_background_color,
            (line_column - strlen(current_line)) * CHAR_WIDTH + left_padding,
            (line_number + 1) * CHAR_HEIGHT);
      }

      Render_Cursor(renderer, font, current_character,
                    line_column * CHAR_WIDTH + left_padding,
                    (line_number + 1) * CHAR_HEIGHT);
      current_line = realloc(current_line, sizeof(char));
      current_line[0] = '\0';
      line_column++;

      // If we are at a newline advance the
      // line number and reset the column
      if (current_character == '\n') {
        line_number++;
        line_column = 0;
      }
      continue;
    }

    // If we are at the end of the buffer, render the current/rest of the line
    if (current_character == '\0' && strlen(current_line) > 0) {
      Render_Text(
          renderer, font, current_line, text_color, text_background_color,
          (line_column - strlen(current_line)) * CHAR_WIDTH + left_padding,
          (line_number + 1) * CHAR_HEIGHT);
      continue;
    }

    // If we are at the end of the line, render the current line and advance the
    // line number and reset the column
    if (current_character == '\n' && strlen(current_line) > 0) {
      Render_Text(
          renderer, font, current_line, text_color, text_background_color,
          (line_column - strlen(current_line)) * CHAR_WIDTH + left_padding,
          (line_number + 1) * CHAR_HEIGHT);
      current_line = realloc(current_line, sizeof(char));
      current_line[0] = '\0';
      line_number++;
      line_column = 0;
      continue;
    }

    // If we encounter a newline, advance the line number and reset the column
    if (current_character == '\n') {
      current_line = realloc(current_line, sizeof(char));
      current_line[0] = '\0';
      line_number++;
      line_column = 0;
      continue;
    }

    // If we are in the middle of the buffer, concatenate the character to the
    // current line and advance the column
    String_Push_Char(&current_line, current_character);
    line_column++;
  }

  free(current_line);
  return 1;
}

int Render_Status_Bar(SDL_Renderer *renderer, TTF_Font *font) {
  SDL_Color text_color = {0x00, 0x00, 0x00, 0xFF};
  SDL_Color text_background_color = {0xFF, 0xFF, 0xFF, 0xFF};

  switch (mode) {
  case MODE_INSERT:
    text_color = (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF};
    text_background_color = (SDL_Color){0x00, 0x00, 0x00, 0xFF};
    break;
  case MODE_NORMAL:
    text_color = (SDL_Color){0x00, 0x00, 0x00, 0xFF};
    text_background_color = (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF};
    break;
  }

  char *status_bar_text = malloc(100);
  sprintf(status_bar_text, "-- %s --",
          mode == MODE_NORMAL ? "NORMAL" : "INSERT");

  Render_Text(renderer, font, status_bar_text, text_color,
              text_background_color, CHAR_WIDTH,
              screen_height - (CHAR_HEIGHT * 2));

  free(status_bar_text);
  return 1;
}
