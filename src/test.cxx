
// test.cxx
#include <vector>
#include <sstream>
#ifdef _MSC_VER
//////////////////////////////////////////////////////////////////////
#ifdef USE_WSOCK2_LIB
#include <WinSock2.h>
#else
#include <winsock.h>
#endif
#include <conio.h>
//////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////
#include <sys/socket.h> // socklen_t, ...
//////////////////////////////////////////////////////////////////////
#endif // _MSC_VER
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#ifdef HAVE_SIMGEAR
#include <simgear/constants.h>
#endif
#include <stdint.h>
#include <cmath>
#include <signal.h>
#include <string.h>
#include <time.h>

#ifdef DO_POSTGRESQL_TEST
#include "test-sql.hxx"
#endif
#include "cf-log.hxx"
#include "sprtf.hxx"
#include "test.hxx"
#include "test-array.h"
#include "gettimeofday.h"
#include "test-magvar.hxx"
#include "test-intersect.hxx"
#include "test-map.hxx"
#include "utf8/test-utf8.hxx"
#include "global.hxx"
#include "test-codeset.h"
#include "utils.hxx"
#include "test-wildcard.hxx"
#include "getline.h"
#include "test-math.hxx"
#include "test-color.hxx"
#include "test-stg-inc.hxx"
#include "test-tmpname.hxx"
// #include "test-mmap.hxx"
#ifdef _MSC_VER
#include "test-fullpath.hxx"
#include "test-redon.hxx"
#endif
#include "test-secs.hxx"
#include "test-scanf.hxx"
#include "test-open.hxx"

extern void test_strtoimax();

extern void test_mmap();

static const char *module = "test";

// from : http://www.highprogrammer.com/alan/windev/visualstudio.html
#define Stringize( L )			#L
#define MakeString( M, L )		M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define Reminder __FILE__ "(" $Line ") : Reminder: "
#ifdef _MSC_VER
///////////////////////////////////////////////////////////////////////////////////////////////
#pragma message(Reminder "Show a 'reminder' message, with file and line!")
///////////////////////////////////////////////////////////////////////////////////////////////
#else
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
///////////////////////////////////////////////////////////////////////////////////////////////
#endif

typedef struct _UINT128 {
  UINT64 lower;
  UINT64 upper;
} UINT128, *PUINT128;

static UINT128 i128;
// from aixclient.h
#if defined(WIN32)  ||  defined(_WIN32_WCE)
#define socklen_t int
#if defined(_MSC_VER)
	typedef int (__stdcall *iaxc_sendto_t)(SOCKET, const void *, size_t, int,
			const struct sockaddr *, socklen_t);
	typedef int (__stdcall *iaxc_recvfrom_t)(SOCKET, void *, size_t, int,
			struct sockaddr *, socklen_t *);
#else
	typedef int PASCAL (*iaxc_sendto_t)(SOCKET, const char *, int, int,
			const struct sockaddr *, int);
	typedef int PASCAL (*iaxc_recvfrom_t)(SOCKET, char *, int, int,
			struct sockaddr *, int *);
#endif
#else
	/*!
		Defines the portotype for an application provided sendto implementation.
	*/
	typedef int (*iaxc_sendto_t)(int, const void *, size_t, int,
			const struct sockaddr *, socklen_t);
	/*!
		Defines the portotype for an application provided recvfrom implementation.
	*/
	typedef int (*iaxc_recvfrom_t)(int, void *, size_t, int,
			struct sockaddr *, socklen_t *);
#endif

#if defined(WIN32)  ||  defined(_WIN32_WCE)
#if defined(_MSC_VER)
typedef int (__stdcall *iax_sendto_t)(SOCKET, const void *, size_t, int,
		const struct sockaddr *, socklen_t);
typedef int (__stdcall *iax_recvfrom_t)(SOCKET, void *, size_t, int,
		struct sockaddr *, socklen_t *);
#else
typedef int PASCAL (*iax_sendto_t)(SOCKET, const char *, int, int,
		const struct sockaddr *, int);
typedef int PASCAL (*iax_recvfrom_t)(SOCKET, char *, int, int,
		struct sockaddr *, int *);
#endif
#else
typedef int (*iax_sendto_t)(int, const void *, size_t, int,
		const struct sockaddr *, socklen_t);
typedef int (*iax_recvfrom_t)(int, void *, size_t, int,
		struct sockaddr *, socklen_t *);
#endif


/* external global networking replacements */
static iaxc_sendto_t   iaxc_sendto = (iaxc_sendto_t) sendto;
static iaxc_recvfrom_t iaxc_recvfrom = (iaxc_recvfrom_t)recvfrom;

/* external global networking replacements */
static iax_sendto_t   iax_sendto = (iax_sendto_t) sendto;
static iax_recvfrom_t iax_recvfrom = (iax_recvfrom_t) recvfrom;

int verbosity = 0;

static int check_me()
{
    int i;
    SPRTF("%s: Any key to continue!\n", module);
    getchar();
    i = 0;
    return i;
}

//static float f1 = 1.2;
static float f2 = (float)1.2;
static float f3 = 1.2f;


#ifdef ADD_CGAL_TEST
#include <CGAL/Simple_cartesian.h>

#include <CGAL/Cartesian.h>

#include <CGAL/Gmpq.h>

#include <CGAL/Algebraic_kernel_for_circles_2_2.h>

#include <CGAL/Circular_kernel_2.h>

#include <CGAL/Filtered_kernel.h>



typedef CGAL::Simple_cartesian<double> K;

typedef CGAL::Cartesian<double> K2;

typedef CGAL::Simple_cartesian<CGAL::Gmpq> K3;

typedef K3::RT FT3;


typedef CGAL::Algebraic_kernel_for_circles_2_2<double>          AK;

typedef CGAL::Circular_kernel_2<K, AK> CK;


typedef CGAL::Filtered_kernel<K> FK;



int cgal_main() {

  K::Point_2 default_p;
  
  K::Point_2 p(-1./3, 2.);

  K::Vector_2 v = p - CGAL::ORIGIN;

  K::Circle_2 c(p, 10);


  K2::Point_2 p2(-1./3, 2.);

  K2::Vector_2 v2 = p2 - CGAL::ORIGIN;

  K2::Circle_2 c2(p2, 10);

  // no correct pretty-printer for CGAL::Gmpq
  
  K3::Point_2 p3(-3, 10);

  K3::Vector_2 v3 = p3 - CGAL::ORIGIN;


  CK::Point_2 p4;

  CK::Vector_2 v4;


  FK::Point_2 default_p5;

  FK::Vector_2 default_v5;


  FK::Point_2 p5(1, 2);

  FK::Vector_2 v5(3, 4);


  return 0;

}


#endif


#ifdef ADD_MPFR_TEST
#include <mpfr.h>

void check_mpfr()
{
    SPRTF("MPFR library: %-12s\nMPFR header:  %s (based on %d.%d.%d)\n",
               mpfr_get_version (), MPFR_VERSION_STRING, MPFR_VERSION_MAJOR,
               MPFR_VERSION_MINOR, MPFR_VERSION_PATCHLEVEL);


}
#endif // #ifdef ADD_MPFR_TEST

void test_compares()
{
    SPRTF("Comparing %p with %p... ", iaxc_sendto, sendto);
    if (iaxc_sendto == (iaxc_sendto_t)sendto)
        SPRTF("ok\n");
    else
        SPRTF("FAILED!\n");

    SPRTF("Comparing %p with %p... ", iaxc_recvfrom, recvfrom);
    if (iaxc_recvfrom == (iaxc_recvfrom_t)recvfrom)
        SPRTF("ok\n");
    else
        SPRTF("FAILED!\n");

    SPRTF("Comparing %p with %p... ", iax_sendto, sendto);
    if (iax_sendto == (iax_sendto_t)sendto)
        SPRTF("ok\n");
    else
        SPRTF("FAILED!\n");

    SPRTF("Comparing %p with %p... ", iax_recvfrom, recvfrom);
    if (iax_recvfrom == (iax_recvfrom_t)recvfrom)
        SPRTF("ok\n");
    else
        SPRTF("FAILED!\n");

}


#ifdef _MSC_VER
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct tagDW2STG {
    DWORD dw;
    char *stg;
} DW2STG, *PDW2STG;

DW2STG dw2stg[] = {
    { WAIT_ABANDONED, "WAIT_ABANDONED" },
    { WAIT_OBJECT_0, "WAIT_OBJECT_0 (signaled)" },
    { WAIT_TIMEOUT, "WAIT_TIMEOUT" },
    { WAIT_FAILED, "WAIT_FAILED" },
    { 0, 0 }
};

char *get_dw_stg( DWORD ret )
{
    PDW2STG p = &dw2stg[0];
    while (p->stg) {
        if (p->dw == ret) {
            return p->stg;
        }
        p++;
    }
    return "UNKNOWN";
}

static int do_freopen = 1;
DWORD test_wait_for()
{
    DWORD ret, count = 0;
    FILE *fd_stdin;
    FILE *fd_stdout;
    FILE *fd_stderr;
    int timeout_ms = 1000;
    if (do_freopen) {
        if (AllocConsole()) {
            SPRTF("AllocConsole() SUCCEEDED!\n");
        } else {
            SPRTF("AllocConsole() failed, and expected.\n");
        }
        fd_stdin = freopen ( "conin$", "r", stdin );
        SPRTF("Result for freopen( \"conin$\", \"r\", stdin ) = %p\n", fd_stdin);
        fd_stdout = freopen ( "conout$", "w", stdout );
        SPRTF("Result for freopen( \"conout$\", \"r\", stdout ) = %p\n", fd_stdout);
        fd_stderr = freopen ( "conout$", "w", stderr );
        SPRTF("Result for freopen( \"conout$\", \"r\", stderr ) = %p\n", fd_stderr);
        SPRTF("Waiting for key... using WaitForSingleObject()\n");
    }

    start_seconds();
    while (1) {
        double d;
        ret = WaitForSingleObject(
            GetStdHandle( STD_INPUT_HANDLE ),
		    timeout_ms);
        if (ret == WAIT_OBJECT_0) {
            d = get_seconds_elapsed();
            int ch = _getch();
            SPRTF("Got char %d (0x%x), after %f seconds...\n", ch, ch, d );
            break;
        }
        if ((count % 100) == 0) {
            d = get_seconds_elapsed();
            char *cp = get_dw_stg(ret);
            SPRTF("Wait return %d (%s), after %f secoonds (cumm)\n", ret, cp, d);
        }
    }
    return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////
#endif // _MSC_VER

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


}

#ifdef HAVE_SIMGEAR
// we have simgear, so this can be compiled
void test_magvar()
{
    int yy = 13;
    int mm = 11;
    int dd = 13;
    double field[6];
    double var1, var2;
    /* Convert date to Julian day    1950-2049 */
    unsigned long jd = yymmdd_to_julian_days( yy, mm, dd );

    // KSFO - San Fran
    double lat1 = 37.61867421125;
    double lon1 = -122.37500760875;
    // LFPO - Orly, Paris
    double lat2 = 48.7269692925;
    double lon2 = 2.3699923175;

    /* return variation (in degrees) given geodetic latitude (radians), longitude
       (radians) ,height (km) and (Julian) date
        N and E lat and long are positive, S and W negative
    */
    var1 = calc_magvar( SGD_DEGREES_TO_RADIANS * lat1, SGD_DEGREES_TO_RADIANS * lon1, 0.0, jd, field );
#ifdef TEST_NHV_HACKS
    var2 = SGMagVarOrig( SGD_DEGREES_TO_RADIANS * lat1, SGD_DEGREES_TO_RADIANS * lon1, 0.0, jd, field );
    SPRTF( "\nKSFO: On jd %lu, lat %f, lon %f, has magnetic declination of %4.2f (%4.2f) degrees\n", jd,
        lat1, lon1,
        SGD_RADIANS_TO_DEGREES * var1, SGD_RADIANS_TO_DEGREES * var2 );

    var1 = calc_magvar( SGD_DEGREES_TO_RADIANS * lat2, SGD_DEGREES_TO_RADIANS * lon2, 0.0, jd, field );
    var2 = SGMagVarOrig( SGD_DEGREES_TO_RADIANS * lat2, SGD_DEGREES_TO_RADIANS * lon2, 0.0, jd, field );
    SPRTF( "LFPO: On jd %lu, lat %f, lon %f, has magnetic declination of %4.2f degrees (%4.2f)\n", jd,
        lat2, lon2, 
        var1 * SGD_RADIANS_TO_DEGREES, SGD_RADIANS_TO_DEGREES * var2);

#else
    SPRTF( "KSFO: On jd %lu, lat %f, lon %f, has magnetic declination of %0.2f degrees\n", jd,
        lat1, lon1,
        var1 * SGD_RADIANS_TO_DEGREES);
    var1 = calc_magvar( SGD_DEGREES_TO_RADIANS * lat2, SGD_DEGREES_TO_RADIANS * lon2, 0.0, jd, field );
    SPRTF( "LFPO: On jd %lu, lat %f, lon %f, has magnetic declination of %0.2f degrees\n", jd,
        lat2, lon2, 
        var1 * SGD_RADIANS_TO_DEGREES);

#endif

    SPRTF("\n");

}
#endif

// #endif // 0
void init() 
{
    pg = new global;
    int val = pg->foo();
    int val2 = pg->var;
    SPRTF("global var = %d, foo() returned %d\n", val2, val);

}

///////////////////////////////////////////////////////////////////////////

void test_floor_ceil()
{
    double lat = 37.618674211;
    double lon = 122.375007609;
    int ilat,ilon;
    int ilat2,ilon2;
    int ilat3,ilon3;
    if (lat < 0.0) {
        ilat = ceil(lat);
    } else {
        ilat = floor(lat);
    }
    if (lon < 0.0) {
        ilon = ceil(lon);
    } else {
        ilon = floor(lon);
    }
    ilat2 = floor(lat);
    ilon2 = floor(lon);
    ilat3 = ceil(lat);
    ilon3 = ceil(lon);
    SPRTF("For lat,lon %lf,%lf ilat,ilon %d,%d floor %d,%d ceil %d,%d\n", lat, lon, ilat, ilon, ilat2, ilon2, ilat3, ilon3 );
    lat = -37.618674211;
    lon = -122.375007609;
    if (lat < 0.0) {
        ilat = ceil(lat);
    } else {
        ilat = floor(lat);
    }
    if (lon < 0.0) {
        ilon = ceil(lon);
    } else {
        ilon = floor(lon);
    }
    ilat2 = floor(lat);
    ilon2 = floor(lon);
    ilat3 = ceil(lat);
    ilon3 = ceil(lon);
    SPRTF("For lat,lon %lf,%lf ilat,ilon %d,%d floor %d,%d ceil %d,%d\n", lat, lon, ilat, ilon, ilat2, ilon2, ilat3, ilon3 );
    exit(1);
}

///////////////////////////////////////////////////////////////////////////
// from : http://www.tenouk.com/clabworksheet/labworksheet10a.html
void array_test()
{
      int i, j, a[3][4] = {1,2,3,4,5,6,7,8,9,10,11,12};
      SPRTF("Array a[3][4] = {1,2,3,4,5,6,7,8,9,10,11,12};\n");
      for(i = 2; i >= 0; i = i - 1) {
            for(j = 3; j >= 0; j = j - 1) {
                  SPRTF("a[%d][%d] = %d\t", i, j, a[i][j]);
            }
            SPRTF("\n");
      }
      SPRTF("\n");
}

#ifdef HAVE_SIMGEAR  // indication simgear found

///////////////////////////////////////////////////////////////////////////
/* output
expected values are from : http://www.gpsvisualizer.com/calculators
From EGLL to KSFO Haversine = 8616.42, SG = 8638.65, expect 8638.65
From EGLL to LFPO Haversine = 365.44, SG = 365.87, expect 365.87
From LFPG to LFPO Haversine = 34.46, SG = 34.48, expect 34.48
 */

void test_dist()
{
    // EGLL - Heatrow
    double lat1 = 51.468301015;
    double lon1 = -0.457598533;
    // KSFO
    double lat2 = 37.618674211;
    double lon2 = -122.375007609;
    // expect 8638.65
    double dist1 = distance_km(lat1,lon1,lat2,lon2);
    double dist2 = SGDistance_km(lat1,lon1,lat2,lon2);
    SPRTF("\n");
    SPRTF("Run some distance tests comparing Haversine and SimGear (Vincenty) results.\n");

    SPRTF("From EGLL to KSFO Haversine = %.2lf, SG = %.2lf, expect 8638.65\n", dist1, dist2);
    // LFPO
    lat2 = 48.726969293;
    lon2 = 2.369992317;
    dist1 = distance_km(lat1,lon1,lat2,lon2);
    dist2 = SGDistance_km(lat1,lon1,lat2,lon2);
    SPRTF("From EGLL to LFPO Haversine = %.2lf, SG = %.2lf, expect 365.87\n", dist1, dist2);
    lat1 = 49.009742158;
    lon1 = 2.562619395;
    dist1 = distance_km(lat1,lon1,lat2,lon2);
    dist2 = SGDistance_km(lat1,lon1,lat2,lon2);
    SPRTF("From LFPG to LFPO Haversine = %.2lf, SG = %.2lf, expect 34.48\n", dist1, dist2);

    SPRTF("\n");

}

#endif // #ifdef HAVE_SIMGEAR  // indication simgear found

/////////////////////////////////////////////////
// 
void test_string()
{
    std::string s = "obsXXX"; // set up an observer

    // current code
    if (s.compare(0,3,"obs",3) == 0) {
        SPRTF("ok1: 'obs' found in '%s'\n",s.c_str());
    } else {
        SPRTF("NZ1: 'obs' not found in '%s'\n",s.c_str());
    }

    // merge request code
    if (s.compare(0,2,"obs") == 0) {
        SPRTF("ok2: 'obs' found in '%s'\n",s.c_str());
    } else {
        SPRTF("NZ2: 'obs' not found in '%s'\n",s.c_str());
    }

    // other tests
    // specify to only compare length 2
    if (s.compare(0,2,"obs",2) == 0) {
        SPRTF("ok3: 'ob' found in '%s'\n",s.c_str());
    } else {
        SPRTF("NZ3: 'ob' not found in '%s'\n",s.c_str());
    }
    // reduce the compare string to 2
    if (s.compare(0,2,"ob") == 0) {
        SPRTF("ok4: 'ob' found in '%s'\n",s.c_str());
    } else {
        SPRTF("NZ4: 'ob' not found in '%s'\n",s.c_str());
    }

    // modified merge request, but this only compares the first 2 chars
    if (s.compare(0,2,"obs",2) == 0) {
        SPRTF("ok5: 'ob' found in '%s'\n",s.c_str());
    } else {
        SPRTF("NZ5: 'ob' not found in '%s'\n",s.c_str());
    }
    exit(1);
}

#if 0
void test_round()
{
    double d = 5.67;
    int val = (int)round(d);
}
#endif

void show_time()
{
    // time_t values produced from the clock by time(NULL)
    time_t now = time(NULL);
#ifdef WIN32
    __time64_t now64 = _time64(NULL);
#endif
    // time_t values produced from y,m.d.h.m.s parts by mktime and timegm
    // time_t values are analysed into y,m,d,h,m,s by localtime() and gmtime()
    // time_t values are converted to readable string by ctime();
    char *cp = ctime(&now);
    SPRTF("%s: ctime = %s",module,cp);
#ifdef WIN32
    char *cp64 = _ctime64(&now64);
    SPRTF("%s: ctime64 = %s",module,cp64);
#endif


}


void show_sizes()
{
    const char *msg = "UNKNOWN";
    SPRTF("Size of char %lu, short %lu, int %lu, long %lu, long long %lu, uint64_t %lu\n", 
        sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(long long),
        sizeof(uint64_t));
    SPRTF("Size of float %lu, double %lu, long doube %lu, void * %lu, time_t %lu\n",
        sizeof(float), sizeof(double), sizeof(long double), sizeof(void *), sizeof(time_t));
    __int64 size;
    __int64 bytes = sizeof(size_t);

    if (bytes == sizeof(unsigned int)) {
       size = UINT_MAX;
       msg = "UINT_MAX";
    } else if (bytes == sizeof(unsigned long)) {
       size = ULONG_MAX;
       msg = "ULONG_MAX";
    } else if (bytes == sizeof(unsigned long long)) {
       size = ULLONG_MAX;
       msg = "ULLONG_MAX";
    }
    char *cps = get_I64_Stg(size);
    char *cpd = GetNxtBuf();
    nice_num(cpd,cps);
    SPRTF("size_t has a size of %s, thus can hold a max %s %s (%s)\n",
        get_I64_Stg(bytes), cpd, msg, get_k_num(size) );
    if (strcmp(msg,"ULLONG_MAX")) {
        cps = get_I64u_Stg(ULLONG_MAX);
        cpd = GetNxtBuf();
        nice_num(cpd,cps);
        SPRTF("A 64-bit unsigned long long max value %s (%s)\n",cpd,get_k_num(ULLONG_MAX,1));
    }
    // memory allocation limit for a 32-bit app on a 64-bit system
    // You should link your application with /LARGEADDRESSAWARE to make more 
    // than 2GB available to the application. Then you can use up to 4GB 
    // on a 64-bit OS in a 32-bit application.
    // if (i64 > 2000000000) {
    //   SPRTF("%s: Files size %s TOO big to allocate memory\n", module, get_I64_Stg( i64 ));
#if 0 // 000000000000000000000000000000000000000
#ifdef WIN32
    WIN32_FIND_DATA fd;
    uint64_t total = 0;
    int dcnt = 0;
    int fcnt = 0;
    HANDLE hf = FindFirstFile("*.*",&fd);
    if (hf && (hf != INVALID_HANDLE_VALUE)) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                dcnt++;
            } else {
                total += (((uint64_t)fd.nFileSizeHigh << 32) + fd.nFileSizeLow);
                fcnt++;
            }
        } while (FindNextFile(hf,&fd));
        FindClose(hf);
        SPRTF("%s: Found %d directories and %d files totaling %s bytes\n", module,
            dcnt, fcnt, get_I64u_Stg(total));
    }

#endif // WIN32
#endif // 0000000000000000000000000000000

}

static bool forever = true;
/* ------------------------------------------------------
   Signal values
   Seems windows ONLY implements a SMALL subset of signal values - from signal.h
/ * Signal types * /
#define SIGINT          2       / * interrupt * /
#define SIGILL          4       / * illegal instruction - invalid function image * /
#define SIGABRT_COMPAT  6       / * SIGABRT compatible with other platforms, same as SIGABRT * /
#define SIGFPE          8       / * floating point exception * /
#define SIGSEGV         11      / * segment violation * /
#define SIGTERM         15      / * Software termination signal from kill * /
#define SIGBREAK        21      / * Ctrl-Break sequence * /
#define SIGABRT         22      / * abnormal termination triggered by abort call * /

    The FULL unix list is given below - 1 to 30
   ------------------------------------------------------ */
void sigHandler( int sig )
{
    const char *msg = "uncased";
    forever = false;
    switch (sig)
    {
#if defined(_MSC_VER) // && defined(USE_WINDOWS_VALUES)
    case SIGINT:    //          2       /* interrupt */
        msg = "SIGINT - interrupt";
        break;
    case SIGILL:    //          4       /* illegal instruction - invalid function image */
        msg = "SIGILL - illegal";
        break;
    case SIGFPE:    //          8       /* floating point exception */
        msg = "SIGFPE - floating point";
        break;
    case SIGSEGV:   //         11      /* segment violation */
        msg = "SIGSEGV - segment";
        break;
    case SIGTERM:   //        15      /* Software termination signal from kill */
        msg = "SIGTERM - kill";
        break;
    case SIGBREAK:  //        21      /* Ctrl-Break sequence */
        msg = "SIGBREAK - Ctrl+break";
        break;
    case SIGABRT:   //         22      /* abnormal termination triggered by abort call */
        msg = "SIGABRT - abort";
        break;
    case SIGABRT_COMPAT:    //  6       /* SIGABRT compatible with other platforms, same as SIGABRT */
        msg = "SIGABRT_COMPAT = SIGABRT";
        break;
#else
    case  1: msg = "SIGHUP! "; break;
    case  2: msg = "SIGINT! "; break;
    case  3: msg = "SIGQUIT! "; break;
    case  4: msg = "SIGILL! "; break;
    case  5: msg = "SIGTRAP! "; break;
    case  6: msg = "SIGABRT! "; break;
    case  7: msg = "SIGBUS! "; break;
    case  8: msg = "SIGFPE! "; break;
    case  9: msg = "SIGKILL! "; break;
    case 10: msg = "SIGUSR1! "; break;
    case 11: msg = "SIGSEGV! "; break;
    case 12: msg = "SIGUSR2! "; break;
    case 13: msg = "SIGPIPE! "; break;
    case 14: msg = "SIGALRM! "; break;
    case 15: msg = "SIGTERM! "; break;
    case 16: msg = "SIGSTKFLT! "; break;
    case 17: msg = "SIGCHLD! "; break;
    case 18: msg = "SIGCONT! "; break;
    case 19: msg = "SIGSTOP! "; break;
    case 20: msg = "SIGTSTP! "; break;
    case 21: msg = "SIGTTIN! "; break;
    case 22: msg = "SIGTTOU! "; break;
    case 23: msg = "SIGURG! "; break;
    case 24: msg = "SIGXCPU! "; break;
    case 25: msg = "SIGXFSZ! "; break;
    case 26: msg = "SIGVTALRM! "; break;
    case 27: msg = "SIGPROF! "; break;
    case 28: msg = "SIGWINCH! "; break;
    case 29: msg = "SIGIO! "; break;
    case 30: msg = "SIGPWR! "; break;
#endif
    }
    forever = false;
    SPRTF("Signal %d %s\n", sig, msg );
}

#ifdef _MSC_VER
BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    const char *msg = "Uncased event";
    switch(CEvent)
    {
    case CTRL_C_EVENT:
        msg = "CTRL+C received!";
        break;
    case CTRL_BREAK_EVENT:
        msg = "CTRL+BREAK received!";
        break;
    case CTRL_CLOSE_EVENT:
        msg = "Program being closed!";
        break;
    case CTRL_LOGOFF_EVENT:
        msg = "User is logging off!";
        break;
    case CTRL_SHUTDOWN_EVENT:
        msg = "User is shutting down!";
        break;

    }
    SPRTF("ConsoleHandler: %s - removed handler\n", msg);
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler, FALSE );

    forever = false;
    return TRUE;
}

#endif

int millisleep(unsigned ms)
{
#if defined(WIN32)
  SetLastError(0);
  Sleep(ms);
  return GetLastError() ?-1 :0;
#elif defined(LINUX)
  return usleep(1000 * ms);
#else
#error ("no milli sleep available for platform")
  return -1;
#endif
}

void test_signals()
{
    unsigned long long val = 0;
	//
	// catch some signals
	//
#ifdef _MSC_VER
    signal(SIGABRT,  &sigHandler);
    signal(SIGTERM,  &sigHandler);
    signal(SIGINT,   &sigHandler);
    signal(SIGBREAK, &sigHandler);
#else
	signal (SIGINT,  sigHandler);
	signal (SIGHUP,  sigHandler);
	signal (SIGTERM, sigHandler);
	signal (SIGCHLD, sigHandler);
	signal (SIGPIPE, sigHandler);
#endif
#if (defined(_MSC_VER) && defined(ADD_HANDLER))
   if (SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler,TRUE)==FALSE) {
        // unable to install handler... 
        // display message to the user
        SPRTF("Unable to install handler!\n");
        return;
    }
#endif
    SPRTF("Try Ctrl+c, Ctrl+break, etc, to continue...\n");
    while (forever)
    {
        val++;
        if ((val % 10000) == 0) {
            SPRTF("Waiting Ctrl+c, Ctrl+break, etc, to continue...\n");
        }
        millisleep(55);
    }
    SPRTF("Continuing after %I64u cycles\n", val );
}


#define MAX_CNT 200
//#define strtoull _strtoi64
int test_getline()
{
    char *cp = 0;
    size_t len = 0;
    printf("Input a file name : ");
    size_t bytes = getline( &cp, &len, stdin );
    size_t total_bytes = 0;
    if (bytes == 0) {
        printf("No input file given!\n");
        return 0;
    }
    if (bytes != EOF) {
        if ( cp[bytes - 1] == '\n') {
            bytes--;
            cp[bytes] = 0;
        }
        
        //strtoull(buf);
        printf("Got name : '%s'\n",cp);
        if (is_file_or_directory64(cp) == DT_FILE) {
            uint64_t full_file = get_last_file_size64();
            FILE *fp = fopen(cp,"rb");
            if (fp) {
                int line_count = 0;
                int count = getline( &cp, &len, fp );
                while ( count != EOF) {
                    total_bytes += count;
                    if (count) {
                        if (cp[count -1] == '\n') {
                            line_count++;
                        }
                    }
                    count = getline( &cp, &len, fp );
                }
                printf("Counted %d lines in file. Approx. %d bytes... file total %llu\n", 
                    line_count, (int)total_bytes, full_file);
                fclose(fp);
            } else {
                printf("Can NOT open file!\n");
            }
        } else {
            printf("Can NOT 'stat' file!\n");
        }
    } else {
        printf("Got EOF!\n");
    }
    if (cp)
        free(cp);
    bytes = 0;
    return bytes;
}

//////////////////////////////////////////
// variable in <stdint.h>
int test_stdint()
{
    int argc = 1;
    int64_t i = 0;
    // NOTE: This cmake construction, used by include(CheckSymbolExists)
    // check_symbol_exists(int64_t stdint.h HAVE_INT64_T)
//#ifndef int64_t
//  return ((int*)(&int64_t))[argc];
//#else
  return 0;
//#endif

}


int main( int argc, char **argv )
{
    int iret = 0;
    SPRTF("Running %s\n", argv[0] );

    show_time();

    show_sizes();

    //test_open();
    //test_strtoimax();
    //test_scanf();
    //test_secs();

#ifdef _MSC_VER
    // test_fullpath();
    // test_redon();

#ifdef ADD_MMFIO_TEST
    //test_mmap();
#endif
#endif

    //test_tmpnam();

    //test_string_inc();

    //test_math();

    //test_codeset( 1000 );

#ifdef WIN32
    //test_wildcard();
#endif
    //test_getline();
    //test_signals();

    //test_string();
    //test_color();
    //test_floor_ceil();
#ifdef HAVE_SIMGEAR  // indication simgear found
    //test_intersect();
    //test_dist();
#endif // #ifdef HAVE_SIMGEAR  // indication simgear found

    //array_test();
    //test_map();
    //test_utf8();

    //init();
    //SPRTF("Protocol version %s\n", GetProtocolVerString().c_str());
#if (defined(HAVE_SIMGEAR) && defined(TEST_MAG_VER))
    test_magvar();
#endif
    //SPRTF("The file descriptor for stdin  (%p) is %d\n", stdin,  _fileno( stdin )  );
    //SPRTF("The file descriptor for stdout (%p) is %d\n", stdout, _fileno( stdout ) );
    //SPRTF("The file descriptor for stderr (%p) is %d\n", stderr, _fileno( stderr ) );

#if (defined(DO_POSTGRESQL_TEST) && defined(ADD_POSTGRESQL_TEST))
    // for MSVC debug set PATH=C:\Program Files (x86)\PostgreSQL\9.1\bin;%PATH%
    test_sql();
#endif
    //test_k_num();

    //test_gettimeofday();

    //test_wait_for();


    //test_compares();

#ifdef ADD_MPFR_TEST
    check_mpfr();
#endif // #ifdef ADD_MPFR_TEST
#ifdef ADD_CGAL_TEST
   cgal_main();
#endif
//#ifdef ADD_CF_LOG_TEST
//   iret = load_cf_log();
//#endif

//   iret = test_array();
    PRECT pr = 0;
   return iret;
}

// eof
