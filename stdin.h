#ifndef STDIN_H
#define STDIN_H

#include <stdint.h>

uint8_t* read_stdin(void);
uint8_t read_stdin_serial(uint8_t** buf);
void free_stdin(uint8_t* input);

#endif
