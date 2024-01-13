#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include "cursor.h"
#include "globals.h"
#include "utils.h"
#include <SDL_render.h>
#include <SDL_ttf.h>

int Render_Text(SDL_Renderer *renderer, TTF_Font *font, const char *wanted_text,
                SDL_Color text_color, SDL_Color text_background_color, int x,
                int y);

int Render_Buffer(SDL_Renderer *renderer, TTF_Font *font,
                  const char *buffer_text);

int Render_Status_Bar(SDL_Renderer *renderer, TTF_Font *font);

#endif /* RENDER_H_INCLUDED */
