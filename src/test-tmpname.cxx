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

static const char *module = "test-tmpname";

static char tmp[264];
void test_tmpnam()
{
    char *tn;
    char *temp = getenv("TEMP");
    tn = _tempnam(temp,"filio");
    if (tn) {
        SPRTF("Got temp name '%s'\n", tn );
    }

    tn = tmpnam(tmp);
    if (tn) {
        SPRTF("Got temp name '%s'\n", tn );
    }

    exit(1);



}


// eof = test-tmpname.cxx
