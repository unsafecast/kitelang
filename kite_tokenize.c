#include <ctype.h>
#include <stdio.h>
#include "kite_tokenize.h"

#define is_ident(_char) ((_char) == '_' || isalpha((_char)))
#define is_ident_inside(_char) (is_ident(_char) || isdigit((_char)))

kite_token kite_get_token(kite_tokenize_state* state)
{
	while (isspace(state->code.string[state->position]))
		state->position++;

	state->last_position = state->position;
	size_t start = state->position;

	if (is_ident(state->code.string[state->position]))
	{
		while (is_ident_inside(state->code.string[state->position]))
			state->position++;
		return kite_make_token(kite_token_ident,
							   kite_make_location(start, state->position));
	}
	else if (isdigit(state->code.string[state->position]))
	{
		while (isdigit(state->code.string[state->position]))
			state->position++;
		return kite_make_token(kite_token_number,
							   kite_make_location(start, state->position));
	}
	else switch (state->code.string[state->position])
	{
		case '(':
			state->position++;
			return kite_make_token(kite_token_paren_open,
								   kite_make_location(start, state->position));
		case ')':
			state->position++;
			return kite_make_token(kite_token_paren_close,
								   kite_make_location(start, state->position));
		case ',':
			state->position++;
			return kite_make_token(kite_token_comma,
								   kite_make_location(start, state->position));

		case '\0':
			return kite_make_token(kite_token_eof,
								   kite_make_location(start, state->position));
	}

	return kite_make_token(kite_token_err_unexpected_char,
						   kite_make_location(state->position, state->position));
}

kite_string_view kite_get_token_value(kite_string_view code, kite_token token)
{
	return kite_slice(code, token.location.start, token.location.end);
}

void kite_backtrack(kite_tokenize_state* state)
{
	state->position = state->last_position;
}
