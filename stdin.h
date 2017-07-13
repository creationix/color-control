#ifndef STDIN_H
#define STDIN_H

#include <stdint.h>

uint8_t* read_stdin(void);
void free_stdin(uint8_t* input);

#endif
