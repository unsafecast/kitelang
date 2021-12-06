#ifndef kite_parse_h
#define kite_parse_h
#include "kite_ast.h"
#include "kite_tokenize.h"

kite_ast_node* kite_parse_toplevel(kite_tokenize_state* state);
kite_ast_node* kite_parse_expression(kite_tokenize_state* state);

#endif