#include "kite_error.h"
#include <stdio.h>
#include <stdarg.h>

void kite_error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\033[31merror: \033[0m");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void kite_warning(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\033[33mwarning: \033[0m");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void kite_hint(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\033[36mhint: \033[0m");
    vfprintf(stderr, fmt, args);
    va_end(args);
}
