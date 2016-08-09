#ifndef CHARGEN_H
#define CHARGEN_H
#include "fontdescr.h"
#define FCLEN 5

int render_line(int lineno, char *s, uint8_t slen, const FONT_INFO *fonts[], uint8_t *bin, uint16_t binlen, uint8_t *fcli, uint8_t *fclo);

#endif

