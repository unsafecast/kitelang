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

#define token_expect(_token, _type, _fmt, ...) do { if ((_token).type != (_type))\
	{ kite_error(_fmt, __VA_ARGS__); return error_node(); }} while (0)

#define token_get_and_expect(_state, _type, _fmt, ...)\
	do { kite_token token = kite_get_token((_state));\
		 handle_errors_for_token(token);\
		 token_expect(token, (_type), _fmt, __VA_ARGS__); } while (0)

#define handle_errors_for_node(_node)\
	do { if ((_node)->type < 1) return (kite_ast_node*)(_node); } while (0)

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
	return error_node();
}

static kite_ast_node* finish_parse_call(kite_tokenize_state* state, kite_ast_node* name)
{
	token_get_and_expect(state, kite_token_paren_open, "expected '(' at the beginning of a function call\n");
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
	token_get_and_expect(state, kite_token_paren_close, "expected ')' after function call\n");

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

kite_ast_node* parse_datatype(kite_tokenize_state* state)
{
	kite_token_array arr;
	kite_init_dynamic_array(kite_token, arr);

	while (match(state, kite_token_ident) ||
		   match(state, kite_token_star))
	{
		kite_token token = kite_get_token(state);
		handle_errors_for_token(token);
		kite_push(arr, token);
	}

	if (arr.size == 0)
	{
		kite_token token = kite_get_token(state);
		handle_errors_for_token(token);

		kite_string_view val = kite_get_token_value(state->code, token);
		kite_error("expected a type, found '%.*s'\n", val.size, val.string);

		return error_node();
	}

	return make_node(kite_ast_datatype, kite_ast_node_datatype, arr.elements[0].location,
					 {.value=arr});
}

static kite_ast_node* parse_assign(kite_tokenize_state* state, kite_ast_node* name)
{
	token_get_and_expect(state, kite_token_equal, "expected '=' in assignment\n");
	kite_ast_node* value = kite_parse_expression(state);
	handle_errors_for_node(value);

	return make_node(kite_ast_assign, kite_ast_node_assign, value->location,
					 {.name=name, .value=value});
}

static kite_ast_node* parse_var_create(kite_tokenize_state* state)
{
	token_get_and_expect(state, kite_token_var, "expected 'var' at the beginning of variable creation\n");

	kite_ast_node* name = kite_parse_expression(state);
	handle_errors_for_node(name);

	kite_ast_node* type_hint = 0;
	if (!match(state, kite_token_equal))
	{
		type_hint = parse_datatype(state);
		handle_errors_for_node(type_hint);
	}

	kite_ast_node* assign = parse_assign(state, name);
	handle_errors_for_node(assign);

	return make_node(kite_ast_var_create, kite_ast_node_var_create, assign->location,
					 {.assign=(kite_ast_assign*)assign, .type_hint=(kite_ast_datatype*)type_hint});
}

static kite_ast_node* parse_statement(kite_tokenize_state* state)
{
	kite_token token = kite_get_token(state);
	if (token.type == kite_token_eof) return node_eof(token);
	else kite_backtrack(state);

	kite_ast_node* node = 0;

	if (match(state, kite_token_var)) node = parse_var_create(state);
	else
	{
		node = kite_parse_expression(state);
		handle_errors_for_node(node);

		if (match(state, kite_token_equal)) node = parse_assign(state, node);
	}

	token_get_and_expect(state, kite_token_semicolon, "expected ';' at the end of a statement\n");

	return node;
}

static kite_ast_node* parse_body(kite_tokenize_state* state)
{
	kite_token token = kite_get_token(state);
	handle_errors_for_token(token);
	token_expect(token, kite_token_curly_open, "expected '{' at the beginning of block\n");

	kite_ast_node_array arr;
	kite_init_dynamic_array(kite_ast_node*, arr);

	if (match(state, kite_token_curly_close)) 
	{
		kite_token tok = kite_get_token(state);
		return make_node(kite_ast_body, kite_ast_node_body, tok.location, {.body=arr});
	}

	do
	{
		kite_ast_node* node = parse_statement(state);
		handle_errors_for_node(node);

		kite_push(arr, node);
	} while (!match(state, kite_token_curly_close));
	kite_get_token(state);

	return make_node(kite_ast_body, kite_ast_node_body, token.location, {.body=arr});
}

static kite_ast_node* parse_proc(kite_tokenize_state* state)
{
	token_get_and_expect(state, kite_token_proc, "expected 'proc' at the beginning of a procedure\n");

	kite_token name_tok = kite_get_token(state);
	handle_errors_for_token(name_tok);

	kite_string_view name_val = kite_get_token_value(state->code, name_tok);
	token_expect(name_tok, kite_token_ident, "expected an identifier for procedure name, found '%.*s'\n",
										     name_val.size, name_val.string);

	kite_ast_symbol* name = (kite_ast_symbol*)make_node(kite_ast_symbol, kite_ast_node_symbol, name_tok.location,
									                    {.symbol=name_val});

	kite_ast_symbol_array arg_names;
	kite_init_dynamic_array(kite_ast_symbol*, arg_names);

	kite_ast_datatype_array arg_types;
	kite_init_dynamic_array(kite_ast_datatype*, arg_types);

	token_get_and_expect(state, kite_token_paren_open, "expected '(' at the beginning of parameter list\n");
	if (!match(state, kite_token_paren_close))
	{
		do
		{
			kite_token token = kite_get_token(state);
			handle_errors_for_token(token);

			kite_string_view val = kite_get_token_value(state->code, token);
			token_expect(token, kite_token_ident, "expected an identifier for argument name, found '%.*s'\n",
												  val.size, val.string);

			kite_push(arg_names, (kite_ast_symbol*)make_node(kite_ast_symbol, kite_ast_node_symbol, token.location,
										                     {.symbol=val}));

			kite_ast_node* type = parse_datatype(state);
			handle_errors_for_node(type);
			kite_push(arg_types, (kite_ast_datatype*)type);
		} while (get_and_match(state, kite_token_comma));
		kite_backtrack(state);
	}
	token_get_and_expect(state, kite_token_paren_close, "expected ')' at the end of parameter list\n");

	kite_ast_node* body = parse_body(state);
	handle_errors_for_node(body);

	return make_node(kite_ast_proc, kite_ast_node_proc, name_tok.location,
					 {.name=name, .parameters.names = arg_names, .parameters.types=arg_types,
					  .body=(kite_ast_body*)body});
}

kite_ast_node* kite_parse_toplevel(kite_tokenize_state* state)
{
	if (match(state, kite_token_proc)) return parse_proc(state);

	return parse_statement(state);
}
