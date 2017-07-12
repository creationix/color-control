#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "color-control.c"

static void on_pwm(uint8_t r, uint8_t g, uint8_t b) {
  printf("#%02x%02x%02x - \x1b[48;2;%d;%d;%dm    \x1b[0m\n", r, g, b, r, g, b);
}

static void on_pin(uint8_t pin, bool state) {
  printf("Turn pin-%d %s\n", pin, state ? "ON" : "OFF");
}

static void delay(uint32_t ms) {
  printf("Delay %u\n", ms);
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
  vm_t vm = (vm_t){
    .on_pwm = on_pwm,
    .on_pin = on_pin,
    .on_error = on_error,
    .on_delay = delay,
    .vars = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .pc = (uint8_t[]) {
      DO,
        // Animate a rainbow
        // Repeat the loop getting brighter each time
        FOR1, 15, U8(255), 64,
          // cycle through the whole hue rainbow
          FOR0, 0, U16(0x5ff), 64, DO,
            // Generate a rainbow rgb value using the loop as hue, update LED
            // Mix in outer loop to dampen initially
            PWM,
              MIX,
                0,
                HUE, GET0,
                GET1,
            // Delay enough to get 60fps
            DELAY, 16,
          END,
        // Same thing, but fade to white instead of from black
        FOR1, 15, U8(255), 64,
        // cycle through the whole hue rainbow
          FOR0, 0, U16(0x5ff), 64, DO,
            // Generate a rainbow rgb value using the loop as hue, update LED
            // Mix in outer loop to dampen initially
            PWM,
              MIX,
                HUE, GET0,
                U24(0xffffff),
                GET1,
            // Delay enough to get 60fps
            DELAY, 16,
          END,
        // Blink the status leds in unision 3 times
        // and then alternating 3 times.
        SET1, 1, SET2, 1,
        FOR0, 1, 12, 1, DO,
          // Update the LEDs using variables
          PIN, 1, GET1, PIN, 2, GET2,
          // Delay a bit
          DELAY, U16(250),
          // Invert the variables
          SET1, NOT, GET1,
          SET2, NOT, GET2,
          // Halfway through, switch to alternating
          IF, EQ, GET0, 6,
            SET1, 0,
        END,

        // Then, just for fun, show this is a general purpose programming lang:
        // Count down with decreasing delays
        FOR0, U16(500), 0, 25,
          DELAY, GET0,

        // And sum the first 10000 integers for the final return value.
        FOR0, 1, U16(10000), 1,
          SET3,
            ADD, GET0, GET3,

      // Make sure to end our outer block
      END
    }
  };
  printf("result=%u\n", eval(&vm));
}
