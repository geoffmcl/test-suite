/*\
 * sg-maths.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _SG_MATHS_HXX_
#define _SG_MATHS_HXX_

extern double distance_km(double lat1, double lon1, double lat2, double lon2);
#ifdef HAVE_SIMGEAR
extern double SGDistance_km(double lat1, double lon1, double lat2, double lon2);
extern void test_dist();
extern int sg_geo_direct_wgs_84 ( double lat1, double lon1, double az1, double s, 
                            double *lat2, double *lon2, double *az2 );
extern int sg_geo_inverse_wgs_84( double lat1, double lon1, double lat2, double lon2,
        double *az1, double *az2, double *dist );

#endif

//////////////////////////////////////////////////////////////////////////////////////
// without SG library

// given, lat1, lon1, az1 and distance (s), calculate lat2, lon2
// and az2.  Lat, lon, and azimuth are in degrees.  distance in meters
extern int fg_geo_direct_wgs_84 ( double lat1, double lon1, double az1, double s, /* IN */
                        double *lat2, double *lon2, double *az2 ); /* OUT */
// given lat1, lon1, lat2, lon2, calculate starting and ending
// az1, az2 and distance (s).  Lat, lon, and azimuth are in degrees.
// distance in meters
extern int fg_geo_inverse_wgs_84( double lat1, double lon1, double lat2, double lon2, /* IN */
                        double *az1, double *az2, double *s ); /* OUT */

///////////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef _SG_MATHS_HXX_
// eof - sg-maths.hxx
