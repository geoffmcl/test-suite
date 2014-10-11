/*\
 * sg-maths.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_SIMGEAR  // indication simgear found
#include <simgear/constants.h>
#include <simgear/sg_inlines.h>
#include <simgear/math/SGMath.hxx>
#include <simgear/math/sg_geodesy.hxx>
#endif

#include "sg-maths.hxx"

#ifndef _SG_CONSTANTS_H
const double SGD_DEGREES_TO_RADIANS = M_PI / 180.0;
#endif

static const char *module = "sg-maths";

////////////////////////////////////////////////////////////
/* 
expected values are from : http://www.gpsvisualizer.com/calculators
From EGLL to KSFO Haversine = 8616.42, SG = 8638.65, expect 8638.65
From EGLL to LFPO Haversine = 365.44, SG = 365.87, expect 365.87
From LFPG to LFPO Haversine = 34.46, SG = 34.48, expect 34.48
 */

// // http://www.movable-type.co.uk/scripts/LatLong.html
// degrees to radians
//	rlat1 = deg2rad(lat1); rlon1 = deg2rad(lon1);
//	rlat2 = deg2rad(lat2); rlon2 = deg2rad(lon2);
// deltas
// 	var dlat = rlat2-rlat1; // delta
//	var dlon = rlon2-rlon1; // delta
//	var alat = (rlat1+rlat2)/2; // average
// 	var a = ( Math.sin(dlat/2) * Math.sin(dlat/2) ) + 
//          ( Math.cos(rlat1) * Math.cos(rlat2) * Math.sin(dlon/2) * Math.sin(dlon/2) );
//	var c = 2 * Math.atan( Math.sqrt(a)/Math.sqrt(1-a) );
//double toRadians( double deg) { return (deg * (3.14159265358979/180.0)); }
#define toRadians(a) (a * SGD_DEGREES_TO_RADIANS)
// Haversine formula
double distance_km(double lat1, double lon1, double lat2, double lon2)
{
    double R = 6371; // mean radius in km - set return units
    // get degrees in radians
    double rlat1 = toRadians(lat1);
    double rlon1 = toRadians(lon1);
    double rlat2 = toRadians(lat2);
    double rlon2 = toRadians(lon2);
    // get deltas
    double dlat = rlat2 - rlat1;
    double dlon = rlon2 - rlon1;
    // the 'tricky' stuff
    double a = (sin(dlat/2.0) * sin(dlat/2.0)) +
               ( cos(rlat1) * cos(rlat2) * sin(dlon/2.0) * sin(dlon/2.0) );
    double c = 2 * atan2(sqrt(a),sqrt(1-a));
    double d = R * c;
    int id = (d + 0.005) * 100.0;
    return ((double)id / 100.0);
}

#ifdef HAVE_SIMGEAR  // indication simgear found
// SimGear uses the Thaddeus Vincenty (TV) devised formulae 
// from : http://www.movable-type.co.uk/scripts/latlong-vincenty.html
// Vincenty’s formula is accurate to within 0.5mm, or 0.000015" (!), on the ellipsoid being used. 
// Calculations based on a spherical model, such as the (much simpler) Haversine, are accurate to 
// around 0.3% (which is still good enough for most purposes, of course).
double SGDistance_km(double lat1, double lon1, double lat2, double lon2)
{
    double az1,az2,dist;
    SGGeod geod1,geod2;
    
    geod1.setLatitudeDeg(lat1);
    geod1.setLongitudeDeg(lon1);
    geod1.setElevationM(0.0);

    geod2.setLatitudeDeg(lat2);
    geod2.setLongitudeDeg(lon2);
    geod2.setElevationM(0.0);

    int res = geo_inverse_wgs_84(geod1,geod2,&az1,&az2,&dist);
    double km = dist / 1000.0;

    int id = (km + 0.005) * 100.0;
    return ((double)id / 100.0);

}

int sg_geo_inverse_wgs_84( double lat1, double lon1, double lat2, double lon2,
        double *az1, double *az2, double *dist )
{
    SGGeod geod1,geod2;

    geod1.setLatitudeDeg(lat1);
    geod1.setLongitudeDeg(lon1);
    geod1.setElevationM(0.0);

    geod2.setLatitudeDeg(lat2);
    geod2.setLongitudeDeg(lon2);
    geod2.setElevationM(0.0);

    int res = geo_inverse_wgs_84(geod1,geod2,az1,az2,dist);
    return res;
}

int sg_geo_direct_wgs_84 ( double lat1, double lon1, double az1, double s, 
                            double *lat2, double *lon2, double *az2 )
{
    SGGeod geod1,geod2;
    geod1.setLatitudeDeg(lat1);
    geod1.setLongitudeDeg(lon1);
    geod1.setElevationM(0.0);
    int res = geo_direct_wgs_84( geod1, az1, s, 
        geod2, az2 );
    *lat2 = geod2.getLatitudeDeg();
    *lon2 = geod2.getLongitudeDeg();
    return res;
}

#endif

// eof = sg-maths.cxx
