#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "color-control.c"
#include "stdin.c"

static void on_update(uint8_t* pixels) {
  for (int i = 0; i < LED_COUNT * LED_BPP; i += LED_BPP) {
    printf("\x1b[48;2;%d;%d;%dm   ",
      pixels[i],
      pixels[i + 1],
      pixels[i + 2]);
  }
  printf("\x1b[0m\n");
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
  vm_t vm = (vm_t){
    .on_update = on_update,
    .on_pin = on_pin,
    .on_error = on_error,
    .on_delay = delay,
    .vars = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .pc = input
  };
  printf("result=%u\n", eval(&vm));
  free_stdin(input);
}
