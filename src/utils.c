#include "utils.h"

void Panic(int status, const char *format_message, ...) {
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

  // strcpy(new_str, *str);
  for (int i = 0; i < (int)strlen(*str); i++) {
    new_str[i] = (*str)[i];
  }

  new_str[strlen(*str)] = concat;
  new_str[strlen(*str) + 1] = '\0';

  *str = new_str;
}

void String_Pop_Char(char **str, int position) {
  char *new_str = (char *)malloc((strlen(*str)) * sizeof(char));

  int i = 0;
  int j = 0;

  while ((*str)[i] != '\0') {
    if (i != position) {
      new_str[j] = (*str)[i];
      j++;
    }
    i++;
  }

  new_str[j] = '\0';

  *str = new_str;
}
