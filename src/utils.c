#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void String_Push_Char(char **str, char concat) {
  //                                             1 for the \0 and 1 for the new
  //                                             char
  char *new_str = (char *)malloc((strlen(*str) + 2) * sizeof(char));

  strcpy(new_str, *str);

  new_str[strlen(*str)] = concat;
  new_str[strlen(*str) + 1] = '\0';

  *str = new_str;
}
