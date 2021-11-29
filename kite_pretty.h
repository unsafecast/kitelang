#ifndef kite_pretty_h
#define kite_pretty_h
#include <stdio.h>
#include "kite_token.h"
#include "kite_ast.h"

void kite_pretty_token(FILE* file, kite_token token, kite_string_view code);
void kite_pretty_ast_node(FILE* file, kite_ast_node* node, kite_string_view code, int indent_level);

#endif