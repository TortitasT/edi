#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>

#include "utils.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FONT_PATH "assets/fonts/GoMonoNerdFont-Regular.ttf"

char *buffer = "\0";

void Panic(int status, const char *format_message, ...) {
  va_list args;
  va_start(args, format_message);
  vfprintf(stderr, format_message, args);
  va_end(args);

  TTF_Quit();
  SDL_Quit();

  exit(status);
}

void Type_In_Buffer(char key) { String_Push_Char(&buffer, key); }

int Render_Paragraph(SDL_Renderer *renderer, TTF_Font *font,
                     const char *wantedText, SDL_Color textColor,
                     SDL_Color textBackgroundColor, int x, int y) {
  SDL_Texture *text = NULL;
  SDL_Rect textRect;

  SDL_Surface *textSurface =
      TTF_RenderText_Shaded(font, wantedText, textColor, textBackgroundColor);
  if (!textSurface) {
    Panic(1, "Unable to render text surface!\nSDL2_ttf Error: %s\n",
          TTF_GetError());
  }

  text = SDL_CreateTextureFromSurface(renderer, textSurface);
  if (!text) {
    Panic(1, "Unable to create texture from rendered text!\nSDL2 Error: %s\n",
          SDL_GetError());
  }

  textRect.w = textSurface->w;
  textRect.h = textSurface->h;

  SDL_FreeSurface(textSurface);

  textRect.x = x;
  textRect.y = y;

  SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
  SDL_RenderCopy(renderer, text, NULL, &textRect);

  return 1;
}

int Render_Text(SDL_Renderer *renderer, TTF_Font *font,
                const char *wantedText) {
  SDL_Color textColor = {0x00, 0x00, 0x00, 0xFF};
  SDL_Color textBackgroundColor = {0xFF, 0xFF, 0xFF, 0xFF};

  int line_number = 0;
  char *current_line = "\0";
  char current_character;

  for (int i = 0; i < strlen(wantedText) + 1; i++) {
    printf("%s\n", current_line);
    current_character = wantedText[i];

    if (current_character == '\n') {
      if (strlen(current_line) > 0) {
        Render_Paragraph(renderer, font, current_line, textColor,
                         textBackgroundColor, 0, line_number * 40);
      }

      current_line = "";
      line_number++;
    } else if (current_character != '\0') {
      String_Push_Char(&current_line, current_character);
    } else if (strlen(current_line) > 0) {
      Render_Paragraph(renderer, font, current_line, textColor,
                       textBackgroundColor, 0, line_number * 40);
    }
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

  TTF_Font *font = TTF_OpenFont(FONT_PATH, 40);
  if (!font) {
    Panic(1, "Unable to load font: '%s'!\nTTF_Error: %s\n", FONT_PATH,
          TTF_GetError());
  }

  SDL_StartTextInput();

  bool quit = false;

  while (!quit) {
    SDL_Event e;
    SDL_WaitEvent(&e);

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
      default: {
        break;
      }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    SDL_RenderClear(renderer);

    if (buffer[0] != '\0') {
      Render_Text(renderer, font, buffer);
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
