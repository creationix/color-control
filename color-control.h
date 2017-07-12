#include <stdbool.h>
#include <stdint.h>

typedef struct {
  // Callback provided by consumer.  This will be called every time the LEDs
  // need to change color
  void (*on_pwm)(uint8_t r, uint8_t g, uint8_t b);
  // Called to turn ethernet TX LED on or off
  void (*on_n1)(bool b);
  // Called ot turn ethernet RX LED on or off
  void (*on_n2)(bool b);
  // Callback provided by consumer.  This is called when the system wants to
  // pause for a given number of milliseconds.
  void (*delay)(uint32_t ms);
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
  ADD, SUB, MUL, DIV, MOD, NEG,

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
  IF, ELIF, ELSE, // ( if (cond, expr) else if (cond, expr) else (expr) )
  WHILE, // while (cond, expr)
  DO, END, // block of multiple expressions, evaluates to last one

  // Color Math
  HUE,  // (hue) => RGB
  RGB, // (ref, green, blue) => RGB
  MIX, // (rgb, rgb, split) => RGB - mix two rgb values

  // External I/O functions
  DELAY, PWM, N1, N2
} opcode_t;

// Macros to generating bytecode for literals in C
#define U7(num) (uint32_t)(num) & 0x7f
#define U8(num) UINT8, (uint32_t)(num) & 0xff
#define U16(num) UINT16, ((uint32_t)(num) >> 8) & 0xff, (uint32_t)(num) & 0xff
#define U24(num) UINT24, ((uint32_t)(num) >> 16) & 0xff, ((uint32_t)(num) >> 8) & 0xff, (uint32_t)(num) & 0xff
#define U32(num) UINT32, ((uint32_t)(num) >> 24), ((uint32_t)(num) >> 16) & 0xff, ((uint32_t)(num) >> 8) & 0xff, (uint32_t)(num) & 0xff

uint32_t eval(vm_t* vm);
