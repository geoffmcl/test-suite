/*\
 * test-math.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * Lots of math tests
 *
 * 20140918: Convert lat,lon,orientation to heading, pitch and roll
 *
\*/

#include <stdio.h>
#ifndef _MSC_VER
/////////////////////////////////////////////////////////
#include <stdlib.h> // for abs(), ...
#include <string.h> // for memcpy(), ...
/////////////////////////////////////////////////////////
#endif
#include <math.h>
#include <limits>
#include "sprtf.hxx"
#include "utils.hxx"
#ifdef HAVE_SIMGEAR
#include <simgear/compiler.h>
#include <simgear/constants.h>
#include <simgear/math/SGMath.hxx>
#endif
#include "sg-maths.hxx"
#include "test-math.hxx"

static const char *module = "test-math";

typedef struct tagVec2 {
    double x,y;
}Vec2, *pVec2;


double f_sqr(double x) { return x * x; }
double f_dist2(Vec2 &v, Vec2 &w) { return f_sqr(v.x - w.x) + f_sqr(v.y - w.y); }
double distToSegmentSquared(Vec2 &p, Vec2 &v, Vec2 &w)
{
    double l2 = f_dist2(v, w);
    if (l2 == 0)
        return f_dist2(p, v);
    double t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
    if (t < 0)
        return f_dist2(p, v);
    if (t > 1)
        return f_dist2(p, w);
    Vec2 v2;
    v2.x = v.x + t * (w.x - v.x);
    v2.y = v.y + t * (w.y - v.y);
    return f_dist2(p, v2);
}

double distToSegment(Vec2 &p, Vec2 &v, Vec2 &w)
{ 
    return sqrt(distToSegmentSquared(p, v, w)); 
}

///////////////////////////////////////////////////////////////////////////////
// other service
// OTHER SOLUTIONS
// ===============
double pDistance(double x, double y, double x1, double y1, double x2, double y2)
{
    double A = x - x1;
    double B = y - y1;
    double C = x2 - x1;
    double D = y2 - y1;

    double dot = A * C + B * D;
    double len_sq = C * C + D * D;
    double param = dot / len_sq;

    double xx, yy;

    if ((param < 0) || ((x1 == x2) && (y1 == y2))) {
        xx = x1;
        yy = y1;
    } else if (param > 1) {
        xx = x2;
        yy = y2;
    } else {
        xx = x1 + param * C;
        yy = y1 + param * D;
    }

    double dx = x - xx;
    double dy = y - yy;

    return sqrt((dx * dx) + (dy * dy));
}

//Compute the dot product AB . AC
double DotProduct(double *pointA, double *pointB, double *pointC)
{
    double AB[2];
    double BC[2];
    AB[0] = pointB[0] - pointA[0];
    AB[1] = pointB[1] - pointA[1];
    BC[0] = pointC[0] - pointB[0];
    BC[1] = pointC[1] - pointB[1];
    double dot = AB[0] * BC[0] + AB[1] * BC[1];
    return dot;
}

//Compute the cross product AB x AC
double CrossProduct(double *pointA, double *pointB, double *pointC)
{
    double AB[2];
    double AC[2];
    AB[0] = pointB[0] - pointA[0];
    AB[1] = pointB[1] - pointA[1];
    AC[0] = pointC[0] - pointA[0];
    AC[1] = pointC[1] - pointA[1];
    double cross = AB[0] * AC[1] - AB[1] * AC[0];
    return cross;
}

//Compute the distance from A to B
double Distance(double *pointA, double *pointB)
{
    double d1 = pointA[0] - pointB[0];
    double d2 = pointA[1] - pointB[1];
    return sqrt(d1 * d1 + d2 * d2);
}

//Compute the distance from AB to C
//if isSegment is true, AB is a segment, not a line.
double LineToPointDistance2D(double *pointA, double *pointB, double *pointC, bool isSegment = false);

double LineToPointDistance2D(double *pointA, double *pointB, double *pointC, bool isSegment)
{
    double dist = CrossProduct(pointA, pointB, pointC) / Distance(pointA, pointB);
    if (isSegment)
    {
        double dot1 = DotProduct(pointA, pointB, pointC);
        if (dot1 > 0) 
            return Distance(pointB, pointC);

        double dot2 = DotProduct(pointB, pointA, pointC);
        if (dot2 > 0) 
            return Distance(pointA, pointC);
    }
    return abs(dist);
} 

//////////////////////////////////////////////////
// fuzzy test for zero, or close to zero
#ifndef MY_VERYSMALL
#define MY_VERYSMALL  (1.0E-150)
#endif
#ifndef MY_EPSILON
#define MY_EPSILON    (1.0E-8)
#endif

bool absoluteIsZero(double x)
{
    if (x == 0.0)
        return true;
#ifdef _MSC_VER
    return std::fabs(x) <= std::numeric_limits<double>::epsilon() ;
#else
    return fabs(x) <= std::numeric_limits<double>::epsilon() ;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// from : http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison

bool absoluteToleranceCompare(double x, double y)
{
#ifdef _MSC_VER
    return std::fabs(x - y) <= std::numeric_limits<double>::epsilon() ;
#else
    return fabs(x - y) <= std::numeric_limits<double>::epsilon() ;
#endif
}

// from : http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
// and : http://en.wikipedia.org/wiki/Line-line_intersection

// This gets the intersection point IF one exists when the lines are extended to infinity
// **************************************************************************************
bool IntersectionPoint( double x1, double y1, double x2, double y2, 
                        double x3, double y3, double x4, double y4,
                        double *x, double *y )
{
    double denom = ((x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4));
    if (absoluteIsZero(denom)) {
        return false;   // lines are parallel - NO INTERSECTION
    }
    double dx = (((x1 * y2) - (y1 * x2)) * (x3 - x4)) -
                ((x1 - x2) * ((x3 * y4) - (y3 * x4)));

    double dy = (((x1 * y2) - (y1 * x2)) * (y3 - y4)) -
                ((y1 - y2) * ((x3 * y4) - (y3 * x4)));

    *x = dx / denom;
    *y = dy / denom;

    return true;
}

// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
// intersect the intersection point may be stored in the floats i_x and i_y.
// This checks the lines as is, with no extension.
char get_line_intersection(double p0_x, double p0_y, double p1_x, double p1_y, 
    double p2_x, double p2_y, double p3_x, double p3_y,
    double *i_x, double *i_y)
{
    double s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;
    s2_y = p3_y - p2_y;

    double d = (-s2_x * s1_y + s1_x * s2_y);

    if (absoluteIsZero(d)) {
        return 0;
    }

    double s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / d;
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / d;

    if ((s >= 0) && (s <= 1) && (t >= 0) && (t <= 1))
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}

//////////////////////////////////////////////////////////////////////////
// from : http://ptspts.blogspot.fr/2010/06/how-to-determine-if-two-line-segments.html
// 
bool IsOnSegment(double xi, double yi, double xj, double yj,
                        double xk, double yk) 
{
    return (xi <= xk || xj <= xk) && (xk <= xi || xk <= xj) &&
         (yi <= yk || yj <= yk) && (yk <= yi || yk <= yj);
}

char ComputeDirection(double xi, double yi, double xj, double yj,
                             double xk, double yk) 
{
      double a = (xk - xi) * (yj - yi);
      double b = (xj - xi) * (yk - yi);
      return a < b ? -1 : a > b ? 1 : 0;
}

/** Do line segments (x1, y1)--(x2, y2) and (x3, y3)--(x4, y4) intersect? */
bool DoLineSegmentsIntersect(double x1, double y1, double x2, double y2,
                             double x3, double y3, double x4, double y4) 
{
    char d1 = ComputeDirection(x3, y3, x4, y4, x1, y1);
    char d2 = ComputeDirection(x3, y3, x4, y4, x2, y2);
    char d3 = ComputeDirection(x1, y1, x2, y2, x3, y3);
    char d4 = ComputeDirection(x1, y1, x2, y2, x4, y4);
    return (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
           ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) ||
           (d1 == 0 && IsOnSegment(x3, y3, x4, y4, x1, y1)) ||
           (d2 == 0 && IsOnSegment(x3, y3, x4, y4, x2, y2)) ||
           (d3 == 0 && IsOnSegment(x1, y1, x2, y2, x3, y3)) ||
           (d4 == 0 && IsOnSegment(x1, y1, x2, y2, x4, y4));
}

////////////////////////////////////////////////////////////////
// JUST A BUNCH OF TESTS
// =====================
// ********************************************************************
bool try_line_2_point = false;  // get NaN if line does not have length
// ********************************************************************

void test_angles();

void test_math()
{
    SPRTF("\n");
    SPRTF("%s: Doing various math tests...\n", module);
    double dist, expect, diff, dist2;
    double A[2];
    double B[2];
    double C[2];
    Vec2 p, v, w; // 148.985275 -31.831722 # 23
    // Point: 148.843547, -31.827855
    // Point: 148.843979, -31.832695 from prev:  0.00485924109301086
    p.x = 148.843547;
    p.y = -31.827855;
    v.x = 148.843979;
    v.y = -31.832695;
    w = v;
    test_angles();

    expect = 0.004859;
    dist = distToSegment( p, v, w );
    diff = (dist > expect ? dist - expect : expect - dist);
    if (try_line_2_point) {
        // LineToPointDistance2D
        // line AB
        A[0] = v.x;
        A[1] = v.y;
        B[0] = w.x;
        B[1] = w.y;
        // point C
        C[0] = p.x;
        C[1] = p.y;
        dist2 = LineToPointDistance2D( A, B, C );
        SPRTF("%s: Distance is %s, expect %s, diff %g (%s)\n", module, 
            double_to_stg(dist), double_to_stg(expect), diff,
            double_to_stg(dist2));
    } else {
        SPRTF("%s: Distance is %s, expect %s, diff %g\n", module, 
            double_to_stg(dist), double_to_stg(expect), diff);
    }
    // Point: 148.985275, -31.831722
    p.x = 148.985275;
    p.y = -31.831722; // # 23

    // Point: 148.970499, -31.825051
    v.x = 148.970499;
    v.x = -31.825051; // # 50
    // Point: 149.001452, -31.829201
    w.x = 149.001452;
    w.y = -31.829201; // # 51
    //expect = 0.0047;
    expect = 0.0025;
    dist = distToSegment( p, v, w );
    diff = (dist > expect ? dist - expect : expect - dist);
    // LineToPointDistance2D
    // line AB
    A[0] = v.x;
    A[1] = v.y;
    B[0] = w.x;
    B[1] = w.y;
    // point C
    C[0] = p.x;
    C[1] = p.y;
    dist2 = LineToPointDistance2D( A, B, C );
    SPRTF("%s: Distance is %s, expect %s, diff %g (%s)\n", module, 
        double_to_stg(dist), double_to_stg(expect), diff,
        double_to_stg(dist2));

    // Point: 148.931854, -31.837694
    p.x = 148.931854;
    p.y = -31.837694;
    // Point: 148.928107, -31.831668
    v.x = 148.928107;
    v.y = -31.831668;
    w = v;
    expect = 0.007096;
    dist = distToSegment( p, v, w );
    diff = (dist > expect ? dist - expect : expect - dist);
    SPRTF("%s: Distance is %s, expect %s, diff %g\n", module, 
        double_to_stg(dist), double_to_stg(expect), diff);

    // Point: 148.931854, -31.837694 # 17
    p.x = 148.931854;
    p.y = -31.837694;
    // Point: 148.94706, -31.826509 # 47 from prev:  0.0188766167784414
    v.x = 148.94706;
    v.y = -31.826509;
    w = v;
    expect = 0.01888;
    dist = distToSegment( p, v, w );
    diff = (dist > expect ? dist - expect : expect - dist);
    SPRTF("%s: Distance is %s, expect %s, diff %g\n", module, 
        double_to_stg(dist), double_to_stg(expect), diff);

    // Point: 148.931854, -31.837694 # 17
    p.x = 148.931854;
    p.y = -31.837694;
    // Point: 148.930138116285, -31.8311281834117 from prev:  0.0067863248076031
    // Point: 148.928107, -31.831668
    v.x = 148.928107;
    v.y = -31.831668;
    w.x = 148.94706;
    w.y = -31.826509;
    expect = 0.006786;
    dist = distToSegment( p, v, w );
    diff = (dist > expect ? dist - expect : expect - dist);
    // LineToPointDistance2D
    // line AB
    A[0] = v.x;
    A[1] = v.y;
    B[0] = w.x;
    B[1] = w.y;
    // point C
    C[0] = p.x;
    C[1] = p.y;
    dist2 = LineToPointDistance2D( A, B, C );
    SPRTF("%s: Distance is %s, expect %s, diff %g (%s)\n", module, 
        double_to_stg(dist), double_to_stg(expect), diff,
        double_to_stg(dist2));
    //SPRTF("%s: Distance is %s, expect %s, diff %g\n", module, 
    //    double_to_stg(dist), double_to_stg(expect), diff);

    // some intersection tests
    //# test1a.xg
    // color yellow
    double x1, y1, x2, y2;
    double x3, y3, x4, y4;
    double x, y;
    x1 = 148.0;
    y1 = -30.999;
    double ex = 148.0;
    double ey = -31.0;
    x2 = 148.0;
    y2 = -31.001;
    // NEXT
    // # test1b.xg
    // color green
    x3 = 147.998;
    y3 = -31.0;
    // 148.0, -31.0
    x4 = 148.002;
    y4 = -31.0;
    // NEXT

    if ( IntersectionPoint( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y ) ) {
        SPRTF("%s: Intersect at x,y %s,%s, expected %s,%s\n", module,
            double_to_stg(x), double_to_stg(y), double_to_stg(ex), double_to_stg(ey) );
    }
    if ( get_line_intersection( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y ) ) {
        SPRTF("%s: Intersect at x,y %s,%s, expected %s,%s\n", module,
            double_to_stg(x), double_to_stg(y), double_to_stg(ex), double_to_stg(ey) );
    }

    // Point: 305.981595092024, 891.411042944785
    // Point: -36.5030674846626, 307.668711656442
    // Point: 265.490797546012, 179.447852760736
    // Point: 1043.25153374233, 904.907975460123
    x1 = 305.9;
    y1 = 891.4;
    x2 = -36.5;
    y2 = 307.6;
    x3 = 265.4;
    y3 = 179.4;
    x4 = 1043.2;
    y4 = 904.9;
    bool gotip;
    char gotip2;
    gotip = IntersectionPoint( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y );
    gotip2 = get_line_intersection( x1, y1, x2, y2, x3, y3, x4, y4, &ex, &ey );
    if (DoLineSegmentsIntersect(x1, y1, x2, y2, x3, y3, x4, y4)) {
        SPRTF("%s: Lines intersect ", module);
        if (gotip)
            SPRTF("at point %s,%s ", double_to_stg(x), double_to_stg(y));
        if (gotip2)
            SPRTF("(%s,%s) ", double_to_stg(ex), double_to_stg(ey) );
        SPRTF("\n");
    } else {
        SPRTF("%s: Lines do NOT intersect ", module);
        if (gotip)
            SPRTF("extended at pt %s,%s ", double_to_stg(x), double_to_stg(y));
        if (gotip2) {
            SPRTF("(%s,%s) ", double_to_stg(ex), double_to_stg(ey) );
        } else {
            SPRTF("\nand likewise get_line_intersection() found reports 'no collision' ");
        }
        SPRTF("\n");
    }
    // A T on its side, with NO current intersection
    // Point: -166.4, 535.4
    // Point: 422.3, 532.0
    // Point: 650.1, 926.8
    // Point: 643.4, 132.2
    x1 = -166.4;
    y1 = 535.4;
    x2 = 422.3;
    y2 = 532.0;
    x3 = 650.1;
    y3 = 926.8;
    x4 = 643.4;
    y4 = 132.2;
    gotip = IntersectionPoint( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y );
    gotip2 = get_line_intersection( x1, y1, x2, y2, x3, y3, x4, y4, &ex, &ey );
    if (DoLineSegmentsIntersect(x1, y1, x2, y2, x3, y3, x4, y4)) {
        SPRTF("%s: Lines intersect ", module);
        if (gotip)
            SPRTF("at point %s,%s ", double_to_stg(x), double_to_stg(y));
        if (gotip2)
            SPRTF("(%s,%s) ", double_to_stg(ex), double_to_stg(ey) );
        SPRTF("\n");
    } else {
        SPRTF("%s: Lines do NOT intersect ", module);
        if (gotip)
            SPRTF("extended at pt %s,%s ", double_to_stg(x), double_to_stg(y));
        if (gotip2) {
            SPRTF("(%s,%s) ", double_to_stg(ex), double_to_stg(ey) );
        } else {
            SPRTF("\nand likewise get_line_intersection() found reports 'no collision' ");
        }
        SPRTF("\n");
    }

    // parallell
    x1 = 147.0;
    y1 = -31.0;
    x2 = 149.0;
    y2 = -31.0;
    x3 = 147.0;
    y3 = -32.0;
    x4 = 149.0;
    y4 = -32.0;

    gotip = IntersectionPoint( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y );
    gotip2 = get_line_intersection( x1, y1, x2, y2, x3, y3, x4, y4, &ex, &ey );
    if (DoLineSegmentsIntersect(x1, y1, x2, y2, x3, y3, x4, y4)) {
        SPRTF("%s: Lines intersect ", module);
        if (gotip)
            SPRTF("at point %s,%s ", double_to_stg(x), double_to_stg(y));
        if (gotip2)
            SPRTF("(%s,%s) ", double_to_stg(ex), double_to_stg(ey) );
        SPRTF("\n");
    } else {
        SPRTF("%s: Lines do NOT intersect ", module);
        if (gotip)
            SPRTF("extended at pt %s,%s ", double_to_stg(x), double_to_stg(y));
        else
            SPRTF("and IntersectionPoint() sugggests parallel ");
        if (gotip2) {
            SPRTF("(%s,%s) ", double_to_stg(ex), double_to_stg(ey) );
        } else {
            SPRTF("\nand likewise get_line_intersection() found reports 'no collision' ");
        }
        SPRTF("\n");
    }



    y4 = -31.9999999999;

    gotip = IntersectionPoint( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y );
    gotip2 = get_line_intersection( x1, y1, x2, y2, x3, y3, x4, y4, &ex, &ey );
    if (DoLineSegmentsIntersect(x1, y1, x2, y2, x3, y3, x4, y4)) {
        SPRTF("%s: Lines intersect ", module);
        if (gotip)
            SPRTF("at point %s,%s ", double_to_stg(x), double_to_stg(y));
        if (gotip2)
            SPRTF("(%s,%s) ", double_to_stg(ex), double_to_stg(ey) );
        SPRTF("\n");
    } else {
        SPRTF("%s: Lines do NOT intersect ", module);
        if (gotip)
            SPRTF("extended at pt %s,%s ", double_to_stg(x), double_to_stg(y));
        else
            SPRTF("and IntersectionPoint() sugggests parallel ");
        if (gotip2) {
            SPRTF("(%s,%s) ", double_to_stg(ex), double_to_stg(ey) );
        } else {
            SPRTF("\nand likewise get_line_intersection() found reports 'no collision' ");
        }
        SPRTF("\n");
    }

    if (absoluteIsZero((0.9 + 0.1) - 1.0))
        SPRTF("%s: (0.9 + 0.1) - 1.0) is ZERO\n", module);
    else
        SPRTF("%s: WHAT! (0.9 + 0.1) - 1.0) is NOT ZERO\n", module);

    SPRTF("%s: std::numeric_limits<double>::epsilon() = %g\n", module, std::numeric_limits<double>::epsilon() );

    SPRTF("%s: Done various math tests...\n", module);
}

double Vec2_to_rads( Vec2 &p, Vec2 &v )
{
    return atan2( p.x - v.x, p.y - v.y );
}

double Vec2_to_rads_fixedy( Vec2 &p, Vec2 &v )
{
    Vec2 fp, fv;
    if (p.y < v.y) {
        fp = p;
        fv = v;
    } else {
        fp = v;
        fv = p;
    }
    return atan2( fp.x - fv.x, fp.y - fv.y );
}


void test_angles()
{
    Vec2 p, v;
    double angle, degs;
    p.x = 148.843547;
    p.y = -31.827855;
    v.x = 148.843979;
    v.y = -31.832695;
    //double angle = atan2( p.y - v.y , p.x - v.x );
    angle = atan2( p.x - v.x , p.y - v.y );
    degs = angle * RAD2DEG;
    SPRTF("%s: Angle of line %lf,%lf to %lf,%lf is %lf degs, %lf radian. ", module,
        p.x, p.y, v.x, v.y, degs, angle );
    degs = Vec2_to_rads_fixedy( p, v ) * RAD2DEG;
    SPRTF("(fx %s)\n", double_to_stg(degs));


    //Point: 148.841011177914, -31.8305934601227
    //Point: 148.846302515337, -31.8304301472393
    p.x = 148.841011177914;
    p.y = -31.8305934601227;
    v.x = 148.846302515337;
    v.y = -31.8304301472393;

    SPRTF("%s: Angle of line p=%s,%s to v=%s,%s\n", module,
        double_to_stg(p.x), double_to_stg(p.y),
        double_to_stg(v.x), double_to_stg(v.y) );

    angle = Vec2_to_rads( p, v );
    degs = angle * RAD2DEG;
    SPRTF("%s: line p -> v is %s degs", module,
        double_to_stg(degs) );
    angle = Vec2_to_rads( v, p );
    degs = angle * RAD2DEG;
    SPRTF(", line v -> p is %s degs. ", double_to_stg(degs) );
    degs = Vec2_to_rads_fixedy( p, v ) * RAD2DEG;
    SPRTF("(fx %s)\n", double_to_stg(degs));

    p.x = 148.0;
    p.y = -31.0;
    v.x = 149.0;
    v.y = -32.0;
    SPRTF("%s: Angle of line p=%s,%s to v=%s,%s\n", module,
        double_to_stg(p.x), double_to_stg(p.y),
        double_to_stg(v.x), double_to_stg(v.y) );

    angle = Vec2_to_rads( p, v );
    degs = angle * RAD2DEG;
    SPRTF("%s: line p -> v is %s degs", module,
        double_to_stg(degs) );
    angle = Vec2_to_rads( v, p );
    degs = angle * RAD2DEG;
    SPRTF(", line v -> p is %s degs. ", double_to_stg(degs) );
    degs = Vec2_to_rads_fixedy( v, p ) * RAD2DEG;
    SPRTF("(fx %s)", double_to_stg(degs));
    degs = Vec2_to_rads_fixedy( p, v ) * RAD2DEG;
    SPRTF("(fx %s)\n", double_to_stg(degs));

}

///////////////////////////////////////////////////////////////////////
// 20140918:
/* =============================================================================
    Using the 'complicated' SimGear maths got this line
    Victor at 37.405960,-122.042817,42, orien 2.366594,1.994152,1.443017, in 777-200ER hdg=338 spd=3 pkts=1/0 
    OK-JVK at 47.802738,11.996773,19171, orien -1.121633,-2.214272,0.591599, in 777-200 hdg=65 spd=427 pkts=1/0 
    MCA0340 at 8.382262,90.018088,38160, orien -0.630049,-2.556933,2.186189, in 757-200PF hdg=117 spd=533 pkts=1/0 
    AHGM at -5.514906,-34.887578,200, orien -1.121828,-1.020800,0.361874, in DO-J-II-r hdg=61 spd=103 pkts=1/0 
    F-BLCK at 12.847165,-30.467642,16001, orien -1.824425,2.236964,2.139209, in m2000-5 hdg=252 spd=487 pkts=1/0 
    Canseco at 40.804990,1.186377,33061, orien -2.244615,1.214249,1.228393, in m2000-5 hdg=215 spd=655 pkts=2/0 Est=652 kt 0 nm 
    Using alternate maths, got slightly different answers
    Victor at 37.405960,-122.042817,42, orien 2.366594,1.994152,1.443017, in 777-200ER hdg=270 spd=3 pkts=1/0 
    OK-JVK at 47.802738,11.996773,19171, orien -1.121633,-2.214272,0.591599, in 777-200 hdg=344 spd=427 pkts=1/0 
    MCA0340 at 8.382262,90.018088,38160, orien -0.630049,-2.556933,2.186189, in 757-200PF hdg=39 spd=533 pkts=1/0 
    AHGM at -5.514906,-34.887578,200, orien -1.121828,-1.020800,0.361874, in DO-J-II-r hdg=67 spd=103 pkts=1/0 
    F-BLCK at 12.847165,-30.467642,16001, orien -1.824425,2.236964,2.139209, in m2000-5 hdg=45 spd=487 pkts=1/0 
    Canseco at 40.804990,1.186377,33061, orien -2.244615,1.214249,1.228393, in m2000-5 hdg=66 spd=655 pkts=2/0 Est=652 kt 0 nm 

    from : http://stackoverflow.com/questions/24026418/how-to-convert-latitude-and-longitude-to-orientation-yaw-pitch-roll
    d(x,y,z) = (Destination(x,y,z) - Origin(x,y,z))
    double yaw   = -Math.atan2(dx,-dz);
    double pitch = Math.atan2(dy, Math.sqrt((dx * dx) + (dz * dz)));

   ============================================================================= */
typedef double t_Point3D;
typedef double sgdQuat [ 4 ] ;
#define MY_MIN_VAL 0.00000001
#ifndef DEG2RAD
#define DEG2RAD     M_PI / 180.0
#endif
#ifndef RAD2DEG
#define RAD2DEG     180.0 / M_PI
#endif

enum qoff { QX, QY, QZ, QW };

class Point3D
{
public:
    Point3D();
    void clear();
	void Set ( const t_Point3D& X, const t_Point3D& Y, const t_Point3D& Z );
	t_Point3D m_X;
	t_Point3D m_Y;
	t_Point3D m_Z;

};

Point3D::Point3D()
{
    clear();
}

void Point3D::clear ()
{
	m_X = 0.0;
	m_Y = 0.0;
	m_Z = 0.0;
}
void Point3D::Set ( const t_Point3D& X, const t_Point3D& Y, const t_Point3D& Z )
{
    m_X = X;
    m_Y = Y;
    m_Z = Z;
}

class dQuat 
{
public:
    dQuat();
    dQuat(double x, double y, double z, double w) { data[0] = x; data[1] = y; data[2] = z; data[3] = w; }
    void clear();
    double x() { return data[QX]; }
    double y() { return data[QY]; }
    double z() { return data[QZ]; }
    double w() { return data[QW]; }

    double data[4];
};

dQuat::dQuat()
{
    clear();
}
void dQuat::clear()
{
    data[QX] = 0.0;
    data[QY] = 0.0;
    data[QZ] = 0.0;
    data[QW] = 0.0;
}

double m_dot(const Point3D & v1, const Point3D & v2)
{ return (v1.m_X*v2.m_X) + (v1.m_Y*v2.m_Y) + (v1.m_Z*v2.m_Z); }

double m_norm(const Point3D & v)
{ return sqrt(m_dot(v, v)); }

// { return SGQuat<T>(-v(0), -v(1), -v(2), v(3)); }
sgdQuat *m_conj(const sgdQuat & v)
{
    static sgdQuat q;
    q[0] = -v[0];
    q[1] = -v[1];
    q[2] = -v[2];
    q[3] =  v[3];
    return &q;
}

/// write the euler angles into the references
void m_getEulerRad(sgdQuat *pq, double *zRad, double *yRad, double *xRad)
{
    sgdQuat q;
    memcpy(&q,pq,sizeof(sgdQuat));
    double sqrQW = q[QW] * q[QW];   // w()*w();
    double sqrQX = q[QX] * q[QX];   // x()*x();
    double sqrQY = q[QY] * q[QY];   // y()*y();
    double sqrQZ = q[QZ] * q[QZ];   // z()*z();

    double num = 2 * (q[QY]*q[QZ] + q[QW]*q[QX]); // 2*(y()*z() + w()*x());
    double den = sqrQW - sqrQX - sqrQY + sqrQZ;
    if (fabs(den) <= MY_MIN_VAL &&
        fabs(num) <= MY_MIN_VAL)
      *xRad = 0;
    else
      *xRad = atan2(num, den);

    double tmp = 2 * (q[QX]*q[QZ] - q[QW]*q[QY]); // 2*(x()*z() - w()*y());
    if (tmp <= -1)
      *yRad = 0.5 * M_PI;   // SGMisc<T>::pi();
    else if (1 <= tmp)
      *yRad = - (0.5 * M_PI);    // SGMisc<T>::pi();
    else
      *yRad = -asin(tmp);

    num = 2 * (q[QX]*q[QY] + q[QW]*q[QZ]);    // 2*(x()*y() + w()*z());
    den = sqrQW + sqrQX - sqrQY - sqrQZ;
    if (fabs(den) <= MY_MIN_VAL &&
        fabs(num) <= MY_MIN_VAL)
      *zRad = 0;
    else {
      double psi = atan2(num, den);
      if (psi < 0)
        psi += 2 * M_PI;    //SGMisc<T>::pi();
      *zRad = psi;
    }
}

void m_getEulerDeg(sgdQuat *q, double *zDeg, double *yDeg, double *xDeg)
{
    double zRad,yRad,xRad;
    m_getEulerRad(q, &zRad, &yRad, &xRad);
    *zDeg = RAD2DEG * zRad;
    *yDeg = RAD2DEG * yRad;
    *xDeg = RAD2DEG * xRad;
}

/// Scalar multiplication
//template<typename S, typename T>
//inline
//SGVec3<T>
//operator*(S s, const SGVec3<T>& v)
//{ return SGVec3<T>(s*v(0), s*v(1), s*v(2)); }
Point3D *scalar_mult( double s, const Point3D & v )
{
    static Point3D p;
    p.m_X = s * v.m_X;
    p.m_Y = s * v.m_Y;
    p.m_Z = s * v.m_Z;
    return &p;
}

/// Return a quaternion from real and imaginary part
//  static SGQuat fromRealImag(T r, const SGVec3<T>& i)
//  {
//    SGQuat q;
//    q.w() = r;
//    q.x() = i.x();
//    q.y() = i.y();
//    q.z() = i.z();
//    return q;
//  }

  /// Create a quaternion from the angle axis representation where the angle
  /// is stored in the axis' length
static sgdQuat *m_fromAngleAxis(const Point3D & axis)
{
    static sgdQuat q;
    double nAxis = m_norm(axis);
    if (nAxis < MY_MIN_VAL) {
        q[0] = 0.0;
        q[1] = 0.0;
        q[2] = 0.0;
        q[3] = 0.0;
        return &q;
    }
    double angle2 = 0.5 * nAxis;
    double r    = cos(angle2);
    double sin2 = sin(angle2);
    double val  = sin2 / nAxis;
    Point3D *i = scalar_mult( val, axis );
    //q.w() = r;
    //q.x() = i.x();
    //q.y() = i.y();
    //q.z() = i.z();
    q[QW] = r;
    q[QX] = i->m_X;
    q[QY] = i->m_Y;
    q[QZ] = i->m_Z;
    return &q;
}

static sgdQuat *m_fromLonLatRad(double lon, double lat)
{
    static sgdQuat q;
    double zd2 = 0.5*lon;
    double yd2 = (-0.25 * M_PI) - (0.5 *lat);
    double Szd2 = sin(zd2);
    double Syd2 = sin(yd2);
    double Czd2 = cos(zd2);
    double Cyd2 = cos(yd2);
    q[3] = Czd2*Cyd2;   // w
    q[0] = -Szd2*Syd2;  // x
    q[1] = Czd2*Syd2;   // y
    q[2] = Szd2*Cyd2;   // z
    return &q;
}

sgdQuat *m_mult(const sgdQuat & v1, const sgdQuat & v2)
{
    static sgdQuat v;
    //v.x() = v1.w()*v2.x() + v1.x()*v2.w() + v1.y()*v2.z() - v1.z()*v2.y();
    v[QX]   = v1[QW]*v2[QX] + v1[QX]*v2[QW] + v1[QY]*v2[QZ] - v1[QZ]*v2[QY];
    //v.y() = v1.w()*v2.y() - v1.x()*v2.z() + v1.y()*v2.w() + v1.z()*v2.x();
    v[QY]   = v1[QW]*v2[QY] - v1[QX]*v2[QZ] + v1[QY]*v2[QW] + v1[QZ]*v2[QX];
    //v.z() = v1.w()*v2.z() + v1.x()*v2.y() - v1.y()*v2.x() + v1.z()*v2.w();
    v[QZ]   = v1[QW]*v2[QZ] + v1[QX]*v2[QY] - v1[QY]*v2[QX] + v1[QZ]*v2[QW];
    //v.w() = v1.w()*v2.w() - v1.x()*v2.x() - v1.y()*v2.y() - v1.z()*v2.z();
    v[QW]   = v1[QW]*v2[QW] - v1[QX]*v2[QX] - v1[QY]*v2[QY] - v1[QZ]*v2[QZ];
    return &v;
}

sgdQuat *m_mult2(const sgdQuat & v1, const sgdQuat & v2) 
{
    static sgdQuat v;
    v[QX] = v1[QW]*v2[QX] + v1[QX]*v2[QW] + v1[QY]*v2[QZ] - v1[QZ]*v2[QY];
    v[QY] = v1[QW]*v2[QY] - v1[QX]*v2[QZ] + v1[QY]*v2[QW] + v1[QZ]*v2[QX];
    v[QZ] = v1[QW]*v2[QZ] + v1[QX]*v2[QY] - v1[QY]*v2[QX] + v1[QZ]*v2[QW];
    v[QW] = v1[QW]*v2[QW] - v1[QX]*v2[QX] - v1[QY]*v2[QY] - v1[QZ]*v2[QZ];
    return &v;
}

void m_euler_get( double lat, double lon, double ox, double oy, double oz, double *phead, double *ppitch, double *proll )
{
    Point3D v;
    v.Set( ox, oy, oz );
    sgdQuat *recOrient = m_fromAngleAxis(v);
    // ESPRTF("From Point3D %s, got sgdQuat %s\n", get_point3d_stg2(&v), get_quat_stg2(recOrient));
    double lat_rad, lon_rad;
    lat_rad = lat * DEG2RAD;
    lon_rad = lon * DEG2RAD;
    sgdQuat *qEc2Hl = m_fromLonLatRad(lon_rad, lat_rad);
    // ESPRTF("From lat/lon %f,%f, rad %f,%f, fromLonLatRad %s\n", lat, lon, lat_rad, lon_rad, get_quat_stg(qEc2Hl));
    sgdQuat *con = m_conj(*qEc2Hl);
    sgdQuat *rhlOr = m_mult(*con, *recOrient);
    // ESPRTF("From quat_conj %s, from mult_quats %s\n", get_quat_stg(con), get_quat_stg(rhlOr));
    m_getEulerDeg(rhlOr, phead, ppitch, proll );
    // ESPRTF("getEulerDeg returned h=%f, p=%f, r=%f\n", *phead, *ppitch, *proll);
}

/* ===========================================================
   Sample data
FRST AirChav at 53.362321,-2.257193,273, orien -2.882832,1.458704,1.007460, in CRJ1000 hdg=231 spd=3 pkts=1/0 
FRST OK-JVK at 47.802738,11.996773,19171, orien -1.121633,-2.214272,0.591599, in 777-200 hdg=65 spd=427 pkts=1/0 
FRST D-AHGM at -5.514906,-34.887578,200, orien -1.121828,-1.020800,0.361874, in DO-J-II-r hdg=61 spd=103 pkts=1/0 
FRST Hueq-22 at 37.550137,-122.236544,10208, orien 1.295645,2.458253,1.364842, in Bravo hdg=294 spd=326 pkts=1/0 
FRST TICO at 5.883552,-74.843681,24344, orien -1.386105,1.211009,1.523141, in A330-223 hdg=189 spd=492 pkts=1/0 
    ========================================================== */
typedef struct tagTD {
    const char *t1;
    double lat,lon,alt;
    const char *t2;
    double ox,oy,oz;
    const char *t3;
    double exp;
    const char *t4;
}TD, *PTD;

static TD test_data[] = {
    { "FRST AirChav at", 53.362321,-2.257193,273, "orien", -2.882832,1.458704,1.007460, "in CRJ1000 hdg=",231,"spd=3 pkts=1/0" },
    { "FRST OK-JVK at", 47.802738,11.996773,19171, "orien", -1.121633,-2.214272,0.591599, "in 777-200 hdg=", 65, "spd=427 pkts=1/0" },
    { "FRST D-AHGM at", -5.514906,-34.887578,200, "orien", -1.121828,-1.020800,0.361874, "in DO-J-II-r hdg=", 61, "spd=103 pkts=1/0" },
    { "FRST Hueq-22 at", 37.550137,-122.236544,10208, "orien", 1.295645,2.458253,1.364842, "in Bravo hdg=", 294, "spd=326 pkts=1/0" },
    { "FRST TICO at", 5.883552,-74.843681,24344, "orien", -1.386105,1.211009,1.523141, "in A330-223 hdg=", 189, "spd=492 pkts=1/0" },
    { 0 }
};

void compare_maths()
{
    double lat,lon,alt;
    float ox,oy,oz;
    float heading, pitch, roll, exp;
    double hdg,pit,rol;
    int diff;
    PTD ptd = &test_data[0];
    while (ptd->t1) {
        lat = ptd->lat;
        lon = ptd->lon;
        alt = ptd->alt;
        ox  = ptd->ox;
        oy  = ptd->oy;
        oz  = ptd->oz;
        exp = ptd->exp;
        SPRTF("lat,lon %lf,%lf, and xyz %f,%f,%f\n",
            lat, lon, ox, oy, oz );
#ifdef HAVE_SIMGEAR
        SGVec3f angleAxis(ox,oy,oz);
        SGQuatf ecOrient = SGQuatf::fromAngleAxis(angleAxis);
        SGQuatf qEc2Hl = SGQuatf::fromLonLatRad((float)lon * DEG2RAD,
                                              (float)lat * DEG2RAD);
        // The orientation wrt the horizontal local frame
        SGQuatf hlOr = conj(qEc2Hl)*ecOrient;
        float hDeg, pDeg, rDeg;
        hlOr.getEulerDeg(hDeg, pDeg, rDeg);
        heading = (double)((int)(hDeg + 0.5));
        pitch   = pDeg;
        roll    = rDeg;
        diff = (int)(((heading > exp) ? heading - exp : exp - heading) + 0.5);
        SPRTF("SGM gives p %f, r %f, hdg %d, expect %d, diff %d\n",
            pitch, roll, (int)heading, (int)exp, diff );
#endif
        m_euler_get( lat, lon, ox, oy, oz, &hdg, &pit, &rol );
        heading = (double)((int)(hdg + 0.5));
        pitch   = pit;
        roll    = rol;
        diff = (heading > exp) ? heading - exp : exp - heading;
        SPRTF("ALT gives p %f, r %f, hdg %d, expect %d, diff %d\n",
            pitch, roll, (int)heading, (int)exp, diff );

        ptd++;
    }
    diff = 0.0;

}

void compare_maths2()
{
    double lat,lon,alt;
    float ox,oy,oz;
    float heading, pitch, roll, exp;
    // Victor at 37.405960,-122.042817,42, orien 2.366594,1.994152,1.443017 hdg = 338

    lat = 37.405960;
    lon = -122.042817;
    alt = 42.0;
    ox = 2.366594;
    oy = 1.994152;
    oz = 1.443017;
    exp = 338;
    // FRST AirChav at 53.362321,-2.257193,273, orien -2.882832,1.458704,1.007460, in CRJ1000 hdg=231 spd=3 pkts=1/0 
    lat = 53.362321;
    lon = -2.257193;
    alt = 273;
    // orien 
    ox = -2.882832;
    oy = 1.458704;
    oz = 1.007460;
    // in CRJ1000 hdg=
    exp = 231;
    SPRTF("lat,lon %lf,%lf, and xyz %f,%f,%f\n",
        lat, lon, ox, oy, oz );
#ifdef HAVE_SIMGEAR
    SGVec3f angleAxis(ox,oy,oz);
    SGQuatf ecOrient = SGQuatf::fromAngleAxis(angleAxis);
    SGQuatf qEc2Hl = SGQuatf::fromLonLatRad((float)lon * DEG2RAD,
                                          (float)lat * DEG2RAD);
    // The orientation wrt the horizontal local frame
    SGQuatf hlOr = conj(qEc2Hl)*ecOrient;
    float hDeg, pDeg, rDeg;
    hlOr.getEulerDeg(hDeg, pDeg, rDeg);
    heading = hDeg;
    pitch   = pDeg;
    roll    = rDeg;
    SPRTF("SGM gives p %f, r %f, hdg %d, expect %d\n",
        pitch, roll, (int)(heading + 0.5), (int)exp );
#endif
    double hdg,pit,rol;
    m_euler_get( lat, lon, ox, oy, oz, &hdg, &pit, &rol );
    heading = hdg;
    pitch   = pit;
    roll    = rol;
    SPRTF("ALT gives p %f, r %f, hdg %d, expect %d\n",
        pitch, roll, (int)(heading + 0.5), (int)exp );

}

////////////////////////////////////////////////////////////////////
// from : http://forums.codeguru.com/showthread.php?194400-Distance-between-point-and-line-segment

void DistanceFromLine(double cx, double cy, double ax, double ay ,
					  double bx, double by, double &distanceSegment,
					  double &distanceLine)
{

	//
	// find the distance from the point (cx,cy) to the line
	// determined by the points (ax,ay) and (bx,by)
	//
	// distanceSegment = distance from the point to the line segment
	// distanceLine = distance from the point to the line (assuming
	//					infinite extent in both directions
	//

/*

Subject 1.02: How do I find the distance from a point to a line?


    Let the point be C (Cx,Cy) and the line be AB (Ax,Ay) to (Bx,By).
    Let P be the point of perpendicular projection of C on AB.  The parameter
    r, which indicates P's position along AB, is computed by the dot product 
    of AC and AB divided by the square of the length of AB:
    
    (1)     AC dot AB
        r = ---------  
            ||AB||^2
    
    r has the following meaning:
    
        r=0      P = A
        r=1      P = B
        r<0      P is on the backward extension of AB
        r>1      P is on the forward extension of AB
        0<r<1    P is interior to AB
    
    The length of a line segment in d dimensions, AB is computed by:
    
        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 + ... + (Bd-Ad)^2)

    so in 2D:   
    
        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 )
    
    and the dot product of two vectors in d dimensions, U dot V is computed:
    
        D = (Ux * Vx) + (Uy * Vy) + ... + (Ud * Vd)
    
    so in 2D:   
    
        D = (Ux * Vx) + (Uy * Vy) 
    
    So (1) expands to:
    
            (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
        r = -------------------------------
                          L^2

    The point P can then be found:

        Px = Ax + r(Bx-Ax)
        Py = Ay + r(By-Ay)

    And the distance from A to P = r*L.

    Use another parameter s to indicate the location along PC, with the 
    following meaning:
           s<0      C is left of AB
           s>0      C is right of AB
           s=0      C is on AB

    Compute s as follows:

            (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
        s = -----------------------------
                        L^2


    Then the distance from C to P = |s|*L.

*/


	double r_numerator = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
	double r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
	double r = r_numerator / r_denomenator;
//
    double px = ax + r*(bx-ax);
    double py = ay + r*(by-ay);
//     
    double s =  ((ay-cy)*(bx-ax)-(ax-cx)*(by-ay) ) / r_denomenator;

	distanceLine = fabs(s)*sqrt(r_denomenator);

//
// (xx,yy) is the point on the lineSegment closest to (cx,cy)
//
	double xx = px;
	double yy = py;

	if ( (r >= 0) && (r <= 1) )
	{
		distanceSegment = distanceLine;
	}
	else
	{

		double dist1 = (cx-ax)*(cx-ax) + (cy-ay)*(cy-ay);
		double dist2 = (cx-bx)*(cx-bx) + (cy-by)*(cy-by);
		if (dist1 < dist2)
		{
			xx = ax;
			yy = ay;
			distanceSegment = sqrt(dist1);
		}
		else
		{
			xx = bx;
			yy = by;
			distanceSegment = sqrt(dist2);
		}


	}

	return;
}

/* --------------------------------------------------
    nlat[0],nlon[0]             nlat[1],nlon[1]
                    clat,clon       
    nlat[3],nlon[3]             nlat[2],nlon[2]

   See http://stackoverflow.com/a/1253545/1964221 for the answer to this question.

    Latitude: 1 deg = 110.54 km
    Longitude: 1 deg = 111.320 * cos(latitude) km

    To go the other direction, I wonder if we can do this(my algebra is rusty):
    Latitude: 1 km = 1 deg / 110.54 km
    Longitude: 1 km = 1 deg / (111.320 * cos(latitude) km) 

    To keep it really straight forward, we should assume the earth is spherical, 
    so that the answer is independent of location. I'm using mean radius 3,958.761 miles.

    One radian is the angle for a line 3,958.761 miles long, so the angle for a 
    line m miles long is m/3,958.761 radians. Use Math.toDegrees if you want 
    the angle in degrees.

  -------------------------------------------------- */
void test_dist3()
{
#ifdef HAVE_SIMGEAR
    double clat = -31.696845765;
    double clon = 148.636770758;
    //double mdist = 10000;
    double dist1, dist2, az1, az2, seg;
    int i, res;
    double nlat[4];
    double nlon[4];
    i = 0;
    nlat[i] = clat + 0.5;
    nlon[i] = clon - 0.5;
    i++;
    nlat[i] = clat + 0.5;
    nlon[i] = clon + 0.5;
    i++;
    nlat[i] = clat - 0.5;
    nlon[i] = clon + 0.5;
    i++;
    nlat[i] = clat - 0.5;
    nlon[i] = clon - 0.5;
    // have 4 points
    SPRTF("Square: ");
    for (i = 0; i < 4; i++) {
        SPRTF("%d: %lf,%lf ", i, nlat[i], nlon[i]);
    }
    SPRTF("\n");
    DistanceFromLine(nlon[1],nlat[1], 
        nlon[0],nlat[0], nlon[3],nlat[3],
        dist1, seg );
    SPRTF("DP: Distance of pt %lf,%lf to line %lf,%lf - %lf,%lf is %lf (seg=%lf)\n",
        nlat[1],nlon[1], nlat[0],nlon[0], nlat[3],nlon[3], dist1, seg );

    res = sg_geo_inverse_wgs_84( nlat[0],nlon[0], nlat[1],nlon[1], &az1, &az2, &dist2 );
    SPRTF("SG: Distance of pt %lf,%lf to pt %lf,%lf is %lf (az=%lf)\n",
        nlat[1],nlon[1], nlat[0],nlon[0], dist2, az1 );


    SPRTF("\n");
#endif // #ifdef HAVE_SIMGEAR
}

void test_dist2()
{
    SPRTF("%s: *** WARNING *** this test_dist2() test is still NOT valid! WIP!\n",module);
    double lat1 = -31.696845765;
    double lon1 = 148.636770758;
    double lat2 = -33.949273000;
    double lon2 = 151.181346833;
    //my ($p3,$p123,$dist,$az1,$az2,$res,$clat,$clon,$nlat,$nlon,$factor);
    double mdist = 10000;
    double dist, az1, az2;
    int res;
    //my $p1 = [$lon1,$lat1]; # YGIL
    //my $p2 = [$lon2,$lat2]; # YSSY
    //prt("p1 = ".join(",",@{$p1}).", p2 = ".join(",",@{$p2})."\n");
    double tot_dist = 0;
    //# anno 149.12444400 -33.38305600 NDB ORANGE NDB
    double orlat = -33.38305600;
    double orlon = 149.12444400;
    test_dist3();
    //$p3 = [$orlon,$orlat]; # Orange
    //$p123 = [$p1,$p2,$p3];
    //###my $dist = DistanceToLine($p1,$p2,$p3);
    //$dist = DistanceToLine($p123);
    //void DistanceFromLine(double cx, double cy, double ax, double ay ,
	//				  double bx, double by, double &distanceSegment,
	//				  double &distanceLine)
    DistanceFromLine(orlon, orlat, lon1, lat1,
					  lon2, lat2, dist, az1 );

    SPRTF("ORANGE NDB: Dist %lf (%lf)\n",dist, az1);
    tot_dist += abs(dist);
    //# anno 150.83103300 -32.03474700 NDB SCONE NDB
    double sclat = -32.03474700;
    double sclon = 150.83103300;
    //$p3 = [$sclon,$sclat]; # Scone
    //$p123 = [$p1,$p2,$p3];
    //$dist = DistanceToLine($p123);
    DistanceFromLine(sclon, sclat, lon1, lat1,
					  lon2, lat2, dist, az1 );

    SPRTF("SCONE NDB: Dist %lf (%lf)\n",dist, az1);
    //prt("SCONE NDB: Dist $dist\n");
    tot_dist += abs(dist);
#ifdef HAVE_SIMGEAR
    //$res = fg_geo_inverse_wgs_84( $orlat, $orlon, $sclat, $sclon, \$az1, \$az2, \$dist );
    dist = SGDistance_km( orlat, orlon, sclat, sclon );
    az1 = distance_km( orlat, orlon, sclat, sclon );
    double factor = dist / tot_dist;
    //prt("fg dist $dist, sum $tot_dist, factor $factor\n");
    SPRTF("fg dist %lf (%lf), sum %lf, factor %lf\n", dist, az1, tot_dist, factor);
#endif // HAVE_SIMGEAR
    //# anno 149.59368900 -32.56092800 VOR MUDGEE VOR
    double mulat = -32.56092800;
    double mulon = 149.59368900;
    //$p3 = [$mulon,$mulat]; # Mudgee
    //$p123 = [$p1,$p2,$p3];
    //$dist = DistanceToLine($p123);
    //prt("MUDGEE VOR: Dist $dist\n");
    DistanceFromLine(mulon, mulat, lon1, lat1,
					  lon2, lat2, dist, az1 );

    SPRTF("MDUGEE VOR NDB: Dist %lf (%lf)\n",dist, az1);
#ifdef HAVE_SIMGEAR
    double clat,clon;
    res = sg_geo_inverse_wgs_84( lat1, lon1, lat2, lon2, &az1, &az2, &dist );
    res = sg_geo_direct_wgs_84( lat1, lon1, az1, (dist/2), &clat, &clon, &az2 );
    //$p3 = [$clon,$clat]; # Center
    //$p123 = [$p1,$p2,$p3];
    //$dist = DistanceToLine($p123);
    DistanceFromLine(clon, clat, lon1, lat1,
					  lon2, lat2, dist, az2 );
    SPRTF("Center: Dist %lf\n", dist);
    double nlat, nlon;

    az1 += 90;
    if (az1 >= 360)
        az1 -= 360;
    res = sg_geo_direct_wgs_84( clat, clon, az1, mdist, &nlat, &nlon, &az2 );
    //$p3 = [$nlon,$nlat]; # Center
    //$p123 = [$p1,$p2,$p3];
    //$dist = abs(DistanceToLine($p123));
    DistanceFromLine(nlon, nlat, lon1, lat1,
					  lon2, lat2, dist, az1 );
    mdist /= 1000;
    factor = abs(mdist / dist);
    SPRTF("Known: SG %lf, Dist %lf, factor %lf\n", mdist, dist, factor);
#endif // HAVE_SIMGEAR

    SPRTF("%s: *** WARNING *** this test_dist2() test is still NOT valid! WIP!\n",module);
    SPRTF("%s: The 'units' from the DistanceFromLine() service need to be corrected!\n",module);

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



// eof = test-math.cxx
