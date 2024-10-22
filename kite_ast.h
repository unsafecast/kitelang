#ifndef kite_ast_h
#define kite_ast_h
#include <stdint.h>
#include "kite_token.h"
#include "kite_location.h"
#include "kite_string.h"
#include "kite_dynamic_array.h"

typedef enum
{
	kite_ast_node_err = -99,

	kite_ast_node_none = 0,
	kite_ast_node_symbol,
	kite_ast_node_funcall,
	kite_ast_node_number,
	kite_ast_node_datatype,

	kite_ast_node_proc,
	kite_ast_node_body,
	kite_ast_node_assign,
	kite_ast_node_var_create,

	kite_ast_node_eof,
} kite_ast_node_type;

typedef struct kite_ast_node
{
	kite_ast_node_type type;
	kite_location location;
} kite_ast_node;
typedef kite_dynamic_array(kite_ast_node*) kite_ast_node_array;

typedef struct
{
	kite_ast_node node;
	kite_string_view symbol;
} kite_ast_symbol;
typedef kite_dynamic_array(kite_ast_symbol*) kite_ast_symbol_array;

typedef struct
{
	kite_ast_node node;
	kite_string_view number;
} kite_ast_number;

typedef struct
{
	kite_ast_node node;
	kite_ast_node* name;

	kite_ast_node_array arguments;
} kite_ast_funcall;

typedef struct
{
	kite_ast_node node;
	kite_ast_node_array body;
} kite_ast_body;

typedef struct
{
	kite_ast_node node;
	kite_token_array value;
} kite_ast_datatype;
typedef kite_dynamic_array(kite_ast_datatype*) kite_ast_datatype_array;

typedef struct
{
	kite_ast_node node;
	kite_ast_symbol* name;

	struct
	{
		kite_ast_symbol_array names;
		kite_ast_datatype_array types;
	} parameters;
	kite_ast_datatype* return_type;

	kite_ast_body* body;
} kite_ast_proc;

typedef struct
{
	kite_ast_node node;
	kite_ast_node* name;
	kite_ast_node* value;
} kite_ast_assign;

typedef struct
{
	kite_ast_node node;
	kite_ast_assign* assign;
	kite_ast_datatype* type_hint;  // NULL if none
} kite_ast_var_create;

#endif