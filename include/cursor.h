#ifndef CURSOR_H_INCLUDED
#define CURSOR_H_INCLUDED

#include "globals.h"
#include "macros.h"
#include "render.h"
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string.h>

void Move_Cursor(int new_position);

int Render_Cursor(SDL_Renderer *renderer, TTF_Font *font, char character, int x,
                  int y);

#endif /* CURSOR_H_INCLUDED */
