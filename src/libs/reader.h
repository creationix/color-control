#ifndef READER_H
#define READER_H

#include <stdint.h>

typedef struct {
  uint16_t size; // Size of code payload without framing data
  uint16_t offset; // write offset
  uint8_t body[]; // raw program data without framing data
} reader_t;

typedef enum {
  READER_DONE,
  READER_NEED_MORE,
  READER_START_TOO_SMALL,
  READER_MISSING_START_BYTES,
  READER_MISSING_END_BYTES
} reader_result_t;

reader_result_t reader_push(reader_t** ptr, uint8_t* data, uint16_t len);
void reader_cleanup(reader_t** ptr);

#endif
