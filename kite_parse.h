#ifndef kite_parse_h
#define kite_parse_h
#include "kite_ast.h"
#include "kite_tokenize.h"

kite_ast_node* kite_get_ast_node(kite_tokenize_state* state);

#endif