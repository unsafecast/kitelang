#include <stdlib.h>
#include "kite_parse.h"
#include "kite_error.h"

static kite_ast_node* error_node()
{
	kite_ast_node* node = calloc(1, sizeof(kite_ast_node));
	node->type = kite_ast_node_err;

	return (kite_ast_node*)node;
}

static kite_ast_node* node_eof(kite_token token)
{
	kite_ast_node* node = calloc(1, sizeof(kite_ast_node));
	node->type = kite_ast_node_eof;
	node->location = token.location;

	return node;
}

#define handle_errors_for_token(_token)\
	do { if ((_token).type < 1) return error_node(); } while (0)
#define token_expect(_token, _type) do { if ((_token).type != (_type)) return error_node(); } while (0)
#define token_get_and_expect(_state, _type)\
	do { kite_token token = kite_get_token((_state));\
		 handle_errors_for_token(token);\
		 token_expect(token, (_type)); } while (0)
#define handle_errors_for_node(_node)\
	do { if ((_node)->type < 1) return (_node); } while (0)
#define make_node(_type, _enum_type, _location, ...)\
	({ _type* node = calloc(1, sizeof(_type));\
	    *node = (_type)__VA_ARGS__;\
	    node->node.type = (_enum_type);\
	    node->node.location = (_location);\
	    (kite_ast_node*)node; })

static bool get_and_match(kite_tokenize_state* state, kite_token_type type)
{
	kite_token token = kite_get_token(state);
	return token.type == type;
}

static bool match(kite_tokenize_state* state, kite_token_type type)
{
	bool result = get_and_match(state, type);
	kite_backtrack(state);

	return result;
}

static kite_ast_node* parse_simplest_expression(kite_tokenize_state* state)
{
	kite_token token = kite_get_token(state);
	handle_errors_for_token(token);

	if (token.type == kite_token_ident)
	{
		return make_node(kite_ast_symbol, kite_ast_node_symbol, token.location,
						{.symbol=kite_get_token_value(state->code, token)});
	}

	if (token.type == kite_token_number)
	{
		return make_node(kite_ast_number, kite_ast_node_number, token.location,
						{.number=kite_get_token_value(state->code, token)});
	}

	kite_string_view str = kite_get_token_value(state->code, token);
	kite_error("expected an expression, got '%.*s'\n", str.size, str.string);
	kite_hint("sdadasda\n");
	return error_node();
}

static kite_ast_node* finish_parse_call(kite_tokenize_state* state, kite_ast_node* name)
{
	token_get_and_expect(state, kite_token_paren_open);
	kite_ast_node_array args;
	kite_init_dynamic_array(kite_ast_node*, args);

	if (match(state, kite_token_paren_close))
	{
		handle_errors_for_token(kite_get_token(state));
		return make_node(kite_ast_funcall, kite_ast_node_funcall, name->location,
						 {.name = name, .arguments = args});
	}

	do
	{
		kite_ast_node* expr = kite_parse_expression(state);
		handle_errors_for_node(expr);
		kite_push(args, expr);
	} while (get_and_match(state, kite_token_comma));
	kite_backtrack(state);
	token_get_and_expect(state, kite_token_paren_close);

	return make_node(kite_ast_funcall, kite_ast_node_funcall, name->location,
					 {.name = name, .arguments = args});
}

static kite_ast_node* parse_call_or_(kite_tokenize_state* state)
{
	kite_ast_node* expr = parse_simplest_expression(state);
	handle_errors_for_node(expr);

	while (1)
	{
		if (match(state, kite_token_paren_open))
		{
			expr = finish_parse_call(state, expr);
		}
		// Here we're also gonna match struct access & stuff like this
		else
		{
			break;
		}
	}

	return expr;
}

kite_ast_node* kite_parse_expression(kite_tokenize_state* state)
{
	return parse_call_or_(state);
}

static kite_ast_node* parse_statement(kite_tokenize_state* state)
{
	return kite_parse_expression(state);
}

kite_ast_node* kite_parse_toplevel(kite_tokenize_state* state)
{
	return parse_statement(state);
}
