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
		case kite_token_err:
			fputs("error, ", file);
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
		case kite_token_curly_open:
			fputs("curly_open, ", file);
			break;
		case kite_token_curly_close:
			fputs("curly_close, ", file);
			break;
		case kite_token_semicolon:
			fputs("semicolon, ", file);
			break;
		case kite_token_star:
			fputs("star, ", file);
			break;
		case kite_token_equal:
			fputs("equal, ", file);
			break;

		case kite_token_proc:
			fputs("proc, ", file);
			break;
		case kite_token_const:
			fputs("const, ", file);
		case kite_token_var:
			fputs("var, ", file);
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
		case kite_ast_node_err:
		{
			fputs("error", file);
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
			kite_pretty_ast_node(file, funcall->name, code, 0);
			for (size_t i = 0; i < funcall->arguments.size; i++)
			{
				fputs(", ", file);
				kite_pretty_ast_node(file, funcall->arguments.elements[i], code, 0);
			}
			fputs(")", file);
		} break;

		case kite_ast_node_body:
		{
			kite_ast_body* body = (kite_ast_body*)node;
			for (size_t i = 0; i < body->body.size; i++)
			{
				kite_pretty_ast_node(file, body->body.elements[i], code, indent_level + 2);
				fputs("\n", file);
			}
		} break;
		case kite_ast_node_proc:
		{
			kite_ast_proc* proc = (kite_ast_proc*)node;
			fputs("proc(", file);
			kite_pretty_ast_node(file, (kite_ast_node*)proc->name, code, 0);
			for (size_t i = 0; i < proc->parameters.names.size; i++)
			{
				fputs(", ", file);
				kite_pretty_ast_node(file, (kite_ast_node*)proc->parameters.names.elements[i], code, 0);
				fputs(" ", file);
				kite_pretty_ast_node(file, (kite_ast_node*)proc->parameters.types.elements[i], code, 0);
			}
			fputs(")\n", file);
			kite_pretty_ast_node(file, (kite_ast_node*)proc->body, code, indent_level);
		} break;

		case kite_ast_node_datatype:
		{
			kite_ast_datatype* datatype = (kite_ast_datatype*)node;
			fputs("datatype(", file);
			for (size_t i = 0; i < datatype->value.size; i++)
			{
				kite_print_string_view(file, kite_get_token_value(code, datatype->value.elements[i]));
			}
			fputs(")", file);
		} break;

		case kite_ast_node_assign:
		{
			kite_ast_assign* assign = (kite_ast_assign*)node;
			fputs("assign(", file);
			kite_pretty_ast_node(file, (kite_ast_node*)assign->name, code, 0);
			fputs(" = ", file);
			kite_pretty_ast_node(file, assign->value, code, 0);
			fputs(")", file);
		} break;

		case kite_ast_node_var_create:
		{
			kite_ast_var_create* var_create = (kite_ast_var_create*)node;
			fputs("var_create(", file);
			kite_pretty_ast_node(file, (kite_ast_node*)var_create->assign, code, 0);
			if (var_create->type_hint != 0)
			{
				fputs(", ", file);
				kite_pretty_ast_node(file, (kite_ast_node*)var_create->assign, code, 0);
			}
			fputs(")", file);
		} break;

		case kite_ast_node_eof:
			fputs("eof", file);
			break;
	}
}
