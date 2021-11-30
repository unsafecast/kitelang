#ifndef kite_string_h
#define kite_string_h
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct
{
	char* string;
	size_t size;
} kite_string_view;

#define kite_sv(_arr) ((kite_string_view){.string = (_arr), .size = strlen((_arr))})
#define kite_make_string_view(_string, _size) ((kite_string_view){.string = (_arr), .size = (_size)})

kite_string_view kite_slice(kite_string_view string, size_t start, size_t end);
kite_string_view kite_slice_from(kite_string_view string, size_t from);
bool kite_string_view_equal(kite_string_view string1, kite_string_view string2);

void kite_print_string_view(FILE* file, kite_string_view string);

#endif