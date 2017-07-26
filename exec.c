#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "color-control.c"
#include "stdin.c"

static void on_pwm(uint8_t r, uint8_t g, uint8_t b) {
  printf("#%02x%02x%02x - \x1b[48;2;%d;%d;%dm    \x1b[0m\n", r, g, b, r, g, b);
}

static void on_pin(uint8_t pin, bool state) {
  printf("Turn pin-%d %s\n", pin, state ? "ON" : "OFF");
}

static void delay(uint32_t ms) {
  // printf("Delay %u\n", ms);
  struct timeval t = {
    .tv_sec = ms / 1000,
    .tv_usec = (ms % 1000) * 1000
  };
  select(0, NULL, NULL, NULL, &t);
}

static void on_error(uint32_t code, const char* msg) {
  fprintf(stderr, "Error (%d): %s\n", code, msg);
  exit(-code);
}

int main() {
  uint8_t* input = read_stdin();
  
  for (int i = 0; i < 145; i++)
  {
    printf("%#02x%s\n", input[i], "-");
  }
  
  vm_t vm = (vm_t){
    .on_pwm = on_pwm,
    .on_pin = on_pin,
    .on_error = on_error,
    .on_delay = delay,
    .vars = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .pc = input
  };
  printf("result=%u\n", eval(&vm));
  free_stdin(input);
}
