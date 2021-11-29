#ifndef kite_tokenize_h
#define kite_tokenize_h
#include <stdint.h>
#include "kite_string.h"
#include "kite_token.h"

typedef struct
{
	kite_string_view code;
	size_t position;
	size_t last_position;
} kite_tokenize_state;

#define kite_make_tokenize_state(_code) ((kite_tokenize_state){.code = (_code), .position = 0, .last_position = 0})

kite_token kite_get_token(kite_tokenize_state* state);
kite_string_view kite_get_token_value(kite_string_view code, kite_token token);

void kite_backtrack(kite_tokenize_state* state);

#endif