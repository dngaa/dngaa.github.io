#include "core.h"

#include <string.h>
#include <stdarg.h>

char *str_copy(const char *str) {
  size_t size = strlen(str);
  char *buf = (char *)malloc(size + 1);
  memcpy(buf, str, size+1);
  return buf;
}
char *str_copy_part(const char *str, size_t size) {
  char *buf = (char *)malloc(size + 1);
  memcpy(buf, str, size);
  buf[size] = 0;
  return buf;
}

int str_eq(const char *a, const char *b) {
  if (a == b)
    return 1;
  if (!a || !b)
    return 0;
  return !strcmp(a, b);
}

int str_ends_with(const char *a, const char *sub) {
  if (strlen(a) < strlen(sub)) return 0;
  return !memcmp(a+strlen(a)-strlen(sub), sub, strlen(sub));
}

char *mprintf(const char *fmt, ...) {
  va_list arg_ptr;
  va_start( arg_ptr, fmt );
  size_t buffer_size = vsnprintf(NULL, 0, fmt, arg_ptr);
  va_end( arg_ptr );

  va_start( arg_ptr, fmt );
  char *buffer = (char *)malloc(buffer_size + 1);
  vsnprintf(buffer, buffer_size + 1, fmt, arg_ptr);
  buffer[buffer_size] = 0;
  va_end( arg_ptr );

  return buffer;
}

