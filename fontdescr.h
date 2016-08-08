#ifndef FONTDESCR_H
#define FONTDESCR_H

#include <stdint.h>

#define uint_8 uint8_t
typedef struct {
    uint8_t width;
    uint16_t offset;
} FONT_CHAR_INFO;

typedef struct {
    const uint8_t height;
    const uint16_t start;
    const uint16_t end;
    const uint8_t spwidth;
    const FONT_CHAR_INFO *descriptor;
    const uint8_t *bitmaps;

} FONT_INFO;

#endif

