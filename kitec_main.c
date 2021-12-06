#include "kite_tokenize.h"
#include "kite_string.h"
#include "kite_pretty.h"
#include "kite_ast.h"
#include "kite_parse.h"
#include "kite_io.h"

int main()
{
	kite_string_view code = kite_sv(kite_read_whole_file("examples/current_state.kite"));
	kite_tokenize_state tokenize_state = kite_make_tokenize_state(code);

	while (1)
	{
		kite_ast_node* node = kite_parse_toplevel(&tokenize_state);
		kite_pretty_ast_node(stdout, node, code, 0);
		fprintf(stdout, "\n");
		if (node->type == kite_ast_node_eof || node->type < 0) break;
	}

	return 0;
}
