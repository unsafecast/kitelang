#ifndef kite_dynamic_array_h
#define kite_dynamic_array_h
#include <stdint.h>

#define kite_dynamic_array(_type)\
	struct { _type* elements; size_t size; size_t allocated_size; size_t _element_size; }

#define kite_push(_array, _element)\
	do { if ((_array).allocated_size == (_array).size) kite_reserve((_array), 1);\
		 (_array).elements[(_array).size] = (_element);\
		 (_array).size++; } while (0)

#define kite_reserve(_array, _how_many)\
	do { (_array).allocated_size += _how_many;\
		 (_array).elements = realloc((_array).elements, (_array).allocated_size * (_array)._element_size);\
	} while (0)

#define kite_init_dynamic_array(_type, _arr)\
	do { (_arr).elements = malloc(sizeof(_type));\
		 (_arr).size = 0; (_arr).allocated_size = 1; (_arr)._element_size = sizeof(_type); } while (0)

#endif