#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FONT_PATH "assets/fonts/GoMonoNerdFont-Regular.ttf"

char *buffer = "\0";

void panic(int status, const char *format_message, ...) {
  va_list args;
  va_start(args, format_message);
  vfprintf(stderr, format_message, args);
  va_end(args);

  TTF_Quit();
  SDL_Quit();

  exit(status);
}

void String_Push_Char(char **str, char concat) {
  //                                             1 for the \0 and 1 for the new
  //                                             char
  char *new_str = (char *)malloc((strlen(*str) + 2) * sizeof(char));

  strcpy(new_str, *str);

  new_str[strlen(*str)] = concat;
  new_str[strlen(*str) + 1] = '\0';

  *str = new_str;
}

void type_in_buffer(char key) {
  // int len = strlen(buffer);
  // char *new_buffer = malloc(len + 1);
  // strncpy(new_buffer, buffer, len);
  // new_buffer[len] = key;
  // new_buffer[len + 1] = '\0';
  // buffer = new_buffer;

  String_Push_Char(&buffer, key);
}

int subrender_text(SDL_Renderer *renderer, TTF_Font *font,
                   const char *wantedText, SDL_Color textColor,
                   SDL_Color textBackgroundColor, int x, int y) {
  SDL_Texture *text = NULL;
  SDL_Rect textRect;

  SDL_Surface *textSurface =
      TTF_RenderText_Shaded(font, wantedText, textColor, textBackgroundColor);
  if (!textSurface) {
    panic(1, "Unable to render text surface!\nSDL2_ttf Error: %s\n",
          TTF_GetError());
  }

  text = SDL_CreateTextureFromSurface(renderer, textSurface);
  if (!text) {
    panic(1, "Unable to create texture from rendered text!\nSDL2 Error: %s\n",
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

int render_text(SDL_Renderer *renderer, TTF_Font *font,
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
        subrender_text(renderer, font, current_line, textColor,
                       textBackgroundColor, 0, line_number * 40);
      }

      current_line = "";
      line_number++;
    } else if (current_character != '\0') {
      String_Push_Char(&current_line, current_character);
    } else if (strlen(current_line) > 0) {
      subrender_text(renderer, font, current_line, textColor,
                     textBackgroundColor, 0, line_number * 40);
    }
  }

  // char *line = malloc(strlen(wantedText));
  // int line_number = 0;
  //
  // for (int i = 0; i < strlen(wantedText); i++) {
  //   printf("%s\n", line);
  //
  //   if (wantedText[i] == '\0') {
  //     subrender_text(renderer, font, line, textColor, textBackgroundColor, 0,
  //                    line_number * 40);
  //   } else {
  //     printf("adding %c\n", wantedText[i]);
  //     // int len = strlen(line);
  //     // char *new_line = malloc(len + 1);
  //     // strncpy(new_line, line, len);
  //     // new_line[len] = wantedText[i];
  //     // new_line[len + 1] = '\0';
  //     // line = new_line;
  //   }
  //
  //   // if (wantedText[i] != '\n' && wantedText[i] != '\0') {
  //   //   int len = strlen(line);
  //   //   char *new_line = malloc(len + 1);
  //   //   strncpy(new_line, line, len);
  //   //   new_line[len] = wantedText[i];
  //   //   new_line[len + 1] = '\0';
  //   //   line = new_line;
  //   // } else if (wantedText[i] == '\n') {
  //   //   line = malloc(strlen(wantedText));
  //   //   line_number++;
  //   //
  //   //   subrender_text(renderer, font, line, textColor, textBackgroundColor,
  //   0,
  //   //                  line_number * 40);
  //   // } else if (wantedText[i] == '\0') {
  //   //   subrender_text(renderer, font, line, textColor, textBackgroundColor,
  //   0,
  //   //                  line_number * 40);
  //   // }
  // }
  //
  // free(line);
  return 1;
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    panic(1, "SDL2 could not be initialized!\nSDL2 Error: %s\n",
          SDL_GetError());
  }

  TTF_Init();

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
  if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) {
    panic(0, "SDL can not disable compositor bypass!\n");
  }
#endif

  SDL_Window *window =
      SDL_CreateWindow("edi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    panic(1, "Window could not be created!\nSDL_Error: %s\n", SDL_GetError());
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    panic(1, "Renderer could not be created!\nSDL_Error: %s\n", SDL_GetError());
  }

  TTF_Font *font = TTF_OpenFont(FONT_PATH, 40);
  if (!font) {
    panic(1, "Unable to load font: '%s'!\nTTF_Error: %s\n", FONT_PATH,
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
      type_in_buffer(e.text.text[0]);
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
        type_in_buffer('\n');
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
      render_text(renderer, font, buffer);
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
