#include <string.h>
#include <stdlib.h>

#include "reader.h"

reader_result_t reader_push(reader_t* reader, uint8_t* data, uint16_t len) {

  // Read the header in an interruptible way if we don't have it all yet.
  while (!reader->body) {
    if (!len--) return READER_NEED_MORE;
    uint8_t byte = *data++;
    uint8_t offset = reader->offset++;
    if (offset == 0) {
      if (byte == '(') continue;
    }
    if (offset == 1) {
      if (byte == ']') continue;
    }
    if (offset == 2) {
      reader->size = byte << 8;
      continue;
    }
    if (offset == 3) {
      reader->size = (reader->size | byte) - 6;
      reader->offset = 0;
      reader->body = malloc(reader->size);
      if (reader->size) break;
    }
    return READER_INVALID_START_BYTES;
  }

  uint16_t bytes_left = reader->size - reader->offset;

  if (len >= bytes_left) {
    memcpy(reader->body + reader->offset, data, bytes_left);
    if (len > bytes_left + 1) {
      if (data[bytes_left] != '[' || data[bytes_left + 1] != ')') {
        return READER_INVALID_END_BYTES;
      }
    }
    // TODO: verify trailing bytes when they are broken to a new packet?
    return READER_DONE;
  }

  memcpy(reader->body + reader->offset, data, len);
  reader->offset += len;
  return READER_NEED_MORE;
}

void reader_reset(reader_t* reader) {
  free(reader->body);
  reader->body = NULL;
  reader->size = 0;
  reader->offset = 0;
}
