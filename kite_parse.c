#include <stdlib.h>
#include "kite_parse.h"

static kite_ast_node* token_error(kite_token token)
{
	kite_ast_bad_token* node = calloc(1, sizeof(kite_ast_bad_token));
	node->node.type = kite_ast_node_err_bad_token;
	node->node.location = token.location;
	node->token = token;

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
	do { if ((_token).type < 1) return token_error((_token)); } while (0)
#define token_expect(_token, _type) do { if ((_token).type != (_type)) return token_error((_token)); } while (0)
#define token_get_and_expect(_state, _type)\
	do { kite_token token = kite_get_token((_state));\
		 handle_errors_for_token(token);\
		 token_expect(token, (_type)); } while (0)
#define handle_errors_for_node(_node)\
	do { if ((_node)->type < 1) return (_node); } while (0)
#define handle_errors_for_child_node(_node) handle_errors_for_node((kite_ast_node*)(_node))

// The weird indenting is to signify who is to call who
static kite_ast_node* parse_expression(kite_tokenize_state* state);
	static kite_ast_node* parse_from_ident(kite_tokenize_state* state);
		static kite_ast_node* parse_symbol(kite_tokenize_state* state);
		static kite_ast_node* parse_number(kite_tokenize_state* state);
		static kite_ast_node* parse_funcall(kite_ast_symbol* symbol, kite_tokenize_state* state);

kite_ast_node* kite_get_ast_node(kite_tokenize_state* state)
{
	kite_ast_node* expr = parse_expression(state);
	handle_errors_for_node(expr);
	return expr;
}

static kite_ast_node* parse_expression(kite_tokenize_state* state)
{
	kite_token token = kite_get_token(state);
	handle_errors_for_token(token);

	switch (token.type)
	{
		case kite_token_eof:
			return node_eof(token);
		case kite_token_ident:
			kite_backtrack(state);
			return parse_from_ident(state);
		case kite_token_number:
			kite_backtrack(state);
			return parse_number(state);
		default:
			return token_error(token);
	}
}

static kite_ast_node* parse_number(kite_tokenize_state* state)
{
	kite_token token = kite_get_token(state);
	handle_errors_for_token(token);
	token_expect(token, kite_token_number);

	kite_ast_number* node = calloc(1, sizeof(kite_ast_number));
	node->node.type = kite_ast_node_number;
	node->node.location = token.location;
	node->number = kite_get_token_value(state->code, token);

	return (kite_ast_node*)node;
}

static kite_ast_node* parse_from_ident(kite_tokenize_state* state)
{
	kite_ast_symbol* symbol = (kite_ast_symbol*)parse_symbol(state);
	handle_errors_for_child_node(symbol);

	kite_token next = kite_get_token(state);
	handle_errors_for_token(next);
	switch (next.type)
	{
		case kite_token_paren_open:
			kite_backtrack(state);
			return parse_funcall(symbol, state);
		default:
			kite_backtrack(state);
			break;
	}

	return (kite_ast_node*)symbol;
}

static kite_ast_node* parse_symbol(kite_tokenize_state* state)
{
	kite_token ident = kite_get_token(state);
	handle_errors_for_token(ident);
	token_expect(ident, kite_token_ident);

	kite_ast_symbol* symbol = calloc(1, sizeof(kite_ast_symbol));
	symbol->node.type = kite_ast_node_symbol;
	symbol->node.location = ident.location;
	symbol->symbol = kite_get_token_value(state->code, ident);

	return (kite_ast_node*)symbol;
}

static kite_ast_node* parse_funcall(kite_ast_symbol* symbol, kite_tokenize_state* state)
{
	kite_ast_funcall* node = calloc(1, sizeof(kite_ast_funcall));
	node->node.type = kite_ast_node_funcall;
	node->node.location = symbol->node.location;
	node->symbol = symbol;
	kite_init_dynamic_array(kite_ast_node*, node->arguments);

	token_get_and_expect(state, kite_token_paren_open);
	while (1)
	{
		kite_token token = kite_get_token(state);
		handle_errors_for_token(token);
		if (token.type == kite_token_paren_close)
		{
			break;
		}
		else if (token.type == kite_token_eof)
		{
			return token_error(token);
		}
		else
		{
			kite_backtrack(state);

			kite_ast_node* arg = parse_expression(state);
			handle_errors_for_node(arg);
			kite_push(node->arguments, arg);

			kite_token token = kite_get_token(state);
			if (token.type != kite_token_comma)
			{
				token_expect(token, kite_token_paren_close);
				break;
			}
		}
	}

	return (kite_ast_node*)node;
}
