#include "mpc.c"
#include "stdin.c"

int main() {

  uint8_t* input = read_stdin();
  printf("Source: '%s'\n", input);

  mpc_parser_t *Or  = mpc_new("or");
  mpc_parser_t *And  = mpc_new("and");
  mpc_parser_t *Equal  = mpc_new("equality");
  mpc_parser_t *Comp  = mpc_new("comparison");
  mpc_parser_t *Sum  = mpc_new("sum");
  mpc_parser_t *Prod  = mpc_new("product");
  mpc_parser_t *Value = mpc_new("value");
  mpc_parser_t *Maths = mpc_new("maths");

  mpca_lang(MPCA_LANG_DEFAULT,
"    or         : <and>        ('||'                      <and>)*;"
"    and        : <equality>   ('&&'                      <equality>)*;"
"    equality   : <comparison> (('==' | '!=')             <comparison>)*;"
"    comparison : <sum>        (('<=' | '>=' | '<' | '>') <sum>)*;"
"    sum        : <product>    (('+' | '-')               <product>)*;"
"    product    : <value>      (('*' | '/' | '%')         <value>)*;"
"    value      : /[0-9]+/ | '(' <or> ')';"
"    maths      : /^/ <or> /$/;"
,
    Or, And, Equal, Comp, Sum, Prod, Value, Maths, NULL);

  mpc_result_t r;

  if (mpc_parse("input", (char*)input, Maths, &r)) {
    mpc_ast_print(r.output);
    mpc_ast_delete(r.output);
  } else {
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
  }

  mpc_cleanup(8, Or, And, Equal, Comp, Sum, Prod, Value, Maths);

  free_stdin(input);
}
