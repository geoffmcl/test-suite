/*\
 * test-fullpath.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * WINBASEAPI DWORD WINAPI GetFullPathName(
 *      __in            LPCSTR lpFileName,
 *      __in            DWORD nBufferLength,
 *      __out_ecount_part_opt(nBufferLength, return + 1) LPSTR lpBuffer,
 *      __deref_opt_out LPSTR *lpFilePart
 *      );

\*/

#include <stdio.h>
#include <Windows.h>
#include "sprtf.hxx"
#include "test-fullpath.hxx"

static const char *module = "test-fullpath";
#define MY_BUF_SIZE 264

static char res_buffer[MY_BUF_SIZE];

typedef struct tagSPLITPATH {
    TCHAR drive[_MAX_DRIVE+2];
    TCHAR path[_MAX_DIR+2];
    TCHAR fname[_MAX_FNAME+2];
    TCHAR ext[_MAX_EXT+2];
}SPLITPATH, *PSPLITPATH;

static SPLITPATH sp, usp;

void unix_splitpath( const char *absPath, char *pdrive, char *pdir, char *pfile, char *pext )
{
    size_t ii, len = strlen(absPath);
    int c;
    size_t lastsep, bgnoff, off;
    if (pdrive)
        *pdrive = 0;
    if (pdir)
        *pdir = 0;
    if (pfile)
        *pfile = 0;
    if (pext)
        *pext = 0;
    ii = 0;
    if ((len > 2) && (absPath[1] == ':')) {
        if (pdrive) {
            pdrive[0] = absPath[0];
            pdrive[1] = absPath[1];
            pdrive[2] = 0;
        }
        ii = 2;
    }
    lastsep = 0;    // no path separator
    bgnoff = ii;
    off = 0;
    for (; ii < len; ii++) {
        c = absPath[ii];
        if ((c == '\\') || (c == '/')) {
            lastsep = ii;
        }
    }
    if (lastsep) {
        ii = lastsep + 1;
        for (; bgnoff < len; bgnoff++) {
            if (pdir) {
                pdir[off++] = absPath[bgnoff];
            }
            if (bgnoff == lastsep)
                break;
        }
        if (pdir)
            pdir[off] = 0;
    }
    lastsep = 0;    // no '.'
    bgnoff = ii;
    for (; ii < len; ii++) {
        c = absPath[ii];
        if (c == '.') {
            lastsep = ii;
        }
    }
    if (lastsep) {
        off = 0;    
        for (; bgnoff < len; bgnoff++) {
            if (pfile) {
                pfile[off++] = absPath[bgnoff];
            }
            if ((bgnoff + 1) == lastsep) {
                bgnoff++;
                break;
            }
        }
        if (pfile)
            pfile[off] = 0;
        off = 0;    
        for (; bgnoff < len; bgnoff++) {
            if (pext) {
                pext[off++] = absPath[bgnoff];
            }
        }
        if (pext)
            pext[off] = 0;
        
    } else {
        off = 0;    
        for (; bgnoff < len; bgnoff++) {
            if (pfile) {
                pfile[off++] = absPath[bgnoff];
            }
        }
        if (pfile)
            pfile[off] = 0;
    }
}



void test_rel_path( const char *path )
{
    LPSTR lps = 0;
    res_buffer[0] = 0;
    DWORD dwd = GetFullPathName(
        path,
        MY_BUF_SIZE,
        res_buffer,
        &lps );
    if (dwd) {
        if (dwd >= MY_BUF_SIZE) {
            SPRTF("Relative '%s' requires a buffer of %d bytes!\n", dwd);
        } else {
            SPRTF("Relative '%s' returned '%s' %s\n", path, res_buffer,
                lps ? lps : "");
            memset( &sp, 0, sizeof(SPLITPATH));
            _splitpath( res_buffer,
                sp.drive,
                sp.path,
                sp.fname,
                sp.ext );
            SPRTF("_splitpath: d '%s', p '%s', f '%s', e '%s'\n", sp.drive, sp.path, sp.fname, sp.ext );
            memset( &usp, 0, sizeof(SPLITPATH));
            unix_splitpath( res_buffer,
                usp.drive,
                usp.path,
                usp.fname,
                usp.ext );
            SPRTF("unix-spath: d '%s', p '%s', f '%s', e '%s'\n", usp.drive, usp.path, usp.fname, usp.ext );

        }
    } else {
        SPRTF("GetFullPathName() for '%s' FAILED with %d\n", path, GetLastError() );
    }
}

/* NOTES:
    If you specify "\\test-2\q$\lh" the path returned is "\\test-2\q$\lh"
    If you specify "\\?\UNC\test-2\q$\lh" the path returned is "\\?\UNC\test-2\q$\lh"
    If you specify "U:" the path returned is "U:\"

 */

void test_fullpath()
{
    const char *test1 = "test.txt";
    const char *test2 = "..\\test.txt";
    const char *test3 = "..\\..\\test.txt";
    const char *test31 = ".\\..\\..\\test.txt";
    const char *test4 = "C:\\test.txt";
    const char *test5 = "..\\..\\..\\..\\test.txt";
    const char *test6 = "srcs\\..\\..\\test.txt";
    const char *test7 = "none\\..\\..\\test.txt";
    const char *test8 = "C:\\FG\\17\\test\\CMakeLists.txt";
    DWORD dwd;
    SPRTF("%s: some full and relative path tests\n", module);
    res_buffer[0] = 0;
    dwd = GetCurrentDirectory( 256, res_buffer );
    if (dwd) {
        SPRTF("The current work diectory is '%s'\n", res_buffer );
    } else {
        SPRTF("GetCurrentDirectory() FAILED with %d\n", GetLastError() );
    }
    test_rel_path(test1);
    test_rel_path(test2);
    test_rel_path(test3);
    test_rel_path(test31);
    test_rel_path(test4);
    SPRTF("%s: Next sort of fails, but does its best....\n", module);
    test_rel_path(test5);
    SPRTF("%s: Note these two really FAIL!\n", module);
    test_rel_path(test6);
    test_rel_path(test7);
    SPRTF("%s: Should NOT change!\n", module);
    test_rel_path(test8);

    SPRTF("%s: End full and relative path tests...\n", module);
}

// eof = test-fullpath.cxx
