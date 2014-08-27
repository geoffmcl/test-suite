/*\
 * getline.h
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#ifndef _GETLINE_H_
#define _GETLINE_H_
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
extern size_t getline( char **pbuf, size_t *plen, FILE *fp );
#else
#include <stdio.h> // getline(), ...
#endif
#ifndef DEF_MEM_BUMP
#define DEF_MEM_BUMP 256
#endif

#ifdef __cplusplus
}
#endif
#endif // _GETLINE_H_
// eof
