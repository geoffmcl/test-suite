/*\
 * test-bad-alloc.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <new>
#include "sprtf.hxx"
#include "test-bad-alloc.hxx"

static const char *module = "test-bad-alloc";

void test_bad_alloc()
{
    size_t sz = 10000000000;
    SPRTF("%s: bad alloca test...\n", module);
    try
    {
        int* myarray= new int[sz];
        //int* myarray= new int[10000000000];
        delete myarray;
    }
    catch (std::bad_alloc & ba)
    {
        SPRTF("%s: bad_alloc caught: %s\n", module, ba.what());
    }

    try
    {
        int* myarray2 = new (std::nothrow) int[sz];
        if (myarray2) {
            delete myarray2;
        } else {
            const char *file = __FILE__;
            const int line = __LINE__;
            const char *func = __FUNCTION__;
            SPRTF("%s: allocation FAILED! File %s:%d, function %s\n", module, file, line, func);
        }
    }
    catch (std::bad_alloc & ba)
    {
        SPRTF("%s: bad_alloc caught: %s\n", module, ba.what());
    }

    SPRTF("%s: end bad alloca test...\n", module);

}


// eof = test-bad-alloc.cxx
