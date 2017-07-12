#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

uint8_t* read_stdin() {
  size_t total = 256;
  size_t count = 0;
  uint8_t* input = malloc(total);
  ssize_t size;
  while ((size = read(0, input, total - count))) {
    if (size < 0) exit(-1);
    count += size;
    if (count == total) {
      input = realloc(input, (total += 256));
    }
  }
  if (!count) {
    free(input);
    return NULL;
  }
  if (count < total) {
    total = count;
    input = realloc(input, total);
  }
  return input;
}

void free_stdin(char* input) {
  free(input);
}
