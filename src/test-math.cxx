/*\
 * test-math.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#ifndef _MSC_VER
#include <stdlib.h> // for abs(), ...
#endif
#include <math.h>
#include <limits>
#include "sprtf.hxx"
#include "utils.hxx"
#include "test-math.hxx"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define RAD2DEG ( 180.0 / M_PI )
#define DEG2RAD ( M_PI / 180.0 )


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

// eof = test-math.cxx
