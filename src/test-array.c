// test-array.c
#ifdef _MSC_VER
#pragma warning ( disable : 4996 ) // : This function or variable may be unsafe
#endif
#include <stdlib.h>
#include <stdio.h>
#ifndef __cplusplus
typedef char bool;
#endif
#include "sprtf.hxx"
#include "test-array.h"

static const char *module = "test-array";

#ifndef __cplusplus
#ifdef _MSC_VER
#pragma message("Not C++ == C compile")
#endif
typedef struct Geom      Geom;		/* Complete OOGL object */
typedef struct GeomClass GeomClass;	/* Virtual func tbl for Geom */
typedef struct GeomIter  GeomIter;	/* opaque iteration handle */
#else
#ifdef _MSC_VER
#pragma message("Is a C++ compile")
#endif
struct Geom;
struct GeomClass;
struct GeomIter;
#endif

#ifndef _MSC_VER
///////////////////////////////////////////////////////////////////////////////////
#define GEOMFIELDS							\
  int      geomflags;						\
  int      pdim

#define _putenv putenv
///////////////////////////////////////////////////////////////////////////////////
#else
///////////////////////////////////////////////////////////////////////////////////
#define GEOMFIELDS							\
  int              geomflags;						\
  int              pdim /* does this belong here? */

///////////////////////////////////////////////////////////////////////////////////
#endif

struct Geom { /* common data structures for all Geom's */
  GEOMFIELDS;
};

typedef struct HGeom {  /* This tuple appears in hierarchy objects */
  void *h;
  Geom *g;
} HGeom;


#if 0
/* This is the "common" geom stuff which starts every geom */
#define GEOMFIELDS							\
  REFERENCEFIELDS;       /* magic, ref_count, handle */			\
  struct GeomClass *Class;						\
  Appearance       *ap;							\
  Handle           *aphandle;						\
  int              geomflags;						\
  int              pdim; /* does this belong here? */			\
  FreeListNode     **freelisthead; /* memory management */		\
  /* The following three fields are used to attach data to a geometry	\
   * objects which depends on the position of the Geom in the object	\
   * hierarchy. As a Geom object can be referenced by Handles the Geom	\
   * might occur in many positions inside the hierarchy. See also the	\
   * comment in front of "struct NodeData" below.			\
   *									\
   * The components "ppath" and "ppathlen"   newpl.ppath     = pl->ppath;
  newpl.ppathlen  = pl->ppathlen;
are non-persistent and only	\
   * valid through tree traversal with GeomDraw().			\
   */									\
  DblListNode      pernode;  /* per-node data */			\
  char             *ppath;   /* parent path */				\
  int              ppathlen; /* its length */				\
  /* Also a non-persistent entry: the bsptree for correct rendering of	\
   * translucent objects with alpha-blending. If non-NULL, this is the	\
   * tree for the object sub-hierarchy starting at this object.		\
   */									\
  BSPTree          *bsptree

struct Geom { /* common data structures for all Geom's */
  GEOMFIELDS;
};

typedef struct HGeom {  /* This tuple appears in hierarchy objects */
  Handle *h;
  Geom *g;
} HGeom;

#endif


#ifndef TRANSFORM3_TYPEDEF
# define TRANSFORM3_TYPEDEF 1
typedef float Tm3Coord;
typedef Tm3Coord Transform3[4][4];
typedef	Tm3Coord (*TransformPtr)[4];
#endif

typedef Transform3 Transform;

void drawer_post_xform(int id, Transform T, Geom *g );

void drawer_post_xform(int id, Transform T, Geom *g )
{
    char *cp = getenv("PATH");
    _putenv("THIS=THAT");
    cp = getenv("THIS");
    if (cp)
        printf("Set THIS=%s\n", cp);
    else
        printf("Failed _putenv\n");
}


void test_array()
{
    Transform t1;
    Geom g;
    SPRTF("\n");
    SPRTF("%s: doing test array..\n", module );
    drawer_post_xform(1,t1, &g);
    SPRTF("%s: done test array..\n", module );
	return;
}


// eof
