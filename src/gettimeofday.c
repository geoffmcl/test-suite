// gettimeofday.c

#include <time.h>
#ifdef _MSC_VER
typedef int bool;
#include <WinSock2.h>
#include <sys/timeb.h>
#include <stdint.h>
#include "sprtf.hxx"
#include <stdio.h>

static const char *module = "gettimeofday";

// preprocessor definitions
#ifdef _WIN64
   //define something for Windows (64-bit)
#elif _WIN32
   //define something for Windows (32-bit)
#elif __APPLE__
   // define something for apple/mac 
#elif __linux
    // linux
#elif __unix // all unices not caught above
    // Unix
#elif __posix
    // POSIX
#else
    // WHAT IS THIS
#endif


// Pauses for a specified number of milliseconds.
void sleep( clock_t wait )
{
   clock_t goal;
   goal = wait + clock();
   while( goal > clock() )
      Sleep(1);
}

///////////////////////////////////////////////////////////////////////////
// various versions of gettimeofday function from various Windows sources
///////////////////////////////////////////////////////////////////////////
// for cmake use
// CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)

int gettimeofday_cf(struct timeval *tp, void *tzp) // from crossfeed source
{
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    tp->tv_sec = (long)timebuffer.time;
    tp->tv_usec = timebuffer.millitm * 1000;
    return 0;
}

int gettimeofday_ac2gl( struct timeval *tv, void *dummy ) // from ac2glview source
{
    static int done_init = 0;
	static LARGE_INTEGER perfFreq;
    int iret = 1;
	LARGE_INTEGER perfCount;
	double freq, count, time;

	tv->tv_sec = 0;
	tv->tv_usec= 0;
	// Get the current count.
	if( QueryPerformanceCounter( &perfCount ) )
    {
        // Get the frequency
        if (!done_init) {
            done_init = 1;
            QueryPerformanceFrequency( &perfFreq );
        }
    	freq  = (double) perfFreq.LowPart;
	    count = (double) perfCount.LowPart;
	    freq += (double) perfFreq.HighPart  *  4294967296.0;
	    count+= (double) perfCount.HighPart *  4294967296.0;
	    time = count / freq;
    	tv->tv_sec = (int) time;
	    tv->tv_usec= (int) ((time - (double)tv->tv_sec) * 1000000.0);
        iret = 0;
    }
    return iret;
}

void gettimeofday_flu( struct timeval *t, void *vp ) // from FLU source
{
    t->tv_sec = 0;
    t->tv_usec = clock();
}

////////////////////////////////////////////////////////////////////////////////////////
// Based on: http://www.google.com/codesearch/p?hl=en#dR3YEbitojA/os_win32.c&q=GetSystemTimeAsFileTime%20license:bsd
// See COPYING for copyright information.
//struct timeval {
//  long tv_sec, tv_usec;
//};

typedef unsigned long long uint64;
typedef long long int64;
typedef double WallTime;
static int gettimeofday_blender(struct timeval *tv, void* tz) // from blender source
{
#define EPOCHFILETIME (116444736000000000ULL)
  FILETIME ft;
  LARGE_INTEGER li;
  uint64 tt;

  GetSystemTimeAsFileTime(&ft);
  li.LowPart  = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  tt = (li.QuadPart - EPOCHFILETIME) / 10;
  tv->tv_sec  = (long)(tt / 1000000);
  tv->tv_usec = tt % 1000000;

  return 0;
}

int64 CycleClock_Now() {
  // TODO(hamaji): temporary impementation - it might be too slow.
  struct timeval tv;
  gettimeofday_blender(&tv, NULL);
  return (int64)((tv.tv_sec * 1000000) + tv.tv_usec);
}

int64 UsecToCycles(int64 usec) {
  return usec;
}

WallTime WallTime_Now() {
  // Now, cycle clock is retuning microseconds since the epoch.
  return CycleClock_Now() * 0.000001;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int gettimeofday_boost(struct timeval *tp, void *vp)  // from boost source
{
    FILETIME ft;
    long long t;
#if defined(_WIN32_WCE)
    // Windows CE does not define GetSystemTimeAsFileTime so we do it in two steps.
    SYSTEMTIME st;
    GetSystemTime( &st );
    SystemTimeToFileTime( &st, &ft );
#else
    GetSystemTimeAsFileTime( &ft );  // never fails
#endif
    t = (long long)ft.dwHighDateTime;
    t = t << 32;
    t += ft.dwLowDateTime;
    t -= 116444736000000000LL;
    t /= 10;  // microseconds
    tp->tv_sec  = (long)( t / 1000000UL);
    tp->tv_usec = (long)( t % 1000000UL);
    return 0;
  }

/////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
struct timezone {
int     tz_minuteswest; /* minutes W of Greenwich */
int     tz_dsttime;     /* type of dst correction */
};

//int gettimeofday_testt(struct timeval *tv, struct timezone *tz)
int gettimeofday_testt(struct timeval *tv, void *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag = 0;
 
  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);
 
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
 
    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
#if 0
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
#endif // 0
  return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
double get_elapsed_secs( struct timeval *tv1, struct timeval *tv2 )
{
    double secs1 = tv1->tv_sec;
    double secs2 = tv2->tv_sec;
    secs1 += ((double)tv1->tv_usec / 1000000.0);
    secs2 += ((double)tv2->tv_usec / 1000000.0);
    return (secs2 - secs1);
}

void show_elapsed( struct timeval *tv1, struct timeval *tv2 )
{
    SPRTF(" elapsed %.4f secs", get_elapsed_secs( tv1, tv2 ));
}

void show_timeval( struct timeval *tv, char *msg )
{
    double secs = tv->tv_sec;
    secs += ((double)tv->tv_usec / 1000000.0);
    SPRTF("%.4f secs - %s", secs, msg );

}

void test_gettimeofday() 
{
    struct timeval cf;
    //struct timeval ac2gl;
    //struct timeval flu;
    struct timeval blender;
    struct timeval cf2;
    //struct timeval ac2gl2;
    //struct timeval flu2;
    struct timeval blender2;
    struct timeval boost;
    struct timeval boost2;
    struct timeval tt;
    struct timeval tt2;
    SPRTF("\n");
    SPRTF("%s: Testing various implementations of gettimeofday\n", module);

    gettimeofday_cf( &cf, 0 );
    //gettimeofday_ac2gl( &ac2gl, 0 );
    gettimeofday_blender( &blender, 0 );
    gettimeofday_boost( &boost, 0 );
    gettimeofday_testt( &tt, 0 );
    //gettimeofday_flu( &flu, 0 );

    show_timeval( &cf, "cf\n" );
    //show_timeval( &ac2gl, "ac2gl\n" );
    show_timeval( &blender, "blender\n" );
    show_timeval( &boost, "boost\n" );
    show_timeval( &tt, "tt\n" );
    //show_timeval( &flu, "flu" );

    SPRTF("Sleeping for 1 second\n");
    sleep(1000);

    gettimeofday_cf( &cf2, 0 );
    //gettimeofday_ac2gl( &ac2gl2, 0 );
    gettimeofday_blender( &blender2, 0 );
    gettimeofday_boost( &boost2, 0 );
    gettimeofday_testt( &tt2, 0 );

    //gettimeofday_flu( &flu2, 0 );

    show_timeval( &cf2, "cf" );
    show_elapsed( &cf, &cf2 );
    SPRTF("\n");

    //show_timeval( &ac2gl2, "ac2gl" );
    //show_elapsed( &ac2gl, &ac2gl2 );
    //SPRTF("\n");

    show_timeval( &blender2, "blender" );
    show_elapsed( &blender, &blender2 );
    SPRTF("\n");

    show_timeval( &boost, "boost" );
    show_elapsed( &boost, &boost2 );
    SPRTF("\n");

    show_timeval( &tt2, "tt" );
    show_elapsed( &tt, &tt2 );
    SPRTF("\n");
    //show_timeval( &flu2, "flu" );
    SPRTF("%s: end testing various implementations of gettimeofday\n", module);
}

static struct timeval start = { 0 };
int start_seconds()
{
    gettimeofday_testt( &start, 0 );
    return 0;
}
double get_seconds_elapsed()
{
    struct timeval tv;
    gettimeofday_testt( &tv, 0 );
    return get_elapsed_secs(&start,&tv);
}

#endif // _MSC_VER
// eof - gettimeofday.c
