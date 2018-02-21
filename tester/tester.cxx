// tester.cxx
// ==========

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include <cassert>
#include <stdio.h>
#include <stdlib.h>  // for exit()
#include <stdint.h> // for unit64_t, ...
#include <math.h>
//#include <cmath>
#include <limits.h>
#include <string.h> // for strcmp(), ...
#include <time.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <pwd.h> // for getpwuid(), ...
#endif
#include <string>   // std::string, ...
// Note, ugly special location
#include "../src/sprtf.hxx"
#include "../src/utils.hxx"

#ifndef VERSION
#define VERSION "3.0.2"
#endif
#ifndef T_VERS
#define T_VERS __TIME__
#define D_VERS __DATE__
#endif


#ifndef SPRTF
#define SPRTF printf
#endif
#ifndef WIN32
#include <unistd.h> // getpid, ...
#ifndef LINUX
#define LINUX
#endif
#endif

#ifndef SPRINTF
#define SPRINTF sprintf
#endif
#ifndef STRCAT
#define STRCAT strcat
#endif
#ifndef STRLEN
#define STRLEN strlen
#endif

static const char *def_log = "temptester.txt";

///////////////////////////////////////////////////////////////////
#ifndef SPRTF
char *get_I64_Stg( long long val )
{
    char *cp = GetNxtBuf();
#ifdef _MSC_VER
    sprintf(cp,"%I64d",val);
#else
    sprintf(cp,"%lld",val);
#endif
    return cp;
}
char *get_I64u_Stg( unsigned long long val )
{
    char *cp = GetNxtBuf();
#ifdef _MSC_VER
    sprintf(cp,"%I64u",val);
#else
    sprintf(cp,"%llu",val);
#endif
    return cp;
}

/* ======================================================================
   nice_num = get nice number, with commas
   given a destination buffer,
   and a source buffer of ascii
   NO CHECK OF LENGTH DONE!!! assumed destination is large enough
   and assumed it is a NUMBER ONLY IN THE SOURCE
   ====================================================================== */
void nice_num( char * dst, char * src ) // get nice number, with commas
{
   size_t i;
   size_t len = strlen(src);
   size_t rem = len % 3;
   size_t cnt = 0;
   for( i = 0; i < len; i++ )
   {
      if( rem ) {
         *dst++ = src[i];
         rem--;
         if( ( rem == 0 ) && ( (i + 1) < len ) )
            *dst++ = ',';
      } else {
         *dst++ = src[i];
         cnt++;
         if( ( cnt == 3 ) && ( (i + 1) < len ) ) {
            *dst++ = ',';
            cnt = 0;
         }
      }
   }
   *dst = 0;
}

void trim_float_buf( char *pb )
{
   size_t len = STRLEN(pb);
   size_t i, dot, zcnt;
   for( i = 0; i < len; i++ )
   {
      if( pb[i] == '.' )
         break;
   }
   dot = i + 1; // keep position of decimal point (a DOT)
   zcnt = 0;
   for( i = dot; i < len; i++ )
   {
      if( pb[i] != '0' )
      {
         i++;  // skip past first
         if( i < len )  // if there are more chars
         {
            i++;  // skip past second char
            if( i < len )
            {
               size_t i2 = i + 1;
               if( i2 < len )
               {
                  if ( pb[i2] >= '5' )
                  {
                     if( pb[i-1] < '9' )
                     {
                        pb[i-1]++;
                     }
                  }
               }
            }
         }
         pb[i] = 0;
         break;
      }
      zcnt++;     // count ZEROS after DOT
   }
   if( zcnt == (len - dot) )
   {
      // it was ALL zeros
      pb[dot - 1] = 0;
   }
}

#endif


char *get_k_num( uint64_t i64, int type = 0, int dotrim = 1 );

char *get_k_num( uint64_t i64, int type, int dotrim )
{
   char *pb = GetNxtBuf();
   const char *form = " bytes";
   uint64_t byts = i64;
   double res;
   const char*ffm = "%0.20f";  // get 20 digits
   if( byts < 1024 ) {
      SPRINTF(pb, "%llu", byts);
      dotrim = 0;
   } else if( byts < 1024*1024 ) {
      res = ((double)byts / 1024.0);
      form = (type ? " KiloBypes" : " KB");
      SPRINTF(pb, ffm, res);
   } else if( byts < 1024*1024*1024 ) {
      res = ((double)byts / (1024.0*1024.0));
      form = (type ? " MegaBypes" : " MB");
      SPRINTF(pb, ffm, res);
   } else { // if( byts <  (1024*1024*1024*1024)){
      double val = (double)byts;
      double db = (1024.0*1024.0*1024.0);   // x3
      double db2 = (1024.0*1024.0*1024.0*1024.0);   // x4
      if( val < db2 )
      {
         res = val / db;
         form = (type ? " GigaBypes" : " GB");
         SPRINTF(pb, ffm, res);
      }
      else
      {
         db *= 1024.0;  // x4
         db2 *= 1024.0; // x5
         if( val < db2 )
         {
            res = val / db;
            form = (type ? " TeraBypes" : " TB");
            SPRINTF(pb, ffm, res);
         }
         else
         {
            db *= 1024.0;  // x5
            db2 *= 1024.0; // x6
            if( val < db2 )
            {
               res = val / db;
               form = (type ? " PetaBypes" : " PB");
               SPRINTF(pb, ffm, res);
            }
            else
            {
               db *= 1024.0;  // x6
               db2 *= 1024.0; // x7
               if( val < db2 )
               {
                  res = val / db;
                  form = (type ? " ExaBypes" : " EB");
                  SPRINTF(pb, ffm, res);
               }
               else
               {
                  db *= 1024.0;  // x7
                  res = val / db;
                  form = (type ? " ZettaBypes" : " ZB");
                  SPRINTF(pb, ffm, res);
               }
            }
         }
      }
   }
   if( dotrim > 0 )
      trim_float_buf(pb);

   STRCAT(pb, form);

   //if( ascii > 0 )
   //   Convert_2_ASCII(pb);

   return pb;
}

///////////////////////////////////////////////////////////////////

int my_round(double d)
{
	int res;
	if (d < 0.0) {
	   res = ceil( d );
	   if ( ( (double) res - d ) > 0.5 )
		res--;
	} else {
	   res = floor( d );
	   if ( ( d - (double) res ) > 0.5 )
		res++;
	}
	return res;
}

void test_round()
{
	double v1 = 8.567;
	double v2 = -8.567;
#ifdef WIN32
	int iv1 = (int)(v1 + 0.5);
	int iv2 = (int)(v2 + 0.5);
#else
	int iv1 = (int) round(v1);
	int iv2 = (int) round(v2);
#endif
	int cv1 = ceil(v1);
	int fv1 = floor(v1);
	int cv2 = ceil(v2);
	int fv2 = floor(v2);
	int mv1 = my_round(v1);
	int mv2 = my_round(v2);

	printf("val1=%lf round=%d(%d), ceil=%d, floor=%d val2=%lf round=%d(%d), ceil=%d, floor=%d\n", v1, iv1, mv1, cv1, fv1, v2, iv2, mv2, cv2, fv2 );

	v1 = 8.467;
	v2 = -8.467;
#ifdef WIN32
	iv1 = (int)(v1 + 0.5);
	iv2 = (int)(v2 + 0.5);
#else
	iv1 = (int) round(v1);
	iv2 = (int) round(v2);
#endif
	cv1 = ceil(v1);
	fv1 = floor(v1);
	cv2 = ceil(v2);
	fv2 = floor(v2);
	mv1 = my_round(v1);
	mv2 = my_round(v2);

	printf("val1=%lf round=%d(%d), ceil=%d, floor=%d val2=%lf round=%d(%d), ceil=%d, floor=%d\n", v1, iv1, mv1, cv1, fv1, v2, iv2, mv2, cv2, fv2 );
}

static void show_sizes()
{
    const char *msg = "UNKNOWN";
    SPRTF("Size of char %lu, short %lu, int %lu, long %lu, long long %lu, uint64_t %lu\n", 
        sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(long long),
        sizeof(uint64_t));
    SPRTF("Size of float %lu, double %lu, long doube %lu, void * %lu, time_t %lu\n",
        sizeof(float), sizeof(double), sizeof(long double),
        sizeof(void *), sizeof(time_t));
    uint64_t size;
    uint64_t bytes = sizeof(size_t);

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
}

/* Possible access modes... one for each octal value. */
static char * accesses[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};

void trim_buffer( char *cp )
{
   size_t ii, max;
   if (!cp)
    return;
   max = strlen(cp);
   if (!max)
    return;
   for (ii = max - 1; ii > 0; ii--) {
        if (cp[ii] > ' ')
            break;
        cp[ii] = 0;
   }

}

#ifndef WIN32
// -rw-rw-r-- 1 geoff geoff  1189 Sep  1 17:53 bldlog-1.txt
void report( char * name, struct stat * buffer)
{
    int i;
    char *cp;
    struct passwd * passent;
    unsigned short m = buffer->st_mode;
    
    if (S_ISREG(m))
        printf("-");    // file
    else if S_ISDIR(m)
        printf("d");    // directory?
    else if S_ISCHR(m)
        printf("c");    // character device?
    else if S_ISBLK(m)
        printf("b");    // block device?
    else if S_ISFIFO(m)
        printf("p");    // FIFO (named pipe)?
    else if S_ISLNK(m)
        printf("l");    // symbolic link? (Not in POSIX.1-1996.)
    else
        printf("?");    // S_ISSOCK(m) 
    for(i = 6; i >= 0; i -= 3)
        printf("%s", accesses[(m >> i) & 7]);
    printf(" ");
    
    passent = getpwuid(buffer->st_uid);
    if(passent != NULL)
        printf("%-12s ", passent->pw_name);
    else
        printf("%-12s ", "unknown");
    printf("%8ld ", buffer->st_size);
        
    cp = ctime( &(buffer->st_atime));
    trim_buffer(cp);
    printf("%s ", cp);
    
    printf("%s\n", name);

}


/* Decode and present the status information. */
void report_full( char * name, struct stat * buffer)
{
    int i;
    char *cp;
    struct passwd * passent;
    ushort mode = buffer->st_mode;
    // printf("Item name '%s' :\n", name);
    
    cp = ctime( &(buffer->st_atime));
    trim_buffer(cp);
    printf(" Last access : %s", cp);
    if (buffer->st_atime != buffer->st_mtime) {
        cp = ctime( &(buffer->st_mtime));
        trim_buffer(cp);
        printf(" mod : %s", cp);
    }
    if ((buffer->st_atime != buffer->st_ctime)&&(buffer->st_mtime != buffer->st_ctime) ) {
        cp = ctime( &(buffer->st_ctime));
        trim_buffer(cp);
        printf(" change : %s", cp);
    }
    printf("\n");
    printf(" Current file size : %ld", buffer->st_size);

    /* type dev_t is int*/
    printf(" device %d", buffer->st_dev);

    /* type ino_t is unsigned  */
    printf(" Inode %lu\n", buffer->st_ino);

    /* Identify the owner by number and by name. */
    passent = getpwuid(buffer->st_uid);
    if(passent != NULL)
        printf(" Owner of the file is #%d - %s\n", buffer->st_uid, passent->pw_name);
    else
        printf(" Owner of the file is #%d - unknown\n", buffer->st_uid);

    printf(" Access mode 0%o: ", mode);
    for(i = 6; i >= 0; i -=3)
        printf("%s", accesses[(mode >> i) & 7]);
    printf("\n");
}

#endif

static struct stat buf;

#ifdef MAC
std::string get_basepath() {
    std::string path = "./";
    ProcessSerialNumber PSN;
    ProcessInfoRec pinfo;
    FSSpec pspec;
    FSRef fsr;
    OSStatus err;
    /* set up process serial number */
    PSN.highLongOfPSN = 0;
    PSN.lowLongOfPSN = kCurrentProcess;
    /* set up info block */
    pinfo.processInfoLength = sizeof(pinfo);
    pinfo.processName = NULL;
    pinfo.processAppSpec = &pspec;
    /* grab the vrefnum and directory */
    err = GetProcessInformation(&PSN, &pinfo);
    if (! err ) {
        char c_path[2048];
        FSSpec fss2;
        int tocopy;
        err = FSMakeFSSpec(pspec.vRefNum, pspec.parID, 0, &fss2);
        if ( ! err ) {
            err = FSpMakeFSRef(&fss2, &fsr);
            if ( ! err ) {
                char c_path[2049];
                err = (OSErr)FSRefMakePath(&fsr, (UInt8*)c_path, 2048);
                if (! err ) {
                    path = c_path;
                }
            }
        }
    }
    return (path);
}
#else // !MAC
#ifdef LINUX
std::string get_basepath() 
{
    std::string path = "";
    pid_t pid = getpid();
    char buf[10];
    sprintf(buf,"%d",pid);
    std::string _link = "/proc/";
    _link.append( buf );
    _link.append( "/exe");
    char proc[512];
    int ch = readlink(_link.c_str(),proc,512);
    if (ch != -1) {
        proc[ch] = 0;
        path = proc;
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
    }
    return (path);
}
#else // !LINUX
#ifdef WIN32
std::string get_basepath() 
{
    std::string path = "";
    char buf[264];
    char *lpb = buf;
    int dwd = GetModuleFileName( NULL, lpb, 256 );
    char *p = strrchr( lpb, '\\' );
    if( !p )
       p = lpb;
   *p = 0;
   path = lpb;
   return path;
}
#else // !WIN32
#error "Need macro and port for this OS
#endif // WIN32 y/n
#endif // LINUX y/n
#endif // MAC y/n

void show_proc_path()
{
    std::string path = get_basepath();
    printf("Base path: '%s'\n", path.c_str());
}

void test_pos()
{
#ifndef WIN32
    // this NOT supported in Windows!!!
    printf("Pos 1: %1$s, Pos 2: %2$s\n", "one", "two");
    printf("Pos 2: %2$s, Pos 1: %1$s\n", "one", "two");
#endif
}

int main( int argc, char ** argv )
{
    int   iret = 0;
    int   i;
    char * arg = NULL;
    set_log_file((char *)def_log, false);
    printf("Running %s... v.%s, compiled %s, at %s\n", argv[0], VERSION, D_VERS, T_VERS);
    show_proc_path();
    show_sizes();
    if (argc > 1) {
        printf("With %d arguments...\n", argc - 1);
        for (i = 1; i < argc; i++) {
            arg = argv[i];
            if (stat(arg,&buf) == 0) {
#ifdef WIN32
                printf( "%2d: [%s] stat succeeded, so...\n", i, arg );
#else
                report( arg, &buf );
#endif
            } else {
                printf( "%2d: [%s] (address:%p)\n", i, arg, arg );
            } 
        }
   } else {
      printf("With no arguments...\n");
   }
   arg = NULL;

    float foo = 3.1415;
    const char *fmt = "Float 3.1415 [%.3f] ";
    printf(fmt, foo);
    printf("[%f]\n",foo);
    //printf(" Testing a MACRO...\n" );
   //assert( arg != NULL );
   //ATLAS_MACRO( arg != NULL );
	//test_round();
	test_pos();
   return iret;
}

// eof - tester.cxx
