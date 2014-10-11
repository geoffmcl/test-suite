/*\
 * test-math.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _TEST_MATH_HXX_
#define _TEST_MATH_HXX_
// some CONSTANTS
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef RAD2DEG
#define RAD2DEG ( 180.0 / M_PI )
#endif
#ifndef DEG2RAD
#define DEG2RAD ( M_PI / 180.0 )
#endif

extern void test_math();
extern void compare_maths();
extern void test_dist2();

#endif // #ifndef _TEST_MATH_HXX_
// eof - test-math.hxx
