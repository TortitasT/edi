#include <SDL_events.h>
#include <SDL_timer.h>
#include <stdlib.h>

#include "cursor.h"
#include "globals.h"
#include "macros.h"
#include "render.h"
#include "utils.h"

#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

void Type_In_Buffer(char key) {
  for (int i = 0; i < (int)strlen(buffer) + 1; i++) {
    if (i == cursor_position) {
      char *new_buffer = (char *)malloc((i + 1) * sizeof(char));
      for (int j = 0; j < i; j++) {
        new_buffer[j] = buffer[j];
      }
      new_buffer[i] = '\0';

      String_Push_Char(&new_buffer, key);
      for (int j = i; j < (int)strlen(buffer); j++) {
        String_Push_Char(&new_buffer, buffer[j]);
      }

      buffer = new_buffer;
    }
  }

  Move_Cursor(cursor_position + 1);
}

void Handle_Key(SDL_KeyboardEvent *key) {
  switch (key->keysym.sym) {
  case SDLK_ESCAPE:
    mode = MODE_NORMAL;
    break;
  case SDLK_i: {
    if (mode == MODE_NORMAL) {
      mode = MODE_INSERT;
    }
    break;
  }
  case SDLK_BACKSPACE: {
    if (mode == MODE_INSERT) {
      String_Pop_Char(&buffer, cursor_position - 1);
      Move_Cursor(cursor_position - 1);
    }
    break;
  }
  case SDLK_RETURN: {
    if (mode == MODE_INSERT) {
      Type_In_Buffer('\n');
    }
    break;
  }

  case SDLK_h:
  case SDLK_LEFT: {
    if (mode == MODE_INSERT && key->keysym.sym == SDLK_h) {
      break;
    }

    Move_Cursor(cursor_position - 1);
    break;
  }
  case SDLK_l:
  case SDLK_RIGHT: {
    if (mode == MODE_INSERT && key->keysym.sym == SDLK_l) {
      break;
    }

    Move_Cursor(cursor_position + 1);
    break;
  }
  case SDLK_k:
  case SDLK_UP: {
    if (mode == MODE_INSERT && key->keysym.sym == SDLK_k) {
      break;
    }

    int last_newline_position = 0;
    int last_line_length = 0;
    int steps_since_last_newline = 0;
    for (int i = 0; i < (int)strlen(buffer); i++) {
      if (buffer[i] == '\n') {
        last_newline_position = i;
        last_line_length = steps_since_last_newline;
        steps_since_last_newline = 0;
        continue;
      }

      steps_since_last_newline++;

      if (i == cursor_position) {
        if (last_line_length - steps_since_last_newline < 0) {
          Move_Cursor(last_newline_position - 1);
          break;
        }

        Move_Cursor(last_newline_position - last_line_length +
                    steps_since_last_newline - 1);
        break;
      }
    }
    break;
  }
  case SDLK_j:
  case SDLK_DOWN: {
    if (mode == MODE_INSERT && key->keysym.sym == SDLK_j) {
      break;
    }

    int steps_since_last_newline = 0;
    int line_cursor_position = 0;
    for (int i = 0; i < (int)strlen(buffer); i++) {
      if (buffer[i] == '\n') {
        if (i > cursor_position) {
          Move_Cursor(i + line_cursor_position + 1);
          break;
        }

        steps_since_last_newline = 0;
        continue;
      }

      if (i == cursor_position) {
        line_cursor_position = steps_since_last_newline;
      }

      steps_since_last_newline++;
    }
    break;
  }
  default: {
    break;
  }
  }
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
    // SDL_WaitEvent(&e);

    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_TEXTINPUT: {
        if (mode == MODE_INSERT) {
          Type_In_Buffer(e.text.text[0]);
        }
        break;
      }

      case SDL_KEYDOWN:
        break;

      case SDL_KEYUP:
        Handle_Key(&e.key);
        break;

      case SDL_QUIT:
        quit = true;
        break;

      default:
        break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    if (buffer[0] != '\0') {
      Render_Buffer(renderer, font, buffer);
    }

    Render_Status_Bar(renderer, font);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_CloseFont(font);

  TTF_Quit();
  SDL_Quit();

  return 0;
}
