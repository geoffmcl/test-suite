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

#endif // #ifndef _SG_MATHS_HXX_
// eof - sg-maths.hxx
