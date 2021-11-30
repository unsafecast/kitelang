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

bool kite_string_view_equal(kite_string_view string1, kite_string_view string2)
{
	if (string1.size != string2.size) return false;

	for (size_t i = 0; i < string1.size; i++)
	{
		if (string1.string[i] != string2.string[i]) return false;
	}

	return true;
}

void kite_print_string_view(FILE* file, kite_string_view string)
{
	fwrite(string.string, string.size, 1, file);
}
