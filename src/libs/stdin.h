#ifndef STDIN_H
#define STDIN_H

#include <stdint.h>
#include <unistd.h>

uint8_t* stdin_read(size_t* size);
uint8_t* stdin_resize(uint8_t* input, size_t new_size);
void stdin_free(uint8_t* input);

#endif
