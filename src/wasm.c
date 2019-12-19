#include <emscripten.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "./libs/color-control.c"

extern void on_update(uint8_t* pixels);
extern void on_pin(uint8_t bank, uint8_t pin, bool state);
extern void on_error(uint32_t code, const char* msg);
extern void on_delay(uint32_t ms);

static uint8_t code[1024];
static vm_t vm = (vm_t){.on_update = on_update,
                        .on_pin = on_pin,
                        .on_error = on_error,
                        .on_delay = on_delay,
                        .vars = {0, 0, 0, 0, 0, 0, 0, 0},
                        .pc = code};

EMSCRIPTEN_KEEPALIVE
uint8_t* get_code() {
  return code;
}

EMSCRIPTEN_KEEPALIVE
uint32_t color_control(uint16_t len) {
  return eval(&vm);
}
