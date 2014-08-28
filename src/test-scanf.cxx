/*\
 * test-scanf.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include "sprtf.hxx"
// other includes
#include "test-scanf.hxx"

static const char *module = "test-scanf";

void test_scanf()
{
    int i, result;
    float fp;
    char  c, s[81];
    wchar_t wc, ws[81];
    SPRTF("\n%s: Testing scanf... reuies some inputs...\n", module);
    SPRTF("Enter something like '71 98.6 h z Byte characters' - ie 6 fields\n");
    result = scanf( "%d %f %c %C %80s %80S", &i, &fp, &c, &wc, s, ws ); // C4996
    SPRTF( "The number of fields input is %d\n", result );
    if (result == 6) {
        SPRTF( "The contents are: %d %f %c %C %s %S\n", i, fp, c, wc, s, ws);
    } else {
        SPRTF("%s: Did NOT get 6 fields...\n", module);
    }
#ifdef _MSC_VER
    SPRTF("Again enter something like '36 92.3 y n Wide characters' - again 6 fields\n");
    result = wscanf( L"%d %f %hc %lc %80S %80ls", &i, &fp, &c, &wc, s, ws ); // C4996
    wprintf( L"The number of fields input is %d\n", result );
    if (result == 6) {
        wprintf( L"The contents are: %d %f %C %c %hs %s\n", i, fp, c, wc, s, ws);
    } else {
        SPRTF("%s: Did NOT get 6 fields...\n", module);
    }
#endif    
    SPRTF("%s: End testing scanf...\n", module);
}

// eof = test-scanf.cxx
