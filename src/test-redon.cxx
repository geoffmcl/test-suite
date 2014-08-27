/*\
 * test-redon.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <Windows.h>
#include <io.h>
#include "sprtf.hxx"
#include "test-redon.hxx"

static const char *module = "test-redon";
#ifndef VFH
#define VFH(a) ( a && ( a != INVALID_HANDLE_VALUE ))
#endif

void test_isatty()
{
    SPRTF("%s: Using _isatty() function...\n", module);
    if( _isatty( _fileno( stdout ) ) )
        SPRTF( "stdout has not been redirected to a file\n" );
    else
        SPRTF( "stdout has been redirected to a file\n");
    if( _isatty( _fileno( stderr ) ) )
        SPRTF( "stderr has not been redirected to a file\n" );
    else
        SPRTF( "stderr has been redirected to a file\n");
    if( _isatty( _fileno( stdin ) ) )
        SPRTF( "stdin has not been redirected from a file\n" );
    else
        SPRTF( "stdin has been redirected from a file\n");
}

void test_redon()
{
    DWORD dwMode;
    BOOL bRedON;
    SPRTF("%s: Test is redirection is ON for stdout, stderr and stdin...\n", module);
    test_isatty();
    SPRTF("%s: Using GetConsoleMode() function...\n", module);
    HANDLE hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );   // Standard out
    HANDLE hErrOut = GetStdHandle( STD_ERROR_HANDLE  );   // error out
    HANDLE hStdIn  = GetStdHandle( STD_INPUT_HANDLE  );   // stdin
    if( VFH(hStdOut) )
    {
        if( !GetConsoleMode( hStdOut, &dwMode ) ) {
            SPRTF("stdout IS redirected\n");
            bRedON = TRUE;
        } else {
            SPRTF("stdout is NOT redirected\n");
        }
    } else {
        SPRTF("Did not get a valid handle for stdout!\n");
    }
    if( VFH(hErrOut) )
    {
        if( !GetConsoleMode( hErrOut, &dwMode ) ) {
            SPRTF("stderr IS redirected\n");
            bRedON = TRUE;
        } else {
            SPRTF("stderr is NOT redirected\n");
        }
    } else {
        SPRTF("Did not get a valid handle for stderr!\n");
    }
    if( VFH(hStdIn) )
    {
        if( !GetConsoleMode( hStdIn, &dwMode ) ) {
            SPRTF("stdin IS redirected\n");
            bRedON = TRUE;
        } else {
            SPRTF("stdin is NOT redirected\n");
        }
    } else {
        SPRTF("Did not get a valid handle for stdin!\n");
    }


    SPRTF("%s: End test of is redirection ON/OFF for std handles...\n", module);
}

// eof = test-redon.cxx
