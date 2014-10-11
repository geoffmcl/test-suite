/*\
 * test-area.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <cmath>
#include "test-math.hxx"    // for M_PI, and DEG2RAD/RAD2DEG
#include "sprtf.hxx"
#include "test-area.hxx"

static const char *module = "test-area";

#define RADa 6378.137       /* semi-major axis (equatorial radius) of WGS84 ellipsoid (km) */
#define RADb 6356.7523142   /* semi-minor axis referenced to the WGS84 ellipsoid (km) */
#define RADm 6371;          // mean radius in km - set return units

/////////////////////////////////////////////////////////////////////////////
// Example 1
// from : http://forum.worldwindcentral.com/showthread.php?20724-A-method-to-compute-the-area-of-a-spherical-polygon
// BUT there are comments
// what do you mean by "the code is unusable in numerical work"?
// My judgment is based upon failures of the code in specific tests. Naturally, 
// each application is different, and the direct use of the theorem and the code 
// may be appropriate for some applications. 


/// <summary>
/// Haversine function : hav(x) = (1-cos(x))/2
/// </summary>
/// <param name="x"></param>
/// <returns>Returns the value of Haversine function</returns>
double Haversine( double x )
{
    return ( 1.0 - cos( x ) ) / 2.0;
}

#define Cos cos
#define Sqrt sqrt
#define Tan tan
#define Abs fabs
#define Asin asin
#define Atan atan

#ifndef PI
#define PI M_PI
#endif

/// <summary>
/// Compute the Area of a Spherical Polygon
/// </summary>
/// <param name="lat">the latitudes of all vertices(in radian)</param>
/// <param name="lon">the longitudes of all vertices(in radian)</param>
/// <param name="r">spherical radius</param>
/// <returns>Returns the area of a spherical polygon</returns>
double SphericalPolygonArea( double *lat, double *lon, double r, int Length )
{
    double lam1 = 0, lam2 = 0, beta1 =0, beta2 = 0, cosB1 =0, cosB2 = 0;
    double hav = 0;
    double sum = 0;

    for( int j = 0 ; j < Length ; j++ )
    {
        int k = j + 1;
	    if( j == 0 )
	    {
	        lam1 = lon[j];
	        beta1 = lat[j];
	        lam2 = lon[j + 1];
	        beta2 = lat[j + 1];
	        cosB1 = Cos( beta1 );
	        cosB2 = Cos( beta2 );
        } else {
            k = ( j + 1 ) % Length;
    	    lam1 = lam2;
	        beta1 = beta2;
	        lam2 = lon[k];
            beta2 = lat[k];
	        cosB1 = cosB2;
	        cosB2 = Cos( beta2 );
	    }
        if( lam1 != lam2 ) {
            hav = Haversine( beta2 - beta1 ) + 
                          cosB1 * cosB2 * Haversine( lam2 - lam1 );
	        double a = 2 * Asin( Sqrt( hav ) );
	        double b = PI / 2 - beta2;
	        double c = PI / 2 - beta1;
	        double s = 0.5 * ( a + b + c );
	        double t = Tan( s / 2 ) * Tan( ( s - a ) / 2 ) *  
                                Tan( ( s - b ) / 2 ) * Tan( ( s - c ) / 2 );
            double excess = Abs( 4 * Atan( Sqrt( Abs( t ) ) ) );
            if( lam2 < lam1 ) {
                excess = -excess;
	        }
	        sum += excess;
    	}
    }
    return Abs( sum ) * r * r;
}

#if 0 // 00000000000000000000000000000000000000000000
/// <summary>
/// Calculates the azimuth from latA/lonA to latB/lonB
/// Borrowed from http://williams.best.vwh.net/avform.htm
/// </summary>
public static Angle Azimuth( Angle latA, Angle lonA, Angle latB, Angle lonB )
{
    double cosLatB = Math.Cos(latB.Radians);
    Angle tcA = Angle.FromRadians( Math.Atan2(
        Math.Sin(lonA.Radians - lonB.Radians) * cosLatB,
        Math.Cos(latA.Radians) * Math.Sin(latB.Radians) - 
        Math.Sin(latA.Radians) * cosLatB * 
        Math.Cos(lonA.Radians - lonB.Radians)));
    if(tcA.Radians < 0) 
        tcA.Radians = tcA.Radians + Math.PI*2;
    tcA.Radians = Math.PI*2 - tcA.Radians;
        return tcA;
}
#endif // 000000000000000000000000000000000000000000

void test_area()
{
    double lat1, lon1, lat2, lon2;
    double r = RADm;
    double lata[4];
    double lona[4];
    double area[4];
    int cnt = 0;
    lat1 = 0;
    lon1 = 0;
    lat2 = 1;
    lon2 = 1;
    lata[0] = DEG2RAD * lat1;
    lona[0] = DEG2RAD * lon1;
    lata[1] = DEG2RAD * lat1;
    lona[1] = DEG2RAD * lon2;
    lata[2] = DEG2RAD * lat2;
    lona[2] = DEG2RAD * lon2;
    lata[3] = DEG2RAD * lat2;
    lona[3] = DEG2RAD * lon1;
    area[cnt] = SphericalPolygonArea( lata, lona, r, 4 );
    SPRTF("lat1/lon1 %lf,%lf lat2,lon2 %lf,%lf as square gave %lf\n",
        lat1,lon1, lat2, lon2, area[cnt] );

    cnt++;
    lat1 += 80;
    lon1 += 80;
    lat2 += 80;
    lon2 += 80;
    lata[0] = DEG2RAD * lat1;
    lona[0] = DEG2RAD * lon1;
    lata[1] = DEG2RAD * lat1;
    lona[1] = DEG2RAD * lon2;
    lata[2] = DEG2RAD * lat2;
    lona[2] = DEG2RAD * lon2;
    lata[3] = DEG2RAD * lat2;
    lona[3] = DEG2RAD * lon1;
    area[cnt] = SphericalPolygonArea( lata, lona, r, 4 );
    SPRTF("lat1/lon1 %lf,%lf lat2,lon2 %lf,%lf as square gave %lf\n",
        lat1,lon1, lat2, lon2, area[cnt] );

    cnt++;
    lat1 += 8;
    lon1 += 8;
    lat2 += 8;
    lon2 += 8;
    lata[0] = DEG2RAD * lat1;
    lona[0] = DEG2RAD * lon1;
    lata[1] = DEG2RAD * lat1;
    lona[1] = DEG2RAD * lon2;
    lata[2] = DEG2RAD * lat2;
    lona[2] = DEG2RAD * lon2;
    lata[3] = DEG2RAD * lat2;
    lona[3] = DEG2RAD * lon1;
    area[cnt] = SphericalPolygonArea( lata, lona, r, 4 );
    SPRTF("lat1/lon1 %lf,%lf lat2,lon2 %lf,%lf as square gave %lf\n",
        lat1,lon1, lat2, lon2, area[cnt] );

    SPRTF("1x1 degree square at equator %d\n", (int)(area[0] + 0.5));
    SPRTF("1x1 degree square at 80 degs %d\n", (int)(area[1] + 0.5));
    SPRTF("1x1 degree square at 88 degs %d\n", (int)(area[2] + 0.5));
    SPRTF("Do these seem resaonable?\n");

}

// eof = test-area.cxx
