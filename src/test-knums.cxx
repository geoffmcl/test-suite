/*\
 * test_knums.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <stdint.h>
#ifdef _MSC_VER
//////////////////////////////////////////////////////////////////////
#include <WinSock2.h>
//////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////
#include <unistd.h>
//////////////////////////////////////////////////////////////////////
#endif // _MSC_VER
#include "sprtf.hxx"
#include "utils.hxx"
#include "test-knums.hxx"

#ifndef _MSC_VER
///////////////////////////////////////////////////////////////////////////////////////////////
typedef uint64_t UINT64;
typedef uint32_t DWORD;
typedef char TCHAR;
typedef struct _ULARGE_INTEGER {
    union {
        struct {
            DWORD LowPart;
            DWORD HighPart;
        };
    };
    unsigned long long QuadPart;
}ULARGE_INTEGER;
#define _UI64_MAX UINT64_MAX
typedef uint64_t __int64;
#define is_file_or_directory64 is_file_or_directory32
#define get_last_file_size64 get_last_file_size32
///////////////////////////////////////////////////////////////////////////////////////////////
#endif // !_MSC_VER

static const char *module = "test_knums";

TCHAR * get_k_num64( ULARGE_INTEGER uli )
{
   static TCHAR _s_kbuf64[264];
   TCHAR * pb = _s_kbuf64;
   double byts = ((double)uli.HighPart * 4294967296.0);
   byts += uli.LowPart;
   if( byts < 1024 ) {
      sprintf(pb, "%f bytes", byts);
   } else if( byts < 1024*1024 ) {
      double ks = ((double)byts / 1024.0);
      sprintf(pb, "%0.2f KB", ks);
   } else if( byts < 1024*1024*1024 ) {
      double ms = ((double)byts / (1024.0*1024.0));
      sprintf(pb, "%0.2f MB", ms);
   } else {
      double gs = ((double)byts / (1024.0*1024.0*1024.0));
      sprintf(pb, "%0.2f GB", gs);
   }
   return pb;
}


static char sbuff[1024];
static char dbuff[1024];
void test_k_num()
{
    char *cp;
    long long tot = 2710732035;
    unsigned long g_dir_count = 4568;
    unsigned long g_file_clean = 75096;
    ULARGE_INTEGER g_total64_size = { 0 };
    g_total64_size.QuadPart = tot;
    SPRTF("\n");
    SPRTF("%s: test k numbers...\n", module);

    sprintf(sbuff,"%llu", g_total64_size.QuadPart);
    nice_num( dbuff, sbuff );
    cp = get_k_num64( g_total64_size );
    SPRTF( "Got %d dirs, %d files, total (approx) %s (%s bytes) ...\n",
        g_dir_count,
        g_file_clean, // (g_do_clean[0] ? g_file_clean : g_file_count),
        cp,
        dbuff );
    g_total64_size.QuadPart = _UI64_MAX;
    sprintf(sbuff,"%llu", g_total64_size.QuadPart);
    nice_num( dbuff, sbuff );
    cp = get_k_num64( g_total64_size );
    SPRTF( "Max I64 total (approx) %s (%s bytes) ...\n",
        cp,
        dbuff );
    SPRTF("%s: end test k numbers...\n", module);

}

// eof = test_knums.cxx
