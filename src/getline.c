/*\
 * getline.c
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * getline() function for windows
 *
\*/
#include <stdio.h>
#include <stdlib.h> // for realloc(), ...
#include <errno.h>
#include "getline.h"

#ifdef _MSC_VER
size_t getline( char **pbuf, size_t *plen, FILE *fp )
{
    int c;
    char *buf;
    size_t off, len;
    if (!fp || !pbuf || !plen) {
        errno = EINVAL; /* Invalid argument. */
        return -1;
    }
    off = 0;
    c = getc(fp);
    if (c == EOF)
        return EOF;
    buf = *pbuf;
    len = *plen;
    while ((c != '\n') && (c != '\r') && (c != EOF) ) {
        if ( (off+2) > len ) {
            len += DEF_MEM_BUMP;
            // must allocate/reallocate buffer
            if (buf) {
                buf = (char *)realloc(buf,len);
                if (!buf) {
                    fprintf(stderr,"Memory reallocation FAILED on %d bytes!\n", (int)len);
                    errno = ENOMEM; /* Not enough space. */
                    return -1;
                }
            } else {
                buf = (char *)malloc(len);
                if (!buf) {
                    fprintf(stderr,"Memory allocation FAILED on %d bytes!\n", (int)len);
                    errno = ENOMEM; /* Not enough space. */
                    return -1;
                }
            }
            *pbuf = buf;
            *plen = len;
        }
        buf[off++] = (char) c;
        c = getc(fp);
    }
    if (buf) {
        if (c != EOF)
            buf[off++] = '\n';
        buf[off] = 0;
    }
    if (off) {
        if ((c != EOF) && (fp != stdin) && (fp != stderr)) {
            // if opened in BINARY mode, will get CR/LF separately, sooooo
            if (c == '\r') {
                c = getc(fp);
                // just eat a '\n'
                if (c != '\n') {
                    if (c != EOF)
                       ungetc(c,fp); // put it back
                }
            }
        }
        return off;
    }
    if (c == EOF)
        return c;
    return off;
}

#endif // _MSC_VER only
// eof
