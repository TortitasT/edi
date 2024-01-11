#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FONT_PATH "assets/fonts/GoMonoNerdFont-Regular.ttf"

char *buffer = "a";

int shutdown() {
  TTF_Quit();
  SDL_Quit();
  return 0;
}

int render_text(SDL_Renderer *renderer, TTF_Font *font, const char *wantedText,
                SDL_Rect *returnTextRect, SDL_Texture **returnTextTexture) {
  SDL_Color textColor = {0x00, 0x00, 0x00, 0xFF};
  SDL_Color textBackgroundColor = {0xFF, 0xFF, 0xFF, 0xFF};
  SDL_Texture *text = NULL;
  SDL_Rect textRect;

  SDL_Surface *textSurface =
      TTF_RenderText_Shaded(font, wantedText, textColor, textBackgroundColor);
  if (!textSurface) {
    printf("Unable to render text surface!\n"
           "SDL2_ttf Error: %s\n",
           TTF_GetError());
    return 0;
  }

  text = SDL_CreateTextureFromSurface(renderer, textSurface);
  if (!text) {
    printf("Unable to create texture from rendered text!\n"
           "SDL2 Error: %s\n",
           SDL_GetError());
    return 0;
  }

  // Get text dimensions
  textRect.w = textSurface->w;
  textRect.h = textSurface->h;

  SDL_FreeSurface(textSurface);

  textRect.x = (SCREEN_WIDTH - textRect.w) / 2;
  textRect.y = (SCREEN_HEIGHT - textRect.h) / 2;

  *returnTextRect = textRect;
  *returnTextTexture = text;

  return 1;
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL2 could not be initialized!\n"
           "SDL2 Error: %s\n",
           SDL_GetError());
    return 0;
  }

  TTF_Init();

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
  if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) {
    printf("SDL can not disable compositor bypass!\n");
    return 0;
  }
#endif

  SDL_Window *window =
      SDL_CreateWindow("edi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Window could not be created!\n"
           "SDL_Error: %s\n",
           SDL_GetError());
    return shutdown();
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("Renderer could not be created!\n"
           "SDL_Error: %s\n",
           SDL_GetError());
    return shutdown();
  }

  TTF_Font *font = TTF_OpenFont(FONT_PATH, 40);
  if (!font) {
    printf("Unable to load font: '%s'!\n"
           "SDL2_ttf Error: %s\n",
           FONT_PATH, TTF_GetError());
    return 0;
  }

  // Event loop exit flag
  bool quit = false;

  // Event loop
  while (!quit) {
    SDL_Event e;
    SDL_WaitEvent(&e);

    SDL_KeyboardEvent *key = NULL;

    switch (e.type) {
    case SDL_KEYDOWN:
      key = &e.key;
      break;

    case SDL_KEYUP:
      printf("Key release detected\n");
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
      default:
        if (key->keysym.sym == SDLK_BACKSPACE) {
          // with malloc
          int len = strlen(buffer);
          if (len > 0) {
            char *new_buffer = malloc(len);
            strncpy(new_buffer, buffer, len - 1);
            new_buffer[len - 1] = '\0';
            buffer = new_buffer;
          }
        } else {
          const char *key_string = SDL_GetKeyName(key->keysym.sym);
          int len = strlen(buffer);
          char *new_buffer = malloc(len + 1);
          strncpy(new_buffer, buffer, len);
          new_buffer[len] = key_string[0];
          new_buffer[len + 1] = '\0';
          buffer = new_buffer;
        }
        break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    SDL_RenderClear(renderer);

    // if (key) {
    //   if (key->keysym.sym == SDLK_BACKSPACE) {
    //     int len = strlen(buffer);
    //     if (len > 0) {
    //       buffer[len - 1] = '\0';
    //     }
    //   } else {
    //     const char *key_string = SDL_GetKeyName(key->keysym.sym);
    //     int len = strlen(buffer);
    //     buffer[len] = key_string[0];
    //     buffer[len + 1] = '\0';
    //   }
    // }
    if (buffer[0] != '\0') {
      SDL_Rect textRect;
      SDL_Texture *text;
      render_text(renderer, font, buffer, &textRect, &text);

      SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
      SDL_RenderCopy(renderer, text, NULL, &textRect);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return shutdown();
}
