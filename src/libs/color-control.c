#include "color-control.h"
// #include <stdio.h>

static void skip(vm_t* vm);

static uint32_t eval_for(uint8_t var, vm_t* vm) {
  uint32_t i = eval(vm); // start
  uint32_t j = eval(vm); // end
  int32_t s = eval(vm);  // step
  uint8_t* pc = vm->pc;
  uint32_t ret = 0;
  if (i <= j) j -= (j - i) % s;
  else j += (i - j) % s, s = -s;
  while (true) {
    // printf("LOOP SET %d=%d\n", var, i);
    vm->vars[var] = i;
    vm->pc = pc;
    ret = eval(vm);
    if (i == j) return ret;
    i += s;
  }
}

static uint32_t eval_while(vm_t* vm) {
  uint8_t* cond = vm->pc;
  uint32_t ret = 0;
  while (vm->pc = cond, eval(vm)) {
    ret = eval(vm);
  }
  skip(vm);
  return ret;
}

static uint32_t eval_if(vm_t* vm) {
  if (eval(vm)) return eval(vm);
  skip(vm);
  return 0;
}

static uint32_t eval_do(vm_t* vm) {
  int32_t ret = 0;
  while (*vm->pc != END) ret = eval(vm);
  vm->pc++;
  return ret;
}

static void skip_do(vm_t* vm) {
  while (*vm->pc != END) skip(vm);
  vm->pc++;
}

static uint32_t eval_hue(vm_t* vm) {
  // h is an integer from 0 to 1535, it has 6 color segments, each 256 wide.
  // Output is a 24-bit RGB value
  uint32_t h = eval(vm);
  h = h % 1536;
  uint32_t r, g, b;
  // Red to Yellow
  (h < 0x100) ? (r = 0xff, g = h, b = 0) :
  // Yellow to Green
  (h < 0x200) ? (r = 0x1ff - h, g = 0xff, b = 0) :
  // Green to Cyan
  (h < 0x300) ? (r = 0, g = 0xff, b = h - 0x200) :
  // Cyan to Blue
  (h < 0x400) ? (r = 0, g = 0x3ff - h, b = 0xff) :
  // Blue to Magenta
  (h < 0x500) ? (r = h - 0x400, g = 0, b = 0xff) :
  // Magenta to Red
                (r = 0xff, g = 0, b = 0x5ff - h);
  // printf("h=%d rgb=%02x %02x %02x\n", h, r, g, b);
  return (r << 16) | (g << 8) | b;
}

static uint32_t eval_rgb(vm_t* vm) {
  // Input is 3 expressions for Red, Green, and Blue from 0-255 each.
  // Output is a 24-bit RGB value
  return ((eval(vm) & 0xff) << 16) |
         ((eval(vm) & 0xff) << 8) |
          (eval(vm) & 0xff);
}

static uint32_t eval_mix(vm_t* vm) {
  uint32_t rgb1 = eval(vm);
  uint32_t rgb2 = eval(vm);
  // y is mix between values
  // 0 is all left
  // 255 is all right
  uint32_t y = (eval(vm) & 0xff) + 1;
  uint32_t x = 256 - y;
  return (((((rgb1 >> 16) & 0xff) * x + ((rgb2 >> 16) & 0xff) * y) >> 8) << 16) |
         (((((rgb1 >> 8) & 0xff) * x + ((rgb2 >> 8) & 0xff) * y) >> 8) << 8) |
          (((rgb1 & 0xff) * x + (rgb2 & 0xff) * y) >> 8);
}

static uint32_t deadbeef_seed;
static uint32_t deadbeef_beef = 0xdeadbeef;

static uint32_t eval_srand(vm_t* vm) {
	deadbeef_seed = eval(vm);
	deadbeef_beef = 0xdeadbeef;
  return deadbeef_seed;
}

static uint32_t eval_rand(vm_t* vm) {
  uint32_t range = eval(vm);
	deadbeef_seed = (deadbeef_seed << 7) ^ ((deadbeef_seed >> 25) + deadbeef_beef);
	deadbeef_beef = (deadbeef_beef << 7) ^ ((deadbeef_beef >> 25) + 0xdeadbeef);
	return deadbeef_seed % range;
}

static uint32_t eval_delay(vm_t* vm) {
  uint32_t ms = eval(vm);
  vm->on_delay(ms);
  return ms;
}
static uint32_t eval_update(vm_t* vm) {
  vm->on_update(vm->pixels);
  return 0;
}
static uint32_t eval_pin(vm_t* vm, uint8_t bank) {
  uint8_t pin = eval(vm);
  bool state = eval(vm);
  vm->on_pin(bank, pin, state);
  return state;
}

static uint32_t eval_fill(vm_t* vm) {
  uint32_t rgb = eval(vm);
  for (int i = 0; i < LED_COUNT * LED_BPP; i += LED_BPP) {
    vm->pixels[i] = (rgb >> 16) & 0xff;
    vm->pixels[i + 1] = (rgb >> 8) & 0xff;
    vm->pixels[i + 2] = (rgb >> 0) & 0xff;
  }
  return rgb;
}

static uint32_t eval_fade(vm_t* vm) {
  uint32_t rgb = eval(vm);
  // y is mix between values
  // 0 is all old value
  // 255 is all new value
  uint32_t y = (eval(vm) & 0xff) + 1;
  uint32_t x = 256 - y + 1;
  for (int i = 0; i < LED_COUNT * LED_BPP; i += LED_BPP) {
    vm->pixels[i + 0] = (vm->pixels[i + 0] * x + ((rgb >> 16) & 0xff) * y) >> 8;
    vm->pixels[i + 1] = (vm->pixels[i + 1] * x + ((rgb >>  8) & 0xff) * y) >> 8;
    vm->pixels[i + 2] = (vm->pixels[i + 2] * x + ((rgb >>  0) & 0xff) * y) >> 8;
  }
  return rgb;
}

static uint32_t eval_write(vm_t* vm) {
  uint32_t index = eval(vm);
  uint32_t rgb = eval(vm);
  if (index < LED_COUNT) {
    int i = index * LED_BPP;
    vm->pixels[i + 0] = (rgb >> 16) & 0xff;
    vm->pixels[i + 1] = (rgb >>  8) & 0xff;
    vm->pixels[i + 2] = (rgb >>  0) & 0xff;
  }
  return rgb;
}

uint32_t eval(vm_t* vm) {
  opcode_t op = *(vm->pc)++;
  // printf("%02x - %08x %08x %08x %08x %08x %08x %08x %08x\n", op,
  //   vm->vars[0], vm->vars[1], vm->vars[2], vm->vars[3],
  //   vm->vars[4], vm->vars[5], vm->vars[6], vm->vars[7]);
  // If the opcode is less than 128, it's a literal unsigned integer
  if (op < 128) return op;

  // Everything else needs to be switched based on it's code.
  switch (op) {
    // Handle the larger number literals (these consume 2-5 bytes)
    case UINT8:  return (vm->pc) += 1,  *(vm->pc - 1);
    case UINT16: return (vm->pc) += 2, (*(vm->pc - 2) << 8)  |  *(vm->pc - 1);
    case UINT24: return (vm->pc) += 3, (*(vm->pc - 3) << 16) | (*(vm->pc - 2) << 8)  |  *(vm->pc - 1);
    case UINT32: return (vm->pc) += 4, (*(vm->pc - 4) << 24) | (*(vm->pc - 3) << 16) | (*(vm->pc - 2) << 8) | *(vm->pc - 1);
    // Handle Arithmetic Operations
    case ADD: return eval(vm) + eval(vm);
    case SUB: return eval(vm) - eval(vm);
    case MUL: return eval(vm) * eval(vm);
    case DIV: return eval(vm) / eval(vm);
    case MOD: return eval(vm) % eval(vm);
    // Handle Comparision Operations
    case GT: return eval(vm) > eval(vm);
    case LT: return eval(vm) < eval(vm);
    case GTE: return eval(vm) >= eval(vm);
    case LTE: return eval(vm) >= eval(vm);
    case EQ: return eval(vm) == eval(vm);
    case NEQ: return eval(vm) != eval(vm);
    // Handle Logical Operations
    case AND: return eval(vm) && eval(vm);
    case OR: return eval(vm) || eval(vm);
    case NOT: return !eval(vm);

    // Handle variable reads/writes
    case GET0: case GET1: case GET2: case GET3:
    case GET4: case GET5: case GET6: case GET7:
      // printf("GET=%d\n", op - GET0);
      return vm->vars[op - GET0];
    case SET0: case SET1: case SET2: case SET3:
    case SET4: case SET5: case SET6: case SET7:
      // printf("SET=%d\n", op - SET0);
      return vm->vars[op - SET0] = eval(vm);
    // Handle variable loops
    case FOR0: case FOR1: case FOR2: case FOR3:
    case FOR4: case FOR5: case FOR6: case FOR7:
      return eval_for(op - FOR0, vm);

    // Handle other control-flow operations
    case WHILE: return eval_while(vm);
    case IF: return eval_if(vm);
    case DO: return eval_do(vm);

    case HUE: return eval_hue(vm);
    case RGB: return eval_rgb(vm);
    case MIX: return eval_mix(vm);
    case RAND: return eval_rand(vm);
    case SRAND: return eval_srand(vm);

    case WRITE: return eval_write(vm);
    case FILL: return eval_fill(vm);
    case FADE: return eval_fade(vm);

    case DELAY: return eval_delay(vm);
    case UPDATE: return eval_update(vm);
    case PINA: case PINB: return eval_pin(vm, op - PINA);

    case END:
      vm->on_error(op, "Illegal opcode");
  }
  vm->on_error(op, "Unknown opcode");
  return -1;
}

static void skip(vm_t* vm) {
  opcode_t op = *(vm->pc)++;
  // If the opcode is less than 128, it's a literal unsigned integer
  if (op < 128) return;
  // Everything else needs to be switched based on it's code.
  switch (op) {
    // Handle the larger number literals (these consume 2-5 bytes)
    case UINT8:   vm->pc += 1; return;
    case UINT16:  vm->pc += 2; return;
    case UINT24:  vm->pc += 3; return;
    case UINT32:  vm->pc += 4; return;

    // Operations that don't consume any expressions
    case UPDATE:
    case GET0: case GET1: case GET2: case GET3:
    case GET4: case GET5: case GET6: case GET7:
      return;

    // Operators that consume one expression
    case SET0: case SET1: case SET2: case SET3:
    case SET4: case SET5: case SET6: case SET7:
    case NOT:
    case DELAY:
    case HUE: case RAND: case SRAND:
    case FILL:
      return skip(vm);

    // Operators that consume two expressions
    case ADD: case SUB: case MUL: case DIV: case MOD:
    case LT: case GT: case LTE: case GTE: case EQ: case NEQ:
    case AND: case OR: case WHILE: case IF:
    case PINA: case PINB: case WRITE: case FADE:
      return skip(vm), skip(vm);

    // Consume 3
    case RGB: case MIX:
      return skip(vm), skip(vm), skip(vm);

    // Operators that consume 4 expressions
    case FOR0: case FOR1: case FOR2: case FOR3:
    case FOR4: case FOR5: case FOR6: case FOR7:
      return skip(vm), skip(vm), skip(vm), skip(vm);

    case DO: return skip_do(vm);

    case END:
      vm->on_error(op, "Illegal opcode");
  }
  vm->on_error(op, "Unknown opcode");
}
