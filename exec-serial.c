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
  
  printf("%i\n", inputLen);
  
  int totalLen = inputLen + 3; //+3 for start len and end byte
  uint8_t* total = malloc(totalLen * sizeof(uint8_t)); // array to hold the result; 

  memcpy(total, &startByte, sizeof(uint8_t));
  memcpy(total + 1, &totalLen, sizeof(uint8_t)); 
  memcpy(total + 2, input, inputLen * sizeof(uint8_t)); // + 2 for start and len
  memcpy(total + 2 + inputLen, &endByte, sizeof(uint8_t));

//   const uint8_t* message =  total;
  
  for (int i = 0; i < inputLen; i++)
  {
    printf("%#02x%s\n", input[i], "-");
  }
  
  printf("\n\n%s\n\n", "-------------");
  
  for (int i = 0; i < totalLen; i++)
  {
    printf("%#02x%s\n", total[i],"-");
  }
  
  int fd = open("/dev/ttyACM0", 1); //TODO: Make this an input arg or something
  int result = write(fd, (const char*)total, totalLen);
  
  printf("result=%u\n", result);
//   free(total);
//   free_stdin(input);
}
