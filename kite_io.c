#include "kite_io.h"

char* kite_read_whole_file(const char* path)
{
	FILE* file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = malloc(size + 1);
	buffer[size] = 0;
	fread(buffer, 1, size, file);
	fclose(file);

	return buffer;
}
