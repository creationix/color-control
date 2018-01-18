#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <fcntl.h>

#include "libs/stdin.c"

int main(int argc, const char** argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s /dev/ttyACM0\n", argv[0]);
    return -1;
  }

  size_t input_len;
  uint8_t* input = stdin_read(&input_len);

  fprintf(stderr, "\nRaw Input: (%lu bytes)\n\n", input_len);
  for (size_t i = 0; i < input_len; i++) {
    fprintf(stderr, "%02x ", input[i]);
    if ((i % 0x10) == 0xf) fprintf(stderr, "\n");
  }

  size_t total_len = input_len + 3;

  // Resize to make room for framing
  input = stdin_resize(input, total_len);

  // First move the data in place
  memmove(input + 2, input, input_len);

  // Then write the start/end markers and the size header
  input[0] = '\x01'; // start byte
  input[1] = (uint8_t)input_len; // length byte
  input[2 + input_len] = '\xff'; // end byte

  fprintf(stderr, "\n\nWrapped Code: (%lu bytes)\n\n", total_len);
  for (size_t i = 0; i < total_len; i++) {
    fprintf(stderr, "%02x ", input[i]);
    if ((i % 0x10) == 0xf) fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n\n");

  const char* serial_path = argv[1];
  fprintf(stderr, "Writing to %s...\n", serial_path);
  int fd = open(serial_path, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    fprintf(stderr, "Problem opening serial port '%s': %s\n", serial_path, strerror(-fd));
    close(fd);
    stdin_free(input);
    return fd;
  }
  int result = write(fd, (const char*)input, total_len);

  fprintf(stderr, "Total Written: (%i bytes)\n", result);
  close(fd);
  stdin_free(input);
}
