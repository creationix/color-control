#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "stdin.h"

uint8_t* read_stdin() {
  size_t total = 256;
  size_t count = 0;
  uint8_t* input = malloc(total);
  ssize_t size;
  while ((size = read(0, input + count, total - count))) {
    if (size < 0) exit(-1);
    count += size;
    if (count == total) {
      input = realloc(input, (total += 256));
    }
  }
  total = count + 1;
  input = realloc(input, total);
  input[count] = 0;
  return input;
}

void free_stdin(uint8_t* input) {
  free(input);
}
