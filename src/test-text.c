/*\
 * test-text.c
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// other includes
#include "test-text.h"

static const char *module = "test-text";

// given various string length, extract begin, add elipse, extract end

static const char *text1 = "abc";
static const char *text2 = "Begin str, and end str";
static const char *text3 = "abcdefghijklmnopqrstuvwxyz123456789";

#define MX_TXT 8
static char buffer[(MX_TXT*4)+8];

char *get_text( const char *text )
{
    int len = (int) strlen(text);
    char *cp  = (char *)text;
    char *end = cp + len;
    int i = 0;
    int insp = 0;
    char c;
    if (len <= ((MX_TXT * 2) + 3)) {
        buffer[0] = 0;
        while (cp < end) {
            c = *cp;
            cp++;
            if (c == '\n') {
                buffer[i++] = '\\';
                buffer[i++] = 'n';
            } else if ( c == ' ' ) {
                if (!insp)
                    buffer[i++] = c;
                insp = 1;
            } else {
                buffer[i++] = c;
                insp = 0;
            }
        }
    } else {
        char *end1 = cp + MX_TXT;
        char *bgn = cp + (len - MX_TXT);
        buffer[0] = 0;
        if (bgn < end1)
            bgn = end1;
        while (cp < end1) {
            c = *cp;
            cp++;
            if (c == '\n') {
                buffer[i++] = '\\';
                buffer[i++] = 'n';
            } else if ( c == ' ' ) {
                if (!insp)
                    buffer[i++] = c;
                insp = 1;
            } else {
                buffer[i++] = c;
                insp = 0;
            }
            if (i >= MX_TXT)
                break;
        }
        c = '.';
        if ((i < len)&&(cp < bgn)) {
            buffer[i++] = c;
            cp++;
            if ((i < len)&&(cp < bgn)) {
                buffer[i++] = c;
                cp++;
                if ((i < len)&&(cp < bgn)) {
                    buffer[i++] = c;
                    cp++;
                }
            }
        }
        cp = bgn;
        while (cp < end) {
            c = *cp;
            cp++;
            if (c == '\n') {
                buffer[i++] = '\\';
                buffer[i++] = 'n';
            } else if ( c == ' ' ) {
                if (!insp)
                    buffer[i++] = c;
                insp = 1;
            } else {
                buffer[i++] = c;
                insp = 0;
            }
        }
    }
    buffer[i] = 0;
    return buffer;
}

static int count = 0;
#define MY_MX_STK 1024
void test_stack()
{
    char buf[MY_MX_STK];
    count++;
    printf("Count %d\n", count);
    test_stack();
}


void test_text()
{
    char buff[MX_TXT*4];
    int i, j, max;
    char *cp;
    // test_stack();
    cp = get_text(text1);
    printf("text [%s] got [%s]\n", text1, cp);    
    cp = get_text(text2);
    printf("text [%s] got [%s]\n", text2, cp);
    max = (MX_TXT * 2) + 3;
    for (j = 0; j < 5; j++) {
        for (i = 0; i < max; i++) {
            buff[i] = text3[i];
        }
        buff[i] = 0;
        cp = get_text(buff);
        printf("text [%s] got [%s]\n", buff, cp);
        max++;
    }
    exit(0);
}


// eof = test-text.c
