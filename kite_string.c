#include "kite_string.h"

kite_string_view kite_slice(kite_string_view string, size_t start, size_t end)
{
	kite_string_view ret;
	ret.string = string.string + start;
	ret.size = end - start;

	return ret;
}

kite_string_view kite_slice_from(kite_string_view string, size_t from)
{
	return kite_slice(string, from, string.size - from);
}

void kite_print_string_view(FILE* file, kite_string_view string)
{
	fwrite(string.string, string.size, 1, file);
}
