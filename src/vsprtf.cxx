/*\
 * vsprtf.cxx - 20150920
 * 
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * from : https://github.com/littlstar/asprintf.c/blob/master/asprintf.c
\*/
/**
 * `asprintf.c' - asprintf
 *
 * copyright (c) 2014 joseph werle <joseph.werle@gmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>

#include "vsprtf.hxx"

static const char *module = "vsprtf";

#ifdef _MSC_VER
#define va_copy(dst,src) dst = src
#endif

// implementation
int asprintf (char **str, const char *fmt, ...)
{
    int size = 0;
    va_list args;

    // init variadic argumens
    va_start(args, fmt);

    // format and get size
    size = vasprintf(str, fmt, args);

    va_end(args);

    return size;
}

int vasprintf (char **str, const char *fmt, va_list args) 
{
    int size = 0;
    va_list tmpa;

    // copy
    va_copy(tmpa, args);

    // apply variadic arguments to
    // sprintf with format to get size
    size = vsnprintf(NULL, size, fmt, tmpa);

    // toss args
    va_end(tmpa);

    // return -1 to be compliant if
    // size is less than 0
    if (size < 0) { return -1; }

    // alloc with size plus 1 for `\0'
    *str = (char *) malloc(size + 1);

    // return -1 to be compliant
    // if pointer is `NULL'
    if (NULL == *str) { return -1; }

    // format string with original
    // variadic arguments and set new size
    size = vsprintf(*str, fmt, args);

    return size;
}

// eof = vsprtf.cxx
