/*\
 * test-tmpname.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <stdlib.h>
#include "sprtf.hxx"
#include "test-tmpname.hxx"
#ifndef _MSC_VER
#define _tempnam tempnam
#endif

/* --------------------------------------------------------------------
   NOTE: the gcc compiler/linker outputs a WARNING
   test-tmpname.cxx:(.text+0x4e): warning: the use of `tmpnam' is dangerous, better use `mkstemp'
   test-tmpname.cxx:(.text+0x23): warning: the use of `tempnam' is dangerous, better use `mkstemp'
   -------------------------------------------------------------------- */

static const char *module = "test-tmpname";

static char tmp[264];
void test_tmpnam()
{
    char *tn;
    char *temp = getenv("TEMP");
    SPRTF("\n%s: Doing tempnam() and tmpnam() test.\n", module );
    
    tn = _tempnam(temp,"filio");
    if (tn) {
        SPRTF("Got temp name '%s'\n", tn );
    } else {
        SPRTF("tempnam() FAILED!\n" );
    }

    tn = tmpnam(tmp);
    if (tn) {
        SPRTF("Got temp name '%s'\n", tn );
    } else {
        SPRTF("tmpnam() FAILED!\n" );
    }

    SPRTF("%s: Done tempnam() and tmpnam() test.\n", module );

}


// eof = test-tmpname.cxx

