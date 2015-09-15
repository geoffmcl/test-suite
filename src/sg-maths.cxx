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
#ifndef _MSC_VER
#include <float.h> // for DBL_MIN, ...
#endif
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

// #ifdef HAVE_SIMGEAR  // indication simgear found
#if (defined(HAVE_SIMGEAR) && defined(USE_SG_GEODESY)) // indication simgear found

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

///////////////////////////////////////////////////////////////////////////////////

// avoid an 'unused parameter' compiler warning.
#define SG_UNUSED(x) (void)x
#define _EQURAD 6378137.0       // meters (6,371 km)
#define _FLATTENING 298.257223563
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

double rad2deg(double val) { return val*180/M_PI; }
double deg2rad(double val) { return val*M_PI/180; }

static inline double M0( double e2 ) {
    //double e4 = e2*e2;
  return M_PI*0.5*(1.0 - e2*( 1.0/4.0 + e2*( 3.0/64.0 + 
						  e2*(5.0/256.0) )));
}


// given, lat1, lon1, az1 and distance (s), calculate lat2, lon2
// and az2.  Lat, lon, and azimuth are in degrees.  distance in meters
int fg_geo_direct_wgs_84 ( double lat1, double lon1, double az1,
                        double s, double *lat2, double *lon2,
                        double *az2 )
{
    double a = _EQURAD, rf = _FLATTENING;
    double testv = 1.0E-10;
    double f = ( rf > 0.0 ? 1.0/rf : 0.0 );
    double b = a*(1.0-f);
    double e2 = f*(2.0-f);
    double phi1 = deg2rad(lat1), lam1 = deg2rad(lon1);
    double sinphi1 = sin(phi1), cosphi1 = cos(phi1);
    double azm1 = deg2rad(az1);
    double sinaz1 = sin(azm1), cosaz1 = cos(azm1);
	
	
    if( fabs(s) < 0.01 ) {	// distance < centimeter => congruency
	*lat2 = lat1;
	*lon2 = lon1;
	*az2 = 180.0 + az1;
	if( *az2 > 360.0 ) *az2 -= 360.0;
	return 0;
    } else if( DBL_MIN < fabs(cosphi1) ) {	// non-polar origin
	// u1 is reduced latitude
	double tanu1 = sqrt(1.0-e2)*sinphi1/cosphi1;
	double sig1 = atan2(tanu1,cosaz1);
	double cosu1 = 1.0/sqrt( 1.0 + tanu1*tanu1 ), sinu1 = tanu1*cosu1;
	double sinaz =  cosu1*sinaz1, cos2saz = 1.0-sinaz*sinaz;
	double us = cos2saz*e2/(1.0-e2);

	// Terms
	double	ta = 1.0+us*(4096.0+us*(-768.0+us*(320.0-175.0*us)))/16384.0,
	    tb = us*(256.0+us*(-128.0+us*(74.0-47.0*us)))/1024.0,
	    tc = 0;

	// FIRST ESTIMATE OF SIGMA (SIG)
	double first = s/(b*ta);  // !!
	double sig = first;
	double c2sigm, sinsig,cossig, temp,denom,rnumer, dlams, dlam;
	do {
	    c2sigm = cos(2.0*sig1+sig);
	    sinsig = sin(sig); cossig = cos(sig);
	    temp = sig;
	    sig = first + 
		tb*sinsig*(c2sigm+tb*(cossig*(-1.0+2.0*c2sigm*c2sigm) - 
				      tb*c2sigm*(-3.0+4.0*sinsig*sinsig)
				      *(-3.0+4.0*c2sigm*c2sigm)/6.0)
			   /4.0);
	} while( fabs(sig-temp) > testv);

	// LATITUDE OF POINT 2
	// DENOMINATOR IN 2 PARTS (TEMP ALSO USED LATER)
	temp = sinu1*sinsig-cosu1*cossig*cosaz1;
	denom = (1.0-f)*sqrt(sinaz*sinaz+temp*temp);

	// NUMERATOR
	rnumer = sinu1*cossig+cosu1*sinsig*cosaz1;
	*lat2 = rad2deg(atan2(rnumer,denom));

	// DIFFERENCE IN LONGITUDE ON AUXILARY SPHERE (DLAMS )
	rnumer = sinsig*sinaz1;
	denom = cosu1*cossig-sinu1*sinsig*cosaz1;
	dlams = atan2(rnumer,denom);

	// TERM C
	tc = f*cos2saz*(4.0+f*(4.0-3.0*cos2saz))/16.0;

	// DIFFERENCE IN LONGITUDE
	dlam = dlams-(1.0-tc)*f*sinaz*(sig+tc*sinsig*
				       (c2sigm+
					tc*cossig*(-1.0+2.0*
						   c2sigm*c2sigm)));
	*lon2 = rad2deg(lam1+dlam);
	if (*lon2 > 180.0  ) *lon2 -= 360.0;
	if (*lon2 < -180.0 ) *lon2 += 360.0;

	// AZIMUTH - FROM NORTH
	*az2 = rad2deg(atan2(-sinaz,temp));
	if ( fabs(*az2) < testv ) *az2 = 0.0;
	if( *az2 < 0.0) *az2 += 360.0;
	return 0;
    } else {			// phi1 == 90 degrees, polar origin
	double dM = a*M0(e2) - s;
	double paz = ( phi1 < 0.0 ? 180.0 : 0.0 );
        double zero = 0.0f;
	    return fg_geo_direct_wgs_84( zero, lon1, paz, dM, lat2, lon2, az2 );
    } 
}

// given lat1, lon1, lat2, lon2, calculate starting and ending
// az1, az2 and distance (s).  Lat, lon, and azimuth are in degrees.
// distance in meters
int fg_geo_inverse_wgs_84( double lat1, double lon1, double lat2,
			double lon2, double *az1, double *az2,
                        double *s )
{
    double a = _EQURAD, rf = _FLATTENING;
    int iter=0;
    double testv = 1.0E-10;
    double f = ( rf > 0.0 ? 1.0/rf : 0.0 );
    double b = a*(1.0-f);
    // double e2 = f*(2.0-f); // unused in this routine
    double phi1 = deg2rad(lat1), lam1 = deg2rad(lon1);
    double sinphi1 = sin(phi1), cosphi1 = cos(phi1);
    double phi2 = deg2rad(lat2), lam2 = deg2rad(lon2);
    double sinphi2 = sin(phi2), cosphi2 = cos(phi2);
	
    if( (fabs(lat1-lat2) < testv && 
	 ( fabs(lon1-lon2) < testv)) || (fabs(lat1-90.0) < testv ) )
    {	
	// TWO STATIONS ARE IDENTICAL : SET DISTANCE & AZIMUTHS TO ZERO */
	*az1 = 0.0; *az2 = 0.0; *s = 0.0;
	return 0;
    } else if(  fabs(cosphi1) < testv ) {
	// initial point is polar
	int k = fg_geo_inverse_wgs_84( lat2,lon2,lat1,lon1, az1,az2,s );
	SG_UNUSED(k);
    
	b = *az1; *az1 = *az2; *az2 = b;
	return 0;
    } else if( fabs(cosphi2) < testv ) {
	// terminal point is polar
        double _lon1 = lon1 + 180.0f;
	int k = fg_geo_inverse_wgs_84( lat1, lon1, lat1, _lon1, 
				    az1, az2, s );
	SG_UNUSED(k);
    
	*s /= 2.0;
	*az2 = *az1 + 180.0;
	if( *az2 > 360.0 ) *az2 -= 360.0; 
	return 0;
    } else if( (fabs( fabs(lon1-lon2) - 180 ) < testv) && 
	       (fabs(lat1+lat2) < testv) ) 
    {
	// Geodesic passes through the pole (antipodal)
	double s1,s2;
	fg_geo_inverse_wgs_84( lat1,lon1, lat1,lon2, az1,az2, &s1 );
	fg_geo_inverse_wgs_84( lat2,lon2, lat1,lon2, az1,az2, &s2 );
	*az2 = *az1;
	*s = s1 + s2;
	return 0;
    } else {
	// antipodal and polar points don't get here
	double dlam = lam2 - lam1, dlams = dlam;
	double sdlams,cdlams, sig,sinsig,cossig, sinaz,
	    cos2saz, c2sigm;
	double tc,temp, us,rnumer,denom, ta,tb;
	double cosu1,sinu1, sinu2,cosu2;

	// Reduced latitudes
	temp = (1.0-f)*sinphi1/cosphi1;
	cosu1 = 1.0/sqrt(1.0+temp*temp);
	sinu1 = temp*cosu1;
	temp = (1.0-f)*sinphi2/cosphi2;
	cosu2 = 1.0/sqrt(1.0+temp*temp);
	sinu2 = temp*cosu2;
    
	do {
	    sdlams = sin(dlams), cdlams = cos(dlams);
	    sinsig = sqrt(cosu2*cosu2*sdlams*sdlams+
			  (cosu1*sinu2-sinu1*cosu2*cdlams)*
			  (cosu1*sinu2-sinu1*cosu2*cdlams));
	    cossig = sinu1*sinu2+cosu1*cosu2*cdlams;
	    
	    sig = atan2(sinsig,cossig);
	    sinaz = cosu1*cosu2*sdlams/sinsig;
	    cos2saz = 1.0-sinaz*sinaz;
	    c2sigm = (sinu1 == 0.0 || sinu2 == 0.0 ? cossig : 
		      cossig-2.0*sinu1*sinu2/cos2saz);
	    tc = f*cos2saz*(4.0+f*(4.0-3.0*cos2saz))/16.0;
	    temp = dlams;
	    dlams = dlam+(1.0-tc)*f*sinaz*
		(sig+tc*sinsig*
		 (c2sigm+tc*cossig*(-1.0+2.0*c2sigm*c2sigm)));
	    if (fabs(dlams) > M_PI && iter++ > 50) {
		return iter;
	    }
	} while ( fabs(temp-dlams) > testv);

	us = cos2saz*(a*a-b*b)/(b*b); // !!
	// BACK AZIMUTH FROM NORTH
	rnumer = -(cosu1*sdlams);
	denom = sinu1*cosu2-cosu1*sinu2*cdlams;
	*az2 = rad2deg(atan2(rnumer,denom));
	if( fabs(*az2) < testv ) *az2 = 0.0;
	if(*az2 < 0.0) *az2 += 360.0;

	// FORWARD AZIMUTH FROM NORTH
	rnumer = cosu2*sdlams;
	denom = cosu1*sinu2-sinu1*cosu2*cdlams;
	*az1 = rad2deg(atan2(rnumer,denom));
	if( fabs(*az1) < testv ) *az1 = 0.0;
	if(*az1 < 0.0) *az1 += 360.0;

	// Terms a & b
	ta = 1.0+us*(4096.0+us*(-768.0+us*(320.0-175.0*us)))/
	    16384.0;
	tb = us*(256.0+us*(-128.0+us*(74.0-47.0*us)))/1024.0;

	// GEODETIC DISTANCE
	*s = b*ta*(sig-tb*sinsig*
		   (c2sigm+tb*(cossig*(-1.0+2.0*c2sigm*c2sigm)-tb*
			       c2sigm*(-3.0+4.0*sinsig*sinsig)*
			       (-3.0+4.0*c2sigm*c2sigm)/6.0)/
		    4.0));
	return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////

// eof = sg-maths.cxx
