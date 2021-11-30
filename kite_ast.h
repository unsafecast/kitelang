#ifndef kite_ast_h
#define kite_ast_h
#include <stdint.h>
#include "kite_token.h"
#include "kite_location.h"
#include "kite_string.h"
#include "kite_dynamic_array.h"

typedef enum
{
	kite_ast_node_err_bad_token = -99,

	kite_ast_node_none = 0,
	kite_ast_node_symbol,
	kite_ast_node_funcall,
	kite_ast_node_number,

	kite_ast_node_eof,
} kite_ast_node_type;

typedef struct kite_ast_node
{
	kite_ast_node_type type;
	kite_location location;
} kite_ast_node;

typedef struct
{
	kite_ast_node node;
	kite_token token;
} kite_ast_bad_token;

typedef struct
{
	kite_ast_node node;
	kite_string_view symbol;
} kite_ast_symbol;

typedef struct
{
	kite_ast_node node;
	kite_string_view number;
} kite_ast_number;

typedef struct
{
	kite_ast_node node;
	kite_ast_symbol* symbol;

	kite_dynamic_array(kite_ast_node*) arguments;
} kite_ast_funcall;

#endif