#include <string.h>
#include <stdlib.h>

#include "reader.h"

reader_result_t reader_push(reader_t** ptr, uint8_t* data, uint16_t len) {
  reader_t* reader = *ptr;
  if (!reader) {
    if (len < 4) {
      // The first chunk didn't have at least 4 bytes, we don't know the length!
      return READER_START_TOO_SMALL;
    }
    if (data[0] != '(' || data[1] != ']') {
      // The required start bytes were missing, we should probably ignore this.
      return READER_MISSING_START_BYTES;
    }
    uint16_t size = ((data[2] >> 8) | data[3]) - 6;
    reader = *ptr = malloc(sizeof(reader_t) + size);
    reader->size = size;
    reader->offset = 0;
    data += 4;
    len -= 4;
  }
  uint16_t bytes_left = reader->size - reader->offset;
  if (len >= bytes_left) {
    memcpy(reader->body + reader->offset, data, bytes_left);
    if (len > bytes_left + 1) {
      if (data[bytes_left] != '[' || data[bytes_left + 1] != ')') {
        return READER_MISSING_END_BYTES;
      }
    }
    // TODO: verify trailing bytes when they are broken to a new packet?
    return READER_DONE;
  }
  memcpy(reader->body + reader->offset, data, len);
  reader->offset += len;
  return READER_NEED_MORE;
}

void reader_cleanup(reader_t** ptr) {
  if (!ptr) return;
  reader_t* reader = *ptr;
  free(reader);
  *ptr = NULL;
}
