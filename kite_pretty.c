#include "kite_pretty.h"
#include "kite_tokenize.h"

static void print_indent(FILE* file, int indent_level)
{
	for (int i = 0; i < indent_level; i++)
	{
		fputs(" ", file);
	}
}

void kite_pretty_token(FILE* file, kite_token token, kite_string_view code)
{
	fputs("token(", file);

	switch (token.type)
	{
		case kite_token_err_unexpected_char:
			fputs("unexpected_char, ", file);
			break;

		case kite_token_none:
			fputs("none, ", file);
			break;
		case kite_token_ident:
			fputs("ident, ", file);
			break;
		case kite_token_number:
			fputs("number, ", file);
			break;
			
		case kite_token_paren_open:
			fputs("paren_open, ", file);
			break;
		case kite_token_paren_close:
			fputs("paren_close, ", file);
			break;
		case kite_token_comma:
			fputs("comma, ", file);
			break;

		case kite_token_eof:
			fputs("eof, ", file);
			break;
	}

	kite_string_view value = kite_get_token_value(code, token);
	fputs("'", file);
	kite_print_string_view(file, value);
	fputs("')", file);
}

void kite_pretty_ast_node(FILE* file, kite_ast_node* node, kite_string_view code, int indent_level)
{
	print_indent(file, indent_level);

	switch (node->type)
	{
		case kite_ast_node_err_bad_token:
		{
			kite_ast_bad_token* bad_token = (kite_ast_bad_token*)node;
			fputs("bad_token(", file);
			kite_pretty_token(file, bad_token->token, code);
			fputs(")", file);
		} break;

		case kite_ast_node_none:
			fputs("none", file);
			break;
		case kite_ast_node_symbol:
		{
			kite_ast_symbol* symbol = (kite_ast_symbol*)node;
			fputs("symbol(", file);
			kite_print_string_view(file, symbol->symbol);
			fputs(")", file);
		} break;
		case kite_ast_node_number:
		{
			kite_ast_number* number = (kite_ast_number*)node;
			fputs("number(", file);
			kite_print_string_view(file, number->number);
			fputs(")", file);
		} break;
		case kite_ast_node_funcall:
		{
			kite_ast_funcall* funcall = (kite_ast_funcall*)node;
			fputs("funcall(", file);
			kite_pretty_ast_node(file, (kite_ast_node*)funcall->symbol, code, indent_level);
			for (size_t i = 0; i < funcall->arguments.size; i++) {
				fputs(", ", file);
				kite_pretty_ast_node(file, funcall->arguments.elements[i], code, indent_level);
			}
			fputs(")", file);
		} break;

		case kite_ast_node_eof:
			fputs("eof", file);
			break;
	}
}
