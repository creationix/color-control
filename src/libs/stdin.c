#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "stdin.h"

#define chunk_size 256

uint8_t* stdin_read(size_t* size) {
  size_t max = chunk_size;
  size_t offset = 0;
  uint8_t* buffer = malloc(max);

  // Read stdin into a auto-growing buffer
  while (true) {
    ssize_t bytes_read = read(0, buffer + offset, max - offset);
    if (bytes_read < 0) return NULL;
    if (bytes_read == 0) break;
    offset += bytes_read;
    if (offset == max) {
      max = offset + chunk_size;
      buffer = realloc(buffer, max);
    }
  }

  // Optionally record size
  if (size) *size = offset;

  // Clean up freespace and add null terminator
  buffer = realloc(buffer, offset + 1);
  buffer[offset] = 0;

  return buffer;
}

uint8_t* stdin_resize(uint8_t* input, size_t new_size) {
  return realloc(input, new_size);
}

void stdin_free(uint8_t* input) {
  free(input);
}
