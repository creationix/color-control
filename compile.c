#include "mpc.c"
#include "stdin.c"

int main() {

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

  mpc_parser_t *State  = mpc_new("statement");
  mpc_parser_t *Ident  = mpc_new("ident");
  mpc_parser_t *Range  = mpc_new("range");
  mpc_parser_t *Number  = mpc_new("number");
  mpc_parser_t *For  = mpc_new("for");
  mpc_parser_t *Or  = mpc_new("or");
  mpc_parser_t *And  = mpc_new("and");
  mpc_parser_t *Equal  = mpc_new("equality");
  mpc_parser_t *Comp  = mpc_new("comparison");
  mpc_parser_t *Sum  = mpc_new("sum");
  mpc_parser_t *Prod  = mpc_new("product");
  mpc_parser_t *Value = mpc_new("value");
  mpc_parser_t *Prog = mpc_new("program");

  mpc_err_t* err = mpca_lang_contents(MPCA_LANG_DEFAULT, "script.grammar",
    State, Ident, Range, Number, For, Or, And, Equal, Comp, Sum, Prod, Value, Prog, NULL);

  if (err) {
    mpc_err_print(err);
    return -1;
  }

  mpc_result_t r;

  if (mpc_parse("input", (char*)input, Prog, &r)) {
    mpc_ast_print(r.output);
    mpc_ast_delete(r.output);
  } else {
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
  }

  mpc_cleanup(11, State, Ident, Range, For, Or, And, Equal, Comp, Sum, Prod, Value, Prog);

  free_stdin(input);
}
