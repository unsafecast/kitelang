#ifndef kite_location_h
#define kite_location_h
#include <stdint.h>

typedef struct
{
	size_t start;
	size_t end;
} kite_location;

#define kite_make_location(_start, _end) ((kite_location){.start = (_start), .end = (_end)})

#endif