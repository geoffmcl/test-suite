// magvar.cxx

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h> // for strlen() in windows

#include <simgear/constants.h>
#include "coremag.hxx"

static int verbose = 0;
static double deg_lat = 9999.0;
static double deg_lon = 9999.0;
static double alt_m = 0.0; // TODO: Check should this be meters??? maybe km???
static int year = 13;
static int month = 11;
static int day = 13;

//char *Get_UTC_Time_Stg(time_t Timestamp)
void set_utc_time()
{
    //char *ps = GetNxtBuf();
    time_t Timestamp = time(0);
    tm  *ptm;
    ptm = gmtime (& Timestamp);
    //sprintf (
    //    ps,
    //    ts_form,
    year = ptm->tm_year - 100; //+1900,
    month = ptm->tm_mon+1;
    day = ptm->tm_mday;
    //    ptm->tm_hour,
    //    ptm->tm_min,
    //    ptm->tm_sec );
    //return ps;
}

int in_world_range( double lat, double lon )
{
    if ((lat > 90.0) || (lat < -90.0) ||
        (lon > 180.0) || (lon < -180.0)) {
            return 0;
    }
    return 1;
}

void give_help()
{
    int c;
    printf("\nmagvar [options] lat_degs lon_degs [altitude mont day year]\n");
    printf("options:\n");
    printf(" --help, -h, -? = This help and exit(2)\n");
    printf(" --verb, -v     = Set verbosity.\n");
    printf("\n");
    printf("Will calculate and print the magnetic variation of the location\n");
    printf("If verbosity set, will show paraameters used, and julian date as well.\n");
    printf("If no altitude is given, then zero will be used\n");
    printf("If no month day year given, then current time UTC will be used.\n");
    printf("The year must be in the range 0-49 for 2000-2049, or 50-99 for 1950-1999.\n");
    printf("Exit(2) with this help, exit(1) if some other error,\n");
    printf("else output the variation in degrees, and exit 0.\n");
    printf("Use '.'+Enter keys to continue...\n");
    while ((c = getchar()) != '.')
        putchar(c);
    
}

#define ISNUM(a) (( a >= '0' )&&( a <= '9' ))

int is_a_float( char *num )
{
    size_t len = strlen(num);
    size_t ii;
    int c, dcnt = 0;
    for( ii = 0; ii < len; ii++ ) {
        c = num[ii];
        if (ISNUM(c)) continue;
        if ((c == '-')||( c == '+')) {
            // accepted ONLY as first char
            if (ii)
                return 0;
        } else if (c == '.') {
            if (dcnt)
                return 0;
            dcnt++;
        } else {
            // not number, sign nor '.'
            return 0;
        }
    }
    return 1; // looks like one
}

int parse_args(int argc, char **argv)
{
    int i, cnt, c;
    char *arg;
    char *sarg;
    cnt = 0;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        if ((*arg == '-') && !is_a_float(arg)) {
            sarg = &arg[1];
            while (*sarg == '-') sarg++;
            c = *sarg;
            switch (c)
            {
            case '?':
            case 'h':
                give_help();
                return 2;
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                printf("Unknown argument %s\n", arg);
                return 1;
            }
        } else {
            switch (cnt)
            {
            case 0:
                deg_lat = atof(arg);
                break;
            case 1:
                deg_lon = atof(arg);
                break;
            case 2:
                alt_m = atof(arg);
                break;
            case 3:
                month = atoi(arg);
                break;
            case 4:
                day = atoi(arg);
                break;
            case 5:
                year = atoi(arg);
                break;
            default:
                printf("Unknown argument %s!. Have already had lat %f, lon %f, alt %f, month %d, day %d, year %d!\n",
                    arg, deg_lat, deg_lon, alt_m, month, day, year );
                return 1;
            }
            cnt++;
        }
    }
    // some quick range checks
    if (deg_lon == 9999.0) {
        printf("Failed to find latitude longitude in the command!\n");
        give_help();
        return 1;
    }
    if ( !in_world_range(deg_lat, deg_lon)) {
        printf("Yeak, apppears lat %f lon %f are not of this world!\n", deg_lat, deg_lon);
        return 1;
    }
    if ((year < 0) || (year > 99)) {
        printf("The year must be in the range 0-49 for 2000-2049, or 50-99 for 1950-1999, not %d!\n", year);
        return 1;
    }
    if ((month < 1)||(month > 12)) {
        printf("Month must be in range 1 - 12! Not %d.\n", month );
        return 1;
    }
    if ((day < 1) || (day > 31)) {
        printf("Day must be in range 1 - 31! Not %d.\n", day );
        return 1;
    }

    return 0; // all appears OK
}


int main( int argc, char **argv )
{
    set_utc_time(); // set default of today
    int iret = parse_args(argc,argv);
    if (iret)
        return iret;
    double field[6];
    long jd = yymmdd_to_julian_days( year, month, day );
    double var = calc_magvar( SGD_DEGREES_TO_RADIANS * deg_lat, 
        SGD_DEGREES_TO_RADIANS * deg_lon, alt_m,
        jd,
        field );
    if (verbose) {
        printf("For location lat %f, lon %f, alt %f, at month %d, day %d, year %d,\n", deg_lat, deg_lon, alt_m, month, day, 
                (year < 50) ? (2000 + year) : (1900 + year) );
        printf("calculated magnetic variation %4.2f\n", SGD_RADIANS_TO_DEGREES * var );
    } else {
        printf("%4.2f", SGD_RADIANS_TO_DEGREES * var );
    }
	return iret;
}

// eof

