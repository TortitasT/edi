#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Panic(int status, const char *format_message, ...);
void String_Push_Char(char **str, char concat);
void String_Pop_Char(char **str, int position);

#endif // UTILS_H_INCLUDED
