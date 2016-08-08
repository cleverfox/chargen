#ifndef CHARGEN_H
#define CHARGEN_H
#include "fontdescr.h"
int render_line(int line, char *s, uint8_t len, const FONT_INFO font, uint8_t *bin, uint16_t binlen, uint8_t dbl, uint8_t spaces);


#endif

