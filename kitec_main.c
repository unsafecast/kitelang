#include "kite_tokenize.h"
#include "kite_string.h"
#include "kite_pretty.h"
#include "kite_ast.h"
#include "kite_parse.h"

int main()
{
	kite_string_view code = kite_sv("proc main { hello_world(hi, hello, 1234); } \0");
	kite_tokenize_state tokenize_state = kite_make_tokenize_state(code);

	kite_ast_node* node;
	do
	{
		node = kite_get_ast_node(&tokenize_state);
		kite_pretty_ast_node(stdout, node, code, 0);
		printf("\n");
	} while (node->type > 0 && node->type != kite_ast_node_eof);

	return 0;
}
