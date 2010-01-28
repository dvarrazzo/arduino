#ifndef ARDULIB_UTILS_H
#define ARDULIB_UTILS_H

#include <stdint.h>

typedef uint8_t pin_t;

static const pin_t INVALID_PIN = 255;

#define array_length(a) (sizeof(a)/sizeof((a)[0]))

#endif // ARDULIB_UTILS_H

