#include <SDL_timer.h>
#include <stdlib.h>

#include "utils.h"

#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CHAR_HEIGHT 20
#define CHAR_WIDTH 12

#define FONT_PATH "assets/fonts/GoMonoNerdFont-Regular.ttf"

char *buffer = "\0";
int cursor_position = 0;

void Panic(int status, const char *format_message, ...) {
  va_list args;
  va_start(args, format_message);
  vfprintf(stderr, format_message, args);
  va_end(args);

  TTF_Quit();
  SDL_Quit();

  exit(status);
}

void move_cursor(int direction) {
  int len = strlen(buffer);
  int new_cursor_position = 0;

  if (direction > 0) {
    new_cursor_position = MAX(0, cursor_position - 1);
  } else {
    new_cursor_position = MIN(len, cursor_position + 1);
  }

  if (buffer[new_cursor_position] == '\n') {
    // TODO: Not working
    move_cursor(direction);
    return;
  }

  if (buffer[new_cursor_position] == '\0') {
    return;
  }

  cursor_position = new_cursor_position;
}

void Type_In_Buffer(char key) { String_Push_Char(&buffer, key); }

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

  return 1;
}

int Render_Cursor(SDL_Renderer *renderer, TTF_Font *font, char character, int x,
                  int y) {
  SDL_Color color = {0x00, 0x00, 0x00, 0xFF};
  SDL_Color background_color = {0xFF, 0xFF, 0xFF, 0xFF};

  // Every 500ms, the cursor will blink
  if (SDL_GetTicks64() % 1000 < 500) {
    background_color = (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF};
  } else {
    background_color = (SDL_Color){0x00, 0x00, 0x00, 0xFF};
  }

  char *character_string = malloc(2);
  character_string[0] = character;
  character_string[1] = '\0';

  Render_Text(renderer, font, character_string, color, background_color, x, y);

  return 1;
}

int Render_Buffer(SDL_Renderer *renderer, TTF_Font *font,
                  const char *buffer_text) {
  SDL_Color text_color = {0x00, 0x00, 0x00, 0xFF};
  SDL_Color text_background_color = {0xFF, 0xFF, 0xFF, 0xFF};

  int line_number = 0;
  int line_column = 0;
  char *current_line = "\0";
  char current_character;

  for (int i = 0; (unsigned)i < strlen(buffer_text) + 1; i++) {
    current_character = buffer_text[i];
    int is_cursor = i == cursor_position;

    if (is_cursor) {
      if (strlen(current_line) > 0) {
        Render_Text(renderer, font, current_line, text_color,
                    text_background_color,
                    (line_column - strlen(current_line)) * CHAR_WIDTH,
                    line_number * CHAR_HEIGHT);
      }

      Render_Cursor(renderer, font, current_character, line_column * CHAR_WIDTH,
                    line_number * CHAR_HEIGHT);

      if (buffer_text[i + 1] != '\n') {
        current_line = "";
        line_column++;
      }

      continue;
    }

    if (current_character == '\0' && strlen(current_line) > 0) {
      Render_Text(renderer, font, current_line, text_color,
                  text_background_color,
                  (line_column - strlen(current_line)) * CHAR_WIDTH,
                  line_number * CHAR_HEIGHT);
      continue;
    }

    if (current_character == '\n' && strlen(current_line) > 0) {
      Render_Text(renderer, font, current_line, text_color,
                  text_background_color,
                  (line_column - strlen(current_line)) * CHAR_WIDTH,
                  line_number * CHAR_HEIGHT);
      current_line = "";
      line_number++;
      line_column = 0;
      continue;
    }

    String_Push_Char(&current_line, current_character);
    line_column++;

    // if (current_character == '\n') {
    //   if (strlen(current_line) > 0) {
    //     Render_Text(renderer, font, current_line, text_color,
    //                 text_background_color,
    //                 (line_column - strlen(current_line)) * CHAR_HEIGHT,
    //                 line_number * CHAR_HEIGHT);
    //   }
    //
    //   current_line = "";
    //   line_number++;
    //   line_column = 0;
    // } else if (current_character != '\0') {
    //   String_Push_Char(&current_line, current_character);
    //   line_column++;
    // } else if (strlen(current_line) > 0) {
    //   Render_Text(renderer, font, current_line, text_color,
    //               text_background_color,
    //               (line_column - strlen(current_line)) * CHAR_HEIGHT,
    //               line_number * CHAR_HEIGHT);
    // }
  }

  return 1;
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    Panic(1, "SDL2 could not be initialized!\nSDL2 Error: %s\n",
          SDL_GetError());
  }

  TTF_Init();

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
  if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) {
    Panic(0, "SDL can not disable compositor bypass!\n");
  }
#endif

  SDL_Window *window =
      SDL_CreateWindow("edi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    Panic(1, "Window could not be created!\nSDL_Error: %s\n", SDL_GetError());
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    Panic(1, "Renderer could not be created!\nSDL_Error: %s\n", SDL_GetError());
  }

  TTF_Font *font = TTF_OpenFont(FONT_PATH, CHAR_HEIGHT);
  if (!font) {
    Panic(1, "Unable to load font: '%s'!\nTTF_Error: %s\n", FONT_PATH,
          TTF_GetError());
  }

  SDL_StartTextInput();

  bool quit = false;

  while (!quit) {
    SDL_Event e;

    // If we use SDL_WaitEvent, the program will wait for an event to happen and
    // won't blink the cursor
    //
    // SDL_WaitEvent(&e);
    SDL_PollEvent(&e);

    SDL_KeyboardEvent *key = NULL;

    switch (e.type) {
    case SDL_KEYDOWN:
      key = &e.key;
      break;

    case SDL_KEYUP:
      break;

    case SDL_TEXTINPUT:
      Type_In_Buffer(e.text.text[0]);
      break;

    case SDL_QUIT:
      quit = true;
      break;

    default:
      break;
    }

    if (key) {
      switch (key->keysym.sym) {
      case SDLK_ESCAPE:
        quit = true;
        break;
      case SDLK_BACKSPACE: {
        int len = strlen(buffer);
        if (len > 0) {
          char *new_buffer = malloc(len);
          strncpy(new_buffer, buffer, len - 1);
          new_buffer[len - 1] = '\0';
          buffer = new_buffer;
        }
        break;
      }
      case SDLK_RETURN: {
        Type_In_Buffer('\n');
        break;
      }
      case SDLK_LEFT: {
        // cursor_position = MAX(0, cursor_position - 1);
        move_cursor(1);
        break;
      }
      case SDLK_RIGHT: {
        // cursor_position = MIN((int)strlen(buffer) - 1, cursor_position + 1);
        move_cursor(-1);
        break;
      }
      default: {
        break;
      }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    SDL_RenderClear(renderer);

    if (buffer[0] != '\0') {
      Render_Buffer(renderer, font, buffer);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_CloseFont(font);

  TTF_Quit();
  SDL_Quit();

  return 0;
}
