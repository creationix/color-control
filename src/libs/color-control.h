#ifndef COLOR_CONTROL_H
#define COLOR_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

#ifndef LED_COUNT
#define LED_COUNT 8
#endif

#ifndef LED_BPP
#define LED_BPP 3
#endif

typedef struct {
  // Callback provided by consumer.  This will be called every time the LEDs
  // need to change color
  void (*on_update)(uint8_t* pixels);
  // Called to turn a pin on of off
  void (*on_pin)(uint8_t bank, uint8_t pin, bool state);
  // Callback for runtime exceptions.
  void (*on_error)(uint32_t code, const char* msg);
  // Callback provided by consumer.  This is called when the system wants to
  // pause for a given number of milliseconds.
  void (*on_delay)(uint32_t ms);
  // RGB Pixel data
  uint8_t pixels[LED_COUNT * LED_BPP];
  // 8 global variables
  uint32_t vars[8];
  // Program Counter
  uint8_t* pc;
} vm_t;


typedef enum {
  // Literals
  // Values less than 128 are UINT7 literals
  UINT8 = 128, // Consume an extra byte as value
  UINT16,      // Consume two extra bytes as value
  UINT24,      // Consume three extra bytes as value
  UINT32,      // Consume four extra bytes as value

  // Arithmetic
  ADD, SUB, MUL, DIV, MOD,

  // Comparison
  GT, LT, GTE, LTE, EQ, NEQ,

  // Logical operators; zero is false, non-zero is true
  AND, OR, NOT,

  // variables
  GET0, GET1, GET2, GET3, GET4, GET5, GET6, GET7,
  SET0, SET1, SET2, SET3, SET4, SET5, SET6, SET7,
  // for variable (start, end, skip, expr)
  FOR0, FOR1, FOR2, FOR3, FOR4, FOR5, FOR6, FOR7,
  // other control flow
  WHILE, // while (cond, expr)
  IF, // if(conf, expr)
  DO, END, // block of multiple expressions, evaluates to last one

  // Color Math
  HUE,  // (hue) => RGB
  RGB, // (ref, green, blue) => RGB
  MIX, // (rgb, rgb, split) => RGB - mix two rgb values
  SRAND, // Seed the RNG with uint32 value
  RAND, // Give a random uint32 value

  WRITE, // (index, RGB)
  FILL, // (RGB) - write all pixels same color
  FADE, // (RGB, split) - mix all pixels with given color

  // External I/O functions
  DELAY, UPDATE, PINA, PINB
} opcode_t;

// Macros to generating bytecode for literals in C
#define U7(num) (uint32_t)(num) & 0x7f
#define U8(num) UINT8, (uint32_t)(num) & 0xff
#define U16(num) UINT16, ((uint32_t)(num) >> 8) & 0xff, (uint32_t)(num) & 0xff
#define U24(num) UINT24, ((uint32_t)(num) >> 16) & 0xff, ((uint32_t)(num) >> 8) & 0xff, (uint32_t)(num) & 0xff
#define U32(num) UINT32, ((uint32_t)(num) >> 24), ((uint32_t)(num) >> 16) & 0xff, ((uint32_t)(num) >> 8) & 0xff, (uint32_t)(num) & 0xff

uint32_t eval(vm_t* vm);

#endif
