/*\
 * test-intersect.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdlib.h> // for exit(n)
#include <stdio.h>
#include "sprtf.hxx"
#include "test-intersect.hxx"

static const char *module = "test-intersect";

#ifdef HAVE_SIMGEAR  // indication simgear found

#include <simgear/constants.h>
#include <simgear/sg_inlines.h>
#include <simgear/math/SGMath.hxx>
#include <simgear/math/sg_geodesy.hxx>

/* lines_intersect:  AUTHOR: Mukesh Prasad
 *
 *   This function computes whether two line segments,
 *   respectively joining the input points (x1,y1) -- (x2,y2)
 *   and the input points (x3,y3) -- (x4,y4) intersect.
 *   If the lines intersect, the output variables x, y are
 *   set to coordinates of the point of intersection.
 *
 *   All values are in integers.  The returned value is rounded
 *   to the nearest integer point.
 *
 *   If non-integral grid points are relevant, the function
 *   can easily be transformed by substituting floating point
 *   calculations instead of integer calculations.
 *
 *   Entry
 *        x1, y1,  x2, y2   Coordinates of endpoints of one segment.
 *        x3, y3,  x4, y4   Coordinates of endpoints of other segment.
 *
 *   Exit
 *        x, y              Coordinates of intersection point.
 *
 *   The value returned by the function is one of:
 *
 *        DONT_INTERSECT    0
 *        DO_INTERSECT      1
 *        COLLINEAR         2
 *
 * Error conditions:
 *
 *     Depending upon the possible ranges, and particularly on 16-bit
 *     computers, care should be taken to protect from overflow.
 *
 *     In the following code, 'long' values have been used for this
 *     purpose, instead of 'int'.
 *
 */

#define	DONT_INTERSECT    0
#define	DO_INTERSECT      1
#define COLLINEAR         2

/**************************************************************
 *                                                            *
 *    NOTE:  The following macro to determine if two numbers  *
 *    have the same sign, is for 2's complement number        *
 *    representation.  It will need to be modified for other  *
 *    number systems.                                         *
 *                                                            *
 **************************************************************/

#define SAME_SIGNS( a, b )	(( ( a <= 0.0 ) && ( b <= 0.0 ) )||( ( a >= 0.0 ) && ( b >= 0.0 ) ))

int lines_intersect( double x1, double y1,   /* First line segment */
		     double x2, double y2,
             double x3, double y3,   /* Second line segment */
		     double x4, double y4,
             double *x, 
		     double *y )    /* Output value:
		                * point of intersection */
{
    double a1, a2, b1, b2, c1, c2; /* Coefficients of line eqns. */
    double r1, r2, r3, r4;         /* 'Sign' values */
    double denom, offset, num;     /* Intermediate values */

    SPRTF("%s: Solving x,y 1 %lf,%lf 2 %lf,%lf 3 %lf,%lf 4 %lf,%lf \n", module,
        x1, y1, x2, y2, x3, y3, x4, y4 );

    /* Compute a1, b1, c1, where line joining points 1 and 2
     * is "a1 x  +  b1 y  +  c1  =  0".
     */

    a1 = y2 - y1;
    b1 = x1 - x2;
    c1 = x2 * y1 - x1 * y2;

    /* Compute r3 and r4.
     */


    r3 = a1 * x3 + b1 * y3 + c1;
    r4 = a1 * x4 + b1 * y4 + c1;

    /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
     * same side of line 1, the line segments do not intersect.
     */

    if ( r3 != 0.0 &&
         r4 != 0.0 &&
         SAME_SIGNS( r3, r4 ))
        return ( DONT_INTERSECT );

    /* Compute a2, b2, c2 */

    a2 = y4 - y3;
    b2 = x3 - x4;
    c2 = x4 * y3 - x3 * y4;

    /* Compute r1 and r2 */

    r1 = a2 * x1 + b2 * y1 + c2;
    r2 = a2 * x2 + b2 * y2 + c2;

    /* Check signs of r1 and r2.  If both point 1 and point 2 lie
     * on same side of second line segment, the line segments do
     * not intersect.
     */

    if ( r1 != 0 &&
         r2 != 0 &&
         SAME_SIGNS( r1, r2 ))
        return ( DONT_INTERSECT );

    /* Line segments intersect: compute intersection point. 
     */

    denom = a1 * b2 - a2 * b1;
    if ( denom == 0.0 )
        return ( COLLINEAR );
    offset = (denom < 0.0) ? - denom / 2 : denom / 2;

    /* The denom/2 is to get rounding instead of truncating.  It
     * is added or subtracted to the numerator, depending upon the
     * sign of the numerator.
     */

    num = b1 * c2 - b2 * c1;
    *x = ( num < 0 ? num - offset : num + offset ) / denom;

    num = a2 * c1 - a1 * c2;
    *y = ( num < 0 ? num - offset : num + offset ) / denom;

    return ( DO_INTERSECT );
    
} /* lines_intersect */

/* A main program to test the function.
 */

/*\

C:\FG\17\test\build>Perl C:\Gtools\Perl\Findap03.pl  KSFO -S
1000 Version - data cycle 2013.10 (1000) file: F:/fgdata/Airports/apt.dat.gz

    13 KSFO San Francisco Intl       37.618674211,-122.375007609

rwy:4:
 10L: 37.62872250,-122.39342127 28R: 37.61351918,-122.35716907 b=117.8 l=3618 m. (s=Asphalt)
 10R: 37.62527727,-122.39074787 28L: 37.61169441,-122.35837447 b=117.8 l=3231.3 m. (s=Asphalt)
 01R: 37.60620279,-122.38114713 19L: 37.62781186,-122.36681630 b=27.8 l=2711.6 m. (s=Asphalt)
 01L: 37.60897310,-122.38223043 19R: 37.62719258,-122.37015433 b=27.8 l=2286 m. (s=Asphalt)

 rt:13 [APP: 120.35 134.5 135.65 ATIS: 118.85 135.45 118.05 CLR: 118.2 DEP: 120.9 135.1
       GRD: 121.8 128.65 TWR: 120.5 UNICOM: 122.95] fg=w130n30/w123n37/942050
See D:\Scenery\terrascenery\data\Scenery\Airports\K\S\F\KSFO.groundnet.xml

    13 KSFO San Francisco Intl (37.61867421125,-122.37500760875) (17)
Type  Latitude     Logitude        Alt.  Freq.  Range  Frequency2    ID  Name
VOR   37.61947222, -122.37388889,     7, 11580,    40,       17.0,  SFO, SAN FRANCISCO VOR-DME    (0.13Km on 48.1)
VDME  37.61948333, -122.37389167,     7, 11580,    40,        0.0,  SFO, SAN FRANCISCO VOR-DME    (0.13Km on 47.6)

 10L: 37.62872250,-122.39342127 28R: 37.61351918,-122.35716907 b=117.8 l=3618 m. (s=Asphalt)
 -------------------------------------------------------------------------------------------
GS    37.61395600, -122.36111400,     9, 11170,    10, 300297.903, IGWQ, KSFO 28R GS              (1.33Km on 113.1)
VDME  37.63017800, -122.39460800,    17, 11170,    18,      0.000, IGWQ, KSFO 28R DME-ILS         (2.15Km on 306.4)
ILS   37.62953900, -122.39531100,     5, 11170,    18,    297.903, IGWQ, KSFO 28R ILS-cat-III     (2.16Km on 303.9)
IM    37.61244200, -122.35454400,    13,     0,     0,    297.903, ----, KSFO 28R IM              (1.93Km on 110.9)
MM    37.60938600, -122.34727200,    13,     0,     0,    297.903, ----, KSFO 28R MM              (2.66Km on 112.8)

ILS   37.62129900, -122.36840200,    13, 11075,    18,    294.802, IFNP, KSFO 28R LDA-GS          (0.65Km on 63.5)
VDME  37.62129900, -122.36840200,    13, 11075,    18,      0.000, IFNP, KSFO 28R DME-ILS         (0.65Km on 63.5)
GS    37.61391800, -122.36114600,    13, 11075,    10, 300294.802, IFNP, KSFO 28R GS              (1.33Km on 113.3)
 -------------------------------------------------------------------------------------------

 01R: 37.60620279,-122.38114713 19L: 37.62781186,-122.36681630 b=27.8 l=2711.6 m. (s=Asphalt)
 -------------------------------------------------------------------------------------------
GS    37.62543600, -122.36958100,     7, 10890,    10, 295207.714, ISIA, KSFO 19L GS              (0.89Km on 32.6)
VDME  37.60519400, -122.38316700,    24, 10890,    18,      0.000, ISIA, KSFO 19L DME-ILS         (1.66Km on 205.7)
ILS   37.60451900, -122.38223600,    10, 10890,    18,    207.667, ISIA, KSFO 19L ILS-cat-I       (1.70Km on 202.1)
MM    37.63632500, -122.36115000,    13,     0,     0,    207.714, ----, KSFO 19L MM              (2.31Km on 32.0)
 -------------------------------------------------------------------------------------------

 10R: 37.62527727,-122.39074787 28L: 37.61169441,-122.35837447 b=117.8 l=3231.3 m. (s=Asphalt)
 -------------------------------------------------------------------------------------------
GS    37.61392200, -122.36113600,     9, 10955,    10, 300297.910, ISFO, KSFO 28L GS              (1.33Km on 113.3)
ILS   37.62656900, -122.39377800,     8, 10955,    18,    297.932, ISFO, KSFO 28L ILS-cat-I       (1.87Km on 297.9)
VDME  37.62609200, -122.39431700,    22, 10955,    18,      0.000, ISFO, KSFO 28L DME-ILS         (1.89Km on 295.8)
 -------------------------------------------------------------------------------------------

Frequency List: 11580 11075 10890 10955 11170 0

\*/

#ifdef USE_SG_GEODESY

char *geod2stg( SGGeod g )
{
    char *cp = GetNxtBuf();
    sprintf(cp,"%lf,%lf", g.getLatitudeDeg(),g.getLongitudeDeg());
    return cp;
}

int get_intersection( SGGeod LOC, double rw_brng, double rw_len_m,
    SGGeod GS, double *plat, double *plon )
{
    SGGeod loc_end, gs_end;
    int res;
    double az1, az2;
    double x1, y1, x2, y2, x3, y3, x4, y4;
    az1 = rw_brng + 180.0;
    if (az1 >= 360.0)
        az1 -= 360.0;
    res = geo_direct_wgs_84(LOC,az1,rw_len_m,loc_end,&az2);
    SPRTF("%s: LOC line %s to %s, on %lf, for %lf (%lf)\n", module,
        geod2stg(LOC),
        geod2stg(loc_end),
        az1,
        rw_len_m,
        az2 );
    az1 = rw_brng + 90.0;
    if (az1 >= 360.0)
        az1 -= 360.0;
    res = geo_direct_wgs_84(GS,az1,rw_len_m,gs_end,&az2);
    SPRTF("%s: GS line %s to %s, on %lf, for %lf (%lf)\n", module,
        geod2stg(GS),
        geod2stg(gs_end),
        az1,
        rw_len_m,
        az2 );

    x1 = LOC.getLongitudeDeg();
    y1 = LOC.getLatitudeDeg(), 
    x2 = loc_end.getLongitudeDeg(),
    y2 = loc_end.getLatitudeDeg(),
    x3 = GS.getLongitudeDeg();
    y3 = GS.getLatitudeDeg();
    x4 = gs_end.getLongitudeDeg();
    y4 = gs_end.getLatitudeDeg();
    res = lines_intersect( x1, y1, x2, y2, x3, y3, x4, y4, plat, plon );
    //res = lines_intersect( LOC.getLongitudeDeg(), LOC.getLatitudeDeg(), 
    //    loc_end.getLongitudeDeg(),
    //    loc_end.getLatitudeDeg(),
    //    GS.getLongitudeDeg(),
    //    GS.getLatitudeDeg(),
    //    gs_end.getLongitudeDeg(), 
    //    gs_end.getLatitudeDeg(),
    //    plat, plon );
    return res;
}

void show_dist( double lat1, double lon1, double lat2, double lon2 )
{
    SGGeod geod1;
    SGGeod geod2;
    int res;
    double az1,az2,dist;
    geod1.setLatitudeDeg(lat1);
    geod1.setLongitudeDeg(lon1);
    geod1.setElevationM(0.0);

    geod2.setLatitudeDeg(lat2);
    geod2.setLongitudeDeg(lon2);
    geod2.setElevationM(0.0);

    res = geo_inverse_wgs_84(geod1,geod2,&az1,&az2,&dist);
    SPRTF("%s: From lat,lon %s to %s: dist %lf, on %lf (%lf)\n", module,
        geod2stg(geod1),
        geod2stg(geod2),
        dist,
        az1,
        az2 );


}

void test_intersect_sg()
{
    double x1, x2, x3, x4, y1, y2, y3, y4;
    double x, y;
    double az1,az2,dist,rwlen,rwbrg,rwopp;
    int res;
    SPRTF("\n");
    SPRTF("%s: doing test intersect simgear...\n", module );
    // runway = LOC - sort of...
    // 10L: 
    y1 = 37.62872250;
    x1 = -122.39342127;
    // 28R: 
    y2 = 37.61351918;
    x2 = -122.35716907;
    // GS    37.61395600, -122.36111400,     9, 11170,    10, 300297.903, IGWQ, KSFO 28R GS (1.33Km on 113.1)
    SGGeod geod1;
    SGGeod geod2;
    //geod1.fromDeg(x1,y1);
    //geod2.fromDeg(x2,y2);
    geod1.setLatitudeDeg(y1);
    geod1.setLongitudeDeg(x1);
    geod1.setElevationM(0.0);

    geod2.setLatitudeDeg(y2);
    geod2.setLongitudeDeg(x2);
    geod2.setElevationM(0.0);

    res = geo_inverse_wgs_84(geod1,geod2,&az1,&az2,&dist);
    rwlen = ((int)((dist+0.05) * 10.0) / 10.0);
    rwbrg = az2;
    rwopp = az1;
    // test-intersect: From lat,lon 37.628723,-122.393421 to 37.613519,-122.357169, 3618.024738 meters on 117.789072 (297.811202)
    SPRTF("%s: From lat,lon %lf,%lf to %lf,%lf, %lf meters on %lf (%lf)\n", module,
        y1,x1,y2,x2,rwlen,rwbrg,az1);

    SGGeod LOC;
    SGGeod GS;
    // GS    37.61395600, -122.36111400,     9, 11170,    10, 300297.903, IGWQ, KSFO 28R GS              (1.33Km on 113.1)
    // VDME  37.63017800, -122.39460800,    17, 11170,    18,      0.000, IGWQ, KSFO 28R DME-ILS         (2.15Km on 306.4)
    LOC.setLatitudeDeg(37.63017800);
    LOC.setLongitudeDeg(-122.39460800);
    LOC.setElevationM(0.0);

    y3 = 37.61395600;
    x3 = -122.36111400;
    GS.setLatitudeDeg(y3);
    GS.setLongitudeDeg(x3);
    GS.setElevationM(0.0);
    res = geo_inverse_wgs_84(LOC,GS,&az1,&az2,&dist);
    SPRTF("%s: From lat,lon %s to %s, dist %lf, az %lf (%lf)\n", module,
        geod2stg(LOC),
        geod2stg(GS),
        dist,
        az2, az1 );
    az1 = rwbrg + 90.0;
    if (az1 >= 360.0) 
        az1 -= 360.0;
    SGGeod dest;
    res = geo_direct_wgs_84(GS,az1,rwlen,dest,&az2);
    SPRTF("%s: From lat,lon %s, on %lf, for %lf, is %s (az=%lf)\n", module,
        geod2stg(GS),
        az1,
        rwlen,
        geod2stg(dest),
        az2 );
    x4 = dest.getLongitudeDeg();
    y4 = dest.getLatitudeDeg();

    double lat,lon;
    bool ok = false;
    res = lines_intersect( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y );
    switch (res) {
    case DONT_INTERSECT:
		SPRTF( "Lines don't intersect\n" );
		break;
    case COLLINEAR:
        SPRTF( "Lines are collinear\n" );
        break;
    case DO_INTERSECT:
		SPRTF( "Lines intersect at lat,lon %lf,%lf\n", y, x );
        lat = y;
        lon = x;
        ok = true;
        break;
    }

    res = get_intersection( LOC, rwbrg, rwlen, GS, &x, &y );
    switch (res) {
    case DONT_INTERSECT:
		SPRTF( "Lines don't intersect\n" );
		break;
    case COLLINEAR:
        SPRTF( "Lines are collinear\n" );
        break;
    case DO_INTERSECT:
		SPRTF( "Lines intersect at lat,lon %lf,%lf\n", y, x );
        if (ok) {
            show_dist(lat,lon,y,x);
        }
        break;
    }

#if 0 // 00000000000000000000000000000
    for (;;) {
        printf( "X1, Y1: " );
        scanf( "%lf %lf", &x1, &y1 );
        printf( "X2, Y2: " );
        scanf( "%lf %lf", &x2, &y2 );
        printf( "X3, Y3: " );
        scanf( "%lf %lf", &x3, &y3 );
        printf( "X4, Y4: " );
        scanf( "%lf %lf", &x4, &y4 );
        res = lines_intersect( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y );
        switch (res) {
        case DONT_INTERSECT:
			printf( "Lines don't intersect\n" );
			break;
        case COLLINEAR:
            printf( "Lines are collinear\n" );
            break;
        case DO_INTERSECT:
			 printf( "Lines intersect at %lf,%lf\n", x, y );
             break;
        }
    }
#endif // 00000000000000000000000000
    SPRTF("%s: end test intersect simgear...\n", module );


} /* main */
#endif // #ifdef USE_SG_GEODESY

#endif // #ifdef HAVE_SIMGEAR

// from : http://stackoverflow.com/questions/217578/point-in-polygon-aka-hit-test
/* ----------------------------------------------------------------------------
    The easiest way is to use ray casting, since it can handle all the polygons, 
    no special handling is necessary and it still provides good speed. 
    The idea of the algorithm is pretty simple: 
    Draw a virtual ray from anywhere outside the polygon to your point and count 
    how often it hits any side of the polygon. 
    If the number of hits is even, it's outside of the polygon, if it's odd, it's inside.
  ----------------------------------------------------------------------------- */
#define RET_NO 0
#define RET_YES 1
#define RET_COLLINEAR 2

int areIntersecting(
    double v1x1, double v1y1, double v1x2, double v1y2,
    double v2x1, double v2y1, double v2x2, double v2y2)
{
    double d1, d2;
    double a1, a2, b1, b2, c1, c2;

    // Convert vector 1 to a line (line 1) of infinite length.
    // We want the line in linear equation standard form: A*x + B*y + C = 0
    // See: http://en.wikipedia.org/wiki/Linear_equation
    a1 = v1y2 - v1y1;
    b1 = v1x1 - v1x2;
    c1 = (v1x2 * v1y1) - (v1x1 * v1y2);

    // Every point (x,y), that solves the equation above, is on the line,
    // every point that does not solve it, is either above or below the line.
    // We insert (x1,y1) and (x2,y2) of vector 2 into the equation above.
    d1 = (a1 * v2x1) + (b1 * v2y1) + c1;
    d2 = (a1 * v2x2) + (b1 * v2y2) + c1;

    // If d1 and d2 both have the same sign, they are both on the same side of
    // our line 1 and in that case no intersection is possible. Careful, 0 is
    // a special case, that's why we don't test ">=" and "<=", but "<" and ">".
    if (d1 > 0 && d2 > 0) return RET_NO;
    if (d1 < 0 && d2 < 0) return RET_NO;

    // We repeat everything above for vector 2.
    // We start by calculating line 2 in linear equation standard form.
    a2 = v2y2 - v2y1;
    b2 = v2x1 - v2x2;
    c2 = (v2x2 * v2y1) - (v2x1 * v2y2);

    // Calulate d1 and d2 again, this time using points of vector 1
    d1 = (a2 * v1x1) + (b2 * v1y1) + c2;
    d2 = (a2 * v1x2) + (b2 * v1y2) + c2;

    // Again, if both have the same sign (and neither one is 0),
    // no intersection is possible.
    if (d1 > 0 && d2 > 0) return RET_NO;
    if (d1 < 0 && d2 < 0) return RET_NO;

    // If we get here, only three possibilities are left. Either the two
    // vectors intersect in exactly one point or they are collinear
    // (they both lie both on the same infinite line), in which case they
    // may intersect in an infinite number of points or not at all.
    if ((a1 * b2) - (a2 * b1) == 0.0f) return RET_COLLINEAR;

    // If they are not collinear, they must intersect in exactly one point.
    return RET_YES;
}

// Polgon (30.4998635139406,122.013360248033,29.5337881864316,122.320684676771,30.5997309223615,122.255392915135,29.7812282643568,122.921499744166)
// BBOX = 122.013360248033,29.5337881864316,122.921499744166,30.5997309223615
void test_intersect_nosg()
{


}


void test_intersect()
{
#if (defined(HAVE_SIMGEAR) && defined(USE_SG_GEODESY)) // indication simgear found
    test_intersect_sg();
#endif
    test_intersect_nosg();
}
// eof = test-intersect.cxx
