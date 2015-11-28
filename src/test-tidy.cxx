/*\
 * test-tidy.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include "sprtf.hxx"
// other includes
#include "test-tidy.hxx"

static const char *module = "test-tidy";

typedef unsigned long ulong;


// implementation
/* unknown */
#define xxxx                   0u

/* W3C defined HTML/XHTML family document types */
#define HT20                   1u
#define HT32                   2u
#define H40S                   4u
#define H40T                   8u
#define H40F                  16u
#define H41S                  32u
#define H41T                  64u
#define H41F                 128u
#define X10S                 256u
#define X10T                 512u
#define X10F                1024u
#define XH11                2048u
#define XB10                4096u

/* proprietary stuff */
#define VERS_SUN            8192u
#define VERS_NETSCAPE      16384u
#define VERS_MICROSOFT     32768u

/* special flag */
#define VERS_XML           65536u

/* HTML5 */
#define HT50              131072u
#define XH50              262144u

/* compatibility symbols */
#define VERS_UNKNOWN       (xxxx)
#define VERS_HTML20        (HT20)
#define VERS_HTML32        (HT32)
#define VERS_HTML40_STRICT (H40S|H41S|X10S)
#define VERS_HTML40_LOOSE  (H40T|H41T|X10T)
#define VERS_FRAMESET      (H40F|H41F|X10F)
#define VERS_XHTML11       (XH11)
#define VERS_BASIC         (XB10)
/* HTML5 */
#define VERS_HTML5         (HT50|XH50)

/* meta symbols */
#define VERS_HTML40        (VERS_HTML40_STRICT|VERS_HTML40_LOOSE|VERS_FRAMESET)
#define VERS_IFRAME        (VERS_HTML40_LOOSE|VERS_FRAMESET)
#define VERS_LOOSE         (VERS_HTML20|VERS_HTML32|VERS_IFRAME)
#define VERS_EVENTS        (VERS_HTML40|VERS_XHTML11)
#define VERS_FROM32        (VERS_HTML32|VERS_HTML40)
#define VERS_FROM40        (VERS_HTML40|VERS_XHTML11|VERS_BASIC)
#define VERS_XHTML         (X10S|X10T|X10F|XH11|XB10|XH50)

/* all W3C defined document types */
#define VERS_ALL           (VERS_HTML20|VERS_HTML32|VERS_FROM40|XH50|HT50)

/* all proprietary types */
#define VERS_PROPRIETARY   (VERS_NETSCAPE|VERS_MICROSOFT|VERS_SUN)


typedef struct tagFLAGS {
    ulong val;
    const char *name;
}FLAGS, *PFLAGS;

static FLAGS flags[] = {
    { HT20, "HT20" }, //                   1u
    { HT32, "HT32" }, //                   2u
    { H40S, "H40S" }, //                   4u
    { H40T, "H40T" }, //                   8u
    { H40F, "H40F" }, //                  16u
    { H41S, "H41S" }, //                  32u
    { H41T, "H41T" }, //                  64u
    { H41F, "H41F" }, //                 128u
    { X10S, "X10S" }, //                 256u
    { X10T, "X10T" }, //                 512u
    { X10F, "X10F" }, //                1024u
    { XH11, "XH11" }, //                2048u
    { XB10, "XB10" }, //                4096u
    /* proprietary stuff */
    { VERS_SUN, "VER_SUN" }, //            8192u
    { VERS_NETSCAPE, "VERS_NETSCAPE" }, //     16384u
    { VERS_MICROSOFT, "VERS_MICROSOFT" }, //     32768u
    /* special flag */
    { VERS_XML, "VERS_XML" }, //          65536u

    /* HTML5 */
    { HT50, "HT50" }, //              131072u
    { XH50, "XH50" }, //              262144u

    /* LAST */
    { 0, 0 }
};

FLAGS compats[] = {
    /* compatibility symbols */
    // { VERS_UNKNOWN, "VERS_UNKNOWN" }, //       (xxxx)
    { VERS_HTML20, "VERS_HTML20" }, //        (HT20)
    { VERS_HTML32, "VERS_HTML32" }, //        (HT32)
    { VERS_HTML40_STRICT, "VERS_HTML40_STRICT" }, // (H40S|H41S|X10S)
    { VERS_HTML40_LOOSE, "VERS_HTML40_LOOSE" }, //  (H40T|H41T|X10T)
    { VERS_FRAMESET, "VERS_FRAMESET" }, //      (H40F|H41F|X10F)
    { VERS_XHTML11, "VERS_XHTML11" }, //       (XH11)
    { VERS_BASIC, "VERS_BASIC" }, //         (XB10)
    /* HTML5 */
    { VERS_HTML5, "VERS_HTML5" }, //         (HT50|XH50)

    /* meta symbols */
    { VERS_HTML40, "VERS_HTML40" }, //        (VERS_HTML40_STRICT|VERS_HTML40_LOOSE|VERS_FRAMESET)
    { VERS_IFRAME, "VERS_IFRAME" }, //        (VERS_HTML40_LOOSE|VERS_FRAMESET)
    { VERS_LOOSE, "VERS_LOOSE" }, //         (VERS_HTML20|VERS_HTML32|VERS_IFRAME)
    { VERS_EVENTS, "VERS_EVENTS" }, //        (VERS_HTML40|VERS_XHTML11)
    { VERS_FROM32, "VERS_FROM32" }, //        (VERS_HTML32|VERS_HTML40)
    { VERS_FROM40, "VERS_FROM40" }, //        (VERS_HTML40|VERS_XHTML11|VERS_BASIC)
    { VERS_XHTML, "VERS_XHTML" }, //         (X10S|X10T|X10F|XH11|XB10|XH50)

    /* all W3C defined document types */
    { VERS_ALL, "VERS_ALL" }, //           (VERS_HTML20|VERS_HTML32|VERS_FROM40|XH50|HT50)

    /* all proprietary types */
    { VERS_PROPRIETARY, "VERS_PROPRIETARY" }, //   (VERS_NETSCAPE|VERS_MICROSOFT|VERS_SUN)

    /* LAST */
    { 0, 0 }
};

void show_value( ulong in_val )
{
    ulong flg, val = in_val;
    SPRTF("Value = %lu 0x%08lx\n", val, val );
    if (val == 0)
        return;
    PFLAGS  pf = &flags[0];
    SPRTF("Flags: ");
    while (pf->name) {
        if (pf->val & val) {
            SPRTF("%s ", pf->name);
        }
        pf++;
    }
    SPRTF("\n");
    pf = &compats[0];
    SPRTF("Comp: ");
    while (pf->name) {
        flg = pf->val;
        if (flg == val) {
            SPRTF("%s exact!", pf->name);
            break;
        } else if ((val & flg) == flg) {
            SPRTF("%s ", pf->name);
        }
        pf++;
    }
    SPRTF("\n");

}

void test_tidy()
{

	ulong val = 0x0004e000;
    show_value(val);


}

// eof = test-tidy.cxx
