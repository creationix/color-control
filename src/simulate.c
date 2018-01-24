#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "libs/color-control.c"
#include "libs/reader.c"


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

static void run_program(reader_t* reader) {
  printf("Running program...\n");
  vm_t vm = (vm_t){
    .on_update = on_update,
    .on_pin = on_pin,
    .on_error = on_error,
    .on_delay = delay,
    .vars = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .pc = reader->body
  };
  printf("result=%u\n", eval(&vm));
}
#define CHUNK_SIZE 16
int main() {
  reader_t* reader = NULL;
  while (true) {
    uint8_t buf[CHUNK_SIZE];
    ssize_t bytes_read = read(0, buf, CHUNK_SIZE);
    if (bytes_read < 0) {
      fprintf(stderr, "Problem reading data from stdin: %s\n", strerror(errno));
      return -1;
    }
    if (!bytes_read) return -1;
    reader_result_t res = reader_push(&reader, buf, bytes_read);
    if (res == READER_NEED_MORE) continue;
    if (res == READER_DONE) {
      run_program(reader);
      reader_cleanup(&reader);
      return 0;
    }
    fprintf(stderr, "Ignoring invalid data: code %d\n", res);
    reader_cleanup(&reader);
  }
}
