/*\
 * test-color.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _TEST_COLOR_HXX_
#define _TEST_COLOR_HXX_

typedef struct tagHGT2COLOR {
    short min;
    short max;
    unsigned int color;
    unsigned int freq;
    unsigned int res1;
    unsigned int res2;
    unsigned int res3;
}HGT2COLOR, *PHGT2COLOR;

// new
typedef struct tagrgb_color {
    char red,green,blue;
}rgb_color, *rgb_color_p;

extern double color_distance( rgb_color_p a, rgb_color_p b );
extern void test_color();


#endif // #ifndef _TEST_COLOR_HXX_
// eof - test-color.hxx
