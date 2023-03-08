#ifndef PSTR_HELPER_H
#define PSTR_HELPER_H

#include <avr/pgmspace.h>

#define MAX_PSTRING 100
static char PSTR_strbuffer[MAX_PSTRING] = "";
#define P_STR(name, value) const static char name[] PROGMEM = value
#define PSTR_USE(name) (strncpy_P(PSTR_strbuffer, name, MAX_PSTRING - 1))

#endif // PSTR_HELPER_H
