#ifndef kite_error_h
#define kite_error_h

// TODO: Remake with _Generic
// TODO: Add more information like location and file

void kite_error(const char* fmt, ...);
void kite_warning(const char* fmt, ...);
void kite_hint(const char* fmt, ...);

#endif