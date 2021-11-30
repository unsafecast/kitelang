#ifndef kite_token_h
#define kite_token_h
#include "kite_location.h"
#include "kite_string.h"

typedef enum
{
	kite_token_err_unexpected_char = -99,

	kite_token_none = 0,
	kite_token_ident,
	kite_token_number,

	kite_token_paren_open,
	kite_token_paren_close,
	kite_token_comma,

	kite_token_eof,
} kite_token_type;

typedef struct
{
	kite_token_type type;
	kite_location location;
} kite_token;

#define kite_make_token(_type, _location) ((kite_token){.type = (_type), .location = (_location)})

#endif