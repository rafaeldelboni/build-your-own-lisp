#include "lib.h"
#include <stdio.h>

int main() {
  char * print_string = get_hello_str();
  printf("%s", print_string);
  return 0;
}
