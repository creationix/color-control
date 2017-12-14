#include <assert.h>
#include "mpc.c"
#include "stdin.c"
#include "color-control.h"

static const char* names[8];

static void generate_block(mpc_ast_t* ast);
static void generate_statement(mpc_ast_t* ast);

static uint8_t intern_ident(const char* ident) {
  for (int i = 0; i < 8; i++) {
    if (!names[i]) {
      names[i] = ident;
      // fprintf(stderr, "Found %s at %d\n", ident, i);
      return i;
    }
    if (!strcmp(names[i], ident)) {
      // fprintf(stderr, "Added %s at %d\n", ident, i);
      return i;
    }
  }
  return -1;
}

static bool ends_with(const char *str, const char *suffix) {
  if (!str || !suffix) return false;
  size_t lenstr = strlen(str);
  size_t lensuffix = strlen(suffix);
  if (lensuffix > lenstr) return false;
  return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

static bool is(mpc_ast_t* ast, const char* tag) {
  return ends_with(ast->tag, tag);
}

static void generate_number(uint32_t num) {
  // fprintf(stderr, "Generate Num %u - %08x\n", num, num);
  if (num < 0x80) {
    putchar((char)num);
  }
  else if (num < 0x100) {
    putchar(UINT8);
    putchar(num);
  }
  else if (num < 0x10000) {
    putchar(UINT16);
    putchar(num >> 8);
    putchar(num);
  }
  else if (num < 0x1000000) {
    putchar(UINT24);
    putchar(num >> 16);
    putchar(num >> 8);
    putchar(num);
  }
  else {
    putchar(UINT32);
    putchar(num >> 24);
    putchar(num >> 16);
    putchar(num >> 8);
    putchar(num);
  }
}


uint32_t parse_number(mpc_ast_t* ast) {
  if (is(ast, "number|decimal|regex")) {
    return atoi(ast->contents);
  }
  if (is(ast, "number|boolean|string")) {
    return !strcmp(ast->contents, "true");
  }
  if (is(ast, "number|hex|regex")) {
    return strtol(ast->contents + 2, NULL, 16);
  }
  assert(false); // Unknown tag for number
}

static void generate_assign(mpc_ast_t* ast) {
  uint8_t id = intern_ident(ast->children[0]->contents);
  putchar(SET0 + id);
  generate_statement(ast->children[2]);
}

static void generate_for(mpc_ast_t* ast) {

  // Emit the FOR for the proper loop variable
  uint8_t id = intern_ident(ast->children[1]->contents);
  putchar(FOR0 + id);

  // Emit start and stop ranges
  uint32_t num = parse_number(ast->children[3]);
  mpc_ast_t* range = mpc_ast_get_child(ast, "range|>");
  if (range) {
    generate_number(num);
    uint32_t end = parse_number(range->children[1]);
    if (!strcmp(range->children[0]->contents, "...")) end--;
    generate_number(end);
  }
  else {
    generate_number(0);
    generate_number(num - 1);
  }

  // Emit iteration step count
  mpc_ast_t* by = mpc_ast_get_child(ast, "by|>");
  generate_number(by ? parse_number(by->children[1]) : 1);

  return generate_block(mpc_ast_get_child(ast, "block|>"));
}

typedef struct {
  const char* name;
  int nargs;
  uint8_t opcode;
} fn_def_t;

static fn_def_t fns[] = {
  {"hue", 1, HUE},
  {"rgb", 3, RGB},
  {"mix", 3, MIX},
  {"srand", 1, SRAND},
  {"rand", 1, RAND},
  {"delay", 1, DELAY},
  {"fill", 1, FILL},
  {"fade", 2, FADE},
  {"write", 2, WRITE},
  {"update", 0, UPDATE},
  {"pin", 2, PIN},
  {NULL}
};

static int match_fn(const char* name) {
  uint8_t i = 0;
  while (fns[i].name) {
    if (!strcmp(fns[i].name, name)) return i;
    i++;
  }
  fprintf(stderr, "No such function %s\n", name);
  exit(-1);
}

static void generate_call(mpc_ast_t* ast) {
  // Loop up the function being called
  fn_def_t fn = fns[match_fn(ast->children[0]->contents)];
  uint8_t nargs = (ast->children_num - 2) / 2;
  if (nargs != fn.nargs) {
    fprintf(stderr, "%s requires %d args, but you supplied %d\n", fn.name, fn.nargs, nargs);
    exit(-1);
  }
  // Emit the initial opcode
  putchar(fn.opcode);

  for (int i = 0; i < nargs; i++) {
    generate_statement(ast->children[i * 2 + 2]);
  }

}

static void generate_get(mpc_ast_t* ast) {
  putchar(GET0 + intern_ident(ast->contents));
}

static void generate_sum(mpc_ast_t* ast) {
  switch (ast->children[1]->contents[0]) {
    case '-': putchar(SUB); break;
    case '+': putchar(ADD); break;
    default: assert(false);
  }
  generate_statement(ast->children[0]);
  generate_statement(ast->children[2]);
}

static void generate_product(mpc_ast_t* ast) {
  switch (ast->children[1]->contents[0]) {
    case '*': putchar(MUL); break;
    case '/': putchar(DIV); break;
    case '%': putchar(MOD); break;
    default: assert(false);
  }
  generate_statement(ast->children[0]);
  generate_statement(ast->children[2]);
}

static void generate_equality(mpc_ast_t* ast) {
  const char* op = ast->children[1]->contents;
  if (!strcmp("<", op)) putchar(LT);
  else if (!strcmp(">", op)) putchar(GT);
  else if (!strcmp("<=", op)) putchar(LTE);
  else if (!strcmp(">=", op)) putchar(GTE);
  else if (!strcmp("==", op)) putchar(EQ);
  else if (!strcmp("!=", op)) putchar(NEQ);
  else assert(-1);
  generate_statement(ast->children[0]);
  generate_statement(ast->children[2]);
}

static void generate_value(mpc_ast_t* ast) {
  if (ast->children[0]->contents[0] == '(') {
    return generate_statement(ast->children[1]);
  }
  mpc_ast_print_to(ast, stderr);
  exit(-1);
}

static void generate_not(mpc_ast_t* ast) {
  putchar(NOT);
  generate_statement(ast->children[1]);
}

static void generate_if(mpc_ast_t* ast) {
  putchar(IF);
  generate_statement(ast->children[1]);
  generate_block(ast->children[2]);
}

static void generate_statement(mpc_ast_t* ast) {
  if (is(ast, "number|decimal|regex")) return generate_number(parse_number(ast));
  if (is(ast, "number|hex|regex")) return generate_number(parse_number(ast));
  if (is(ast, "number|boolean|string")) return generate_number(parse_number(ast));
  if (is(ast, "ident|regex")) return generate_get(ast);
  if (is(ast, "for|>")) return generate_for(ast);
  if (is(ast, "assign|>")) return generate_assign(ast);
  if (is(ast, "call|>")) return generate_call(ast);
  if (is(ast, "sum|>")) return generate_sum(ast);
  if (is(ast, "product|>")) return generate_product(ast);
  if (is(ast, "equality|>")) return generate_equality(ast);
  if (is(ast, "not|>")) return generate_not(ast);
  if (is(ast, "if|>")) return generate_if(ast);
  if (is(ast, "value|>")) return generate_value(ast);
  mpc_ast_print_to(ast, stderr);
  fprintf(stderr, "UNKNOWN: %s\n", ast->tag);
  assert(false);
}
static void generate_block(mpc_ast_t* ast) {
  if (ast->children_num > 3) putchar(DO);
  for (int i = 1; i < ast->children_num - 1; i++) {
    generate_statement(ast->children[i]);
  }
  if (ast->children_num > 3) putchar(END);
}

int main() {

  mpc_parser_t *State  = mpc_new("statement");
  mpc_parser_t *Ident  = mpc_new("ident");
  mpc_parser_t *Number  = mpc_new("number");
  mpc_parser_t *Hex  = mpc_new("hex");
  mpc_parser_t *Dec  = mpc_new("decimal");
  mpc_parser_t *Bool  = mpc_new("boolean");
  mpc_parser_t *For  = mpc_new("for");
  mpc_parser_t *Range  = mpc_new("range");
  mpc_parser_t *By  = mpc_new("by");
  mpc_parser_t *If  = mpc_new("if");
  mpc_parser_t *Assign  = mpc_new("assign");
  mpc_parser_t *Expr  = mpc_new("expression");
  mpc_parser_t *Call  = mpc_new("call");
  mpc_parser_t *Not  = mpc_new("not");
  mpc_parser_t *Or  = mpc_new("or");
  mpc_parser_t *And  = mpc_new("and");
  mpc_parser_t *Equal  = mpc_new("equality");
  mpc_parser_t *Comp  = mpc_new("comparison");
  mpc_parser_t *Sum  = mpc_new("sum");
  mpc_parser_t *Prod  = mpc_new("product");
  mpc_parser_t *Value = mpc_new("value");
  mpc_parser_t *Block = mpc_new("block");
  mpc_parser_t *Prog = mpc_new("program");

  mpc_err_t* err = mpca_lang_contents(MPCA_LANG_DEFAULT, "script.grammar",
    State, Ident, Number, Hex, Dec, Bool, For, Range, By, If, Assign, Expr, Call, Not, Or, And, Equal, Comp, Sum, Prod, Value, Block, Prog, NULL);

  if (err) {
    mpc_err_print_to(err, stderr);
    return -1;
  }

  uint8_t* input = read_stdin();

  // replace comments with white space as preprocessor.
  {
    size_t i = 0;
    bool comment = false;
    uint8_t c;
    while ((c = input[i])) {
      if (comment) {
        if (c == '\n') comment = false;
        else input[i] = ' ';
      }
      else {
        if (c == '#') {
          comment = true;
          input[i] = ' ';
        }
      }
      i++;
    }
  }

  // printf("\n## Source ##\n\n%s\n", input);

  mpc_result_t r;
  if (mpc_parse("input", (char*)input, Prog, &r)) {
    // mpc_ast_print_to(r.output, stderr);
    generate_block(r.output);
    mpc_ast_delete(r.output);
  } else {
    mpc_err_print_to(r.error, stderr);
    mpc_err_delete(r.error);
  }
  free_stdin(input);

  mpc_cleanup(21, State, Ident, Number, Hex, Dec, Bool, For, Range, By, If, Assign, Expr, Call, Not, Or, And, Equal, Comp, Sum, Prod, Value, Block, Prog);

}
