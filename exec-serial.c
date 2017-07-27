#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <fcntl.h>

// #include "color-control.c"
#include "stdin.c"


int main() {
  uint8_t* input;
  uint8_t inputLen  = read_stdin_serial(&input);
  uint8_t startByte = '\x01';
  uint8_t endByte = '\xff';

  int totalLen = inputLen + 3; //+3 for start len and end byte
  uint8_t* total = malloc(totalLen * sizeof(uint8_t)); // array to hold the result;

  memcpy(total, &startByte, sizeof(uint8_t));
  memcpy(total + 1, &totalLen, sizeof(uint8_t));
  memcpy(total + 2, input, inputLen * sizeof(uint8_t)); // + 2 for start and len
  memcpy(total + 2 + inputLen, &endByte, sizeof(uint8_t));

//   const uint8_t* message =  total;

  printf("\nRaw Input: (%i bytes)\n\n", inputLen);
  for (int i = 0; i < inputLen; i++)
  {
    printf("%02x ", input[i]);
    if ((i % 0x10) == 0xf) printf("\n");
  }

  printf("\n\nWrapped Code: (%i bytes)\n\n", totalLen);
  for (int i = 0; i < totalLen; i++)
  {
    printf("%02x ", total[i]);
    if ((i % 0x10) == 0xf) printf("\n");
  }
  printf("\n\n");

  // TODO: Make this an input arg or something
  const char* serial_path = "/dev/ttyUSB0";
  printf("Writing to %s...\n", serial_path);
  int fd = open(serial_path, 1);
  if (fd < 0) {
    printf("Problem opening serial port...\n");
    free(total);
    return fd;
  }
  int result = write(fd, (const char*)total, totalLen);

  printf("Total Written: (%i bytes)\n", result);
  free(total);
//   free_stdin(input);
}
