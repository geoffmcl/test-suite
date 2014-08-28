/*\
 * test-color.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * from : http://rosettacode.org/wiki/Bitmap/Flood_fill#First_example
\*/

#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <math.h>
#ifdef _MSC_VER
#include <WinSock2.h>   // includes <windows.h>
#else
#include <float.h> // for DBL_MAX, ...
#include <string.h> // for memset, ...
#include <stdlib.h> // for exit(), ...
#endif  // _MSC_VER
#include "sprtf.hxx"
#include "test-color.hxx"

static const char *module = "test-color";

static int check_me()
{
    int i;
    SPRTF("%s: Any key to continue!\n", module);
    getchar();
    i = 0;
    return i;
}

// hmmm, distance between two colors - interesting
double color_distance( rgb_color_p a, rgb_color_p b )
{
  return sqrt( (double)(a->red - b->red)*(a->red - b->red) +
	       (double)(a->green - b->green)*(a->green - b->green) +
	       (double)(a->blue - b->blue)*(a->blue - b->blue) ) / (256.0*sqrt(3.0));
}



//////////////////////////////////////////////////////////////////
// extracted from test.cxx
// test color
#ifndef byte
typedef unsigned char byte;
#endif

#ifdef _WINDOWS_
#define rgb RGB
#define getRVal GetRValue
#define getGVal GetGValue
#define getBVal GetBValue
#else
#define rgb(r,g,b) ((unsigned int)(((byte)(r)|((unsigned short)((byte)(g))<<8))|(((unsigned int)(byte)(b))<<16)))
#define loByte(w)           ((byte)(((unsigned int)(w)) & 0xff))
#define hiByte(w)           ((byte)((((unsigned int)(w)) >> 8) & 0xff))

#define getRVal(rgb)      (loByte(rgb))
#define getGVal(rgb)      (loByte(((unsigned short)(rgb)) >> 8))
#define getBVal(rgb)      (loByte((rgb)>>16))
#endif

#define MAX_COLORS 26
#define ZERO_COLOR 20

// add grey above 5000 meters - see SNOWLINE table
// TODO: Maybe this should be less than 5000, maybe 3000???
HGT2COLOR hgtColor3[MAX_COLORS] = {
    // =====================================
    { 7549, 9000	, 0xFFFFFF, 0, 0, 0, 0 },
    { 6493, 7548	, 0xCCCCCC, 0, 0, 0, 0 },
    { 5662, 6492    , 0x999999, 0, 0, 0, 0 },
    { 4977, 5661	, 0x666666, 0, 0, 0, 0 },
    // above 5000 - go gray to white
    { 4395, 4976	, 0xFF0090, 0, 0, 0, 0 },
    { 3888, 4394	, 0xFF0060, 0, 0, 0, 0 },
    { 3439, 3887	, 0xFF0040, 0, 0, 0, 0 },
    { 3037, 3438	, 0xFF0010, 0, 0, 0, 0 },
    { 2672, 3036	, 0xFF0000, 0, 0, 0, 0 },
    { 2338, 2671	, 0xFF1000, 0, 0, 0, 0 },
    // ------------------------------------
    { 2031, 2337	, 0xFF1800, 0, 0, 0, 0 },
    { 1746, 2030	, 0xFF2000, 0, 0, 0, 0 },
    { 1480, 1745	, 0xFF2800, 0, 0, 0, 0 },
    { 1232, 1479	, 0xFF3800, 0, 0, 0, 0 },
    {  998, 1231	, 0xFF4000, 0, 0, 0, 0 },
    {  777,  997	, 0xFF4C00, 0, 0, 0, 0 },
    {  568,  776	, 0xFF7E00, 0, 0, 0, 0 },
    { 370,  567	, 0xFFB000, 0, 0, 0, 0 },
    { 181, 369	, 0xFFFF00, 0, 0, 0, 0 },
    { 1, 180	, 0x00FF00, 0, 0, 0, 0 },
    // ======================================
    { 0, 0	, 0x0000FF, 0, 0, 0, 0 },
    // this should be REVERSED
    //{ -1, -20	, 0x3300FF, 0, 0, 0, 0 },
    //{ -21, -40	, 0x6600FF, 0, 0, 0, 0 },
    //{ -41, -60	, 0x9900FF, 0, 0, 0, 0 },
    //{ -61, -80	, 0xCC00FF, 0, 0, 0, 0 },
    //{ -81, -11000 , 0xFF00FF, 0, 0, 0, 1 }
    { -100, -1	 , 0x3300FF, 0, 0, 0, 0 },
    { -200, -101 , 0x6600FF, 0, 0, 0, 0 },
    { -400, -201 , 0x9900FF, 0, 0, 0, 0 },
    { -2000, -401 , 0xCC00FF, 0, 0, 0, 0 },
    { -11000,-2001, 0xFF00FF, 0, 0, 0, 1 }
};

// an idea to exxpand the table
bool done_table_expansion = false;
bool set_color_expansion = true;
static PHGT2COLOR pct = hgtColor3;
typedef struct tagMAXMINCOLOR {
    int max, min;
    unsigned int colr;
}MAXMINCOLOR, *PMAXMINCOLOR;

typedef std::vector<MAXMINCOLOR> vMMC;

static int pc_table_cnt = MAX_COLORS;
static int pc_zero_cnt = ZERO_COLOR;

char *get_color_stg( unsigned int colr )
{
    byte r,g,b;
    char *cp = GetNxtBuf();
    r = getRVal(colr);
    g = getGVal(colr);
    b = getBVal(colr);
    //sprintf(cp,"#%02X%02X%02X", r, g, b);
    //sprintf(tx,"#%02X%02X%02X", 255 - r, 255 - g, 255 - b);
    sprintf(cp,"#%02X%02X%02X", b, g, r);
    return cp;
}
char *get_color_stg2( rgb_color_p colr )
{
    byte r,g,b;
    char *cp = GetNxtBuf();
    r = colr->red;
    g = colr->green;
    b = colr->blue;
    //sprintf(cp,"#%02X%02X%02X", r, g, b);
    //sprintf(tx,"#%02X%02X%02X", 255 - r, 255 - g, 255 - b);
    sprintf(cp,"#%02X%02X%02X", b, g, r);
    return cp;
}

void show_distances()
{
    int i, i2;
    PHGT2COLOR ph2c1, ph2c2;
    rgb_color a, b;
    double dist, total = 0.0;
    rgb_color mina, minb, maxa, maxb;
    double min_d = DBL_MAX;
    double max_d = DBL_MIN;

    for (i = 0; i < MAX_COLORS; i++) {
        i2 = i + 1;
        if (i2 == MAX_COLORS)
            i2 = 0;
        ph2c1 = &hgtColor3[i];
        ph2c2 = &hgtColor3[i2];
        a.red = getRVal(ph2c1->color);
        a.green = getGVal(ph2c1->color);
        a.blue = getBVal(ph2c1->color);
        b.red = getRVal(ph2c2->color);
        b.green = getGVal(ph2c2->color);
        b.blue = getBVal(ph2c2->color);
        dist = color_distance( &a, &b );
        SPRTF("dist %s to %s = %lf\n", get_color_stg(ph2c1->color), get_color_stg(ph2c2->color), dist );
        total += dist;
        if (dist < min_d) {
            min_d = dist;
            mina = a;
            minb = b;
        }
        if (dist > max_d) {
            max_d = dist;
            maxa = a;
            maxb = b;
        }
    }
    //dist = total / (double)MAX_COLORS;
    dist = (min_d + max_d) / (double)2;
    SPRTF("Av dist %lf of %d color compares\n", dist, MAX_COLORS );
    SPRTF("min dist %s to %s = %lf\n", get_color_stg2(&mina), get_color_stg2(&minb), min_d );
    SPRTF("max dist %s to %s = %lf\n", get_color_stg2(&maxa), get_color_stg2(&maxb), max_d );

}

void do_table_expansion()
{
    bool verb = false;
    PHGT2COLOR ct = hgtColor3;
    int cnt = 0;
    int ranges = 0;
    int rng;
    //int last;
    unsigned char r,g,b;
    unsigned char nr,ng,nb;
    unsigned char pr,pg,pb,tmp;
    int rd,gd,bd,md,i;
    unsigned int colr;
    //unsigned int prev;
    int rdec,gdec,bdec;

    short cmin,cmax,pmin,pmax;
    short nxcmin, nxcmax;
    short nmin,nmax;

    int frng;
    int colrdiff = 0;
    double unit;
    colr = 0;
    rng = 0;
    r = g = b = 0;
    cmin = cmax = 0;
    vMMC vmmc;
    MAXMINCOLOR mmc;
    vMMC table;
    int max, ii;
    if (!set_color_expansion)
        return;

    while (1) {
        pr = getRVal(colr);
        pg = getGVal(colr);
        pb = getBVal(colr);
        //prev = colr;
        pmin = cmin;
        pmax = cmax;
        //last = rng;
        // stop at the ZERO ENTRY
        if (ct->min == 0)
            break;
        cmax = ct->max;
        cmin = ct->min;
        rng = (cmax - cmin);
        colr = ct->color;
        r = getRVal(colr);
        g = getGVal(colr);
        b = getBVal(colr);
        if (cnt) {
            // how many color steps are there between this color and the previous
            // keeping the same relative colors...
            if (r > pr) {
                rdec = 1;
                rd = r - pr;
            } else {
                rdec = 0;
                rd = pr - r;
            }
            if (g > pg) {
                gdec = 1;
                gd = g - pg;
            } else {
                gdec = 0;
                gd = pg - g;
            }
            if (b > pb) {
                bdec = 1;
                bd = b - pb;
            } else {
                bdec = 0;
                bd = pb - b;
            }
            if (rd > gd)
                md = rd;
            else
                md = gd;
            if (bd > md)
                md = bd;
            // range is from cmin to pmax
            if (cnt == 1) {
                nxcmin = cmin;
                nxcmax = pmax;
            } else {
                nxcmin = cmin;
                //nxcmax = cmax;
                nxcmax = pmin;
            }

            frng = nxcmax - nxcmin;
            if (verb) SPRTF("%s: maxd %d, from %02X%02X%02X to %02X%02X%02X range %d-%d-%d-%d, %d steps (%d,%d)\n", module,
                md,
                r,g,b,
                pr,pg,pb,
                cmin, cmax, pmin, pmax, frng,
                nxcmin, nxcmax);
            colrdiff += md;
            unit = ((double)frng / (double)(md+1));
            nmin = nxcmin;
            nmax = nmin + (int)(unit + 0.5);
            if (verb) SPRTF("min/max %d %d %02X%02X%02X\n", nmin, nmax, r, g, b );
            vmmc.clear();
            mmc.max = nmax;
            mmc.min = nmin;
            mmc.colr = rgb(b,g,r);
            vmmc.push_back(mmc);
            for (i = 1; i < md; i++) {
                nmin = nxcmin + (int)((unit * (double)i) + 0.5);
                nmax = nmin + (int)(unit + 0.5);
                nr = r;
                if (rd) {
                    // move r to towards pr by an incremental amount
                    tmp = (int)((((double)rd / (double)md) * (double)i) + 0.5);
                    if (rdec)
                        nr = r - tmp;
                    else
                        nr = r + tmp;
                }
                ng = g;
                if (gd) {
                    // move g to towards pg by an incremental amount
                    tmp = (int)((((double)gd / (double)md) * (double)i) + 0.5);
                    if (gdec)
                        ng = g - tmp;
                    else
                        ng = g + tmp;
                }
                nb = b;
                if (bd) {
                    // move b to towards pb by an incremental amount
                    tmp = (int)((((double)bd / (double)md) * (double)i) + 0.5);
                    if (bdec)
                        nb = b - tmp;
                    else
                        nb = b + tmp;

                }
                if (verb) SPRTF("min/max %d %d %02X%02X%02X\n", nmin, nmax, nr, ng, nb );
                mmc.max = nmax;
                mmc.min = nmin;
                mmc.colr = rgb(nb,ng,nr);
                vmmc.push_back(mmc);
            }
            nmax = nxcmax;
            nmin = nmax - (int)(unit + 0.5);
            if (verb) SPRTF("min/max %d %d %02X%02X%02X\n", nmin, nmax, pr, pg, pb );
            mmc.max = nmax;
            mmc.min = nmin;
            mmc.colr = rgb(pb,pg,pr);
            vmmc.push_back(mmc);
            // DRAT, now need to reverse order
            max = (int)vmmc.size();
            for (ii = max - 1; ii >= 0; ii--) {
                mmc = vmmc[ii];
                if ((mmc.max == 0)&&(mmc.min == 0)) {
                    SPRTF("%s: Ugh! A zero entry!\n",module);
                    check_me();
                    continue;
                }
                table.push_back(mmc);
            }

            if (md && verb)
                SPRTF("\n");
        }
        ranges += rng;

        ct++;
        cnt++;
    }

    // TODO: could likewise expand the negatives, but that for later
    verb = true;
    if (verb) SPRTF("%s: Adding the zero and negative ranges...\n", module );
    while (1) {
        // store the previous
        pr = getRVal(colr);
        pg = getGVal(colr);
        pb = getBVal(colr);
        //prev = colr;
        pmin = cmin;
        pmax = cmax;

        cmax = ct->max;
        cmin = ct->min;
        rng = (cmax - cmin);
        colr = ct->color;
        r = getRVal(colr);
        g = getGVal(colr);
        b = getBVal(colr);
        if (cmax == 0) {
            // just ADD this zero value
            mmc.max = cmax;
            mmc.min = cmin;
            mmc.colr = rgb(b,g,r);
            table.push_back(mmc);
        } else {
            // scale between previous and this current
            if (r > pr) {
                rdec = 1;
                rd = r - pr;
            } else {
                rdec = 0;
                rd = pr - r;
            }
            if (g > pg) {
                gdec = 1;
                gd = g - pg;
            } else {
                gdec = 0;
                gd = pg - g;
            }
            if (b > pb) {
                bdec = 1;
                bd = b - pb;
            } else {
                bdec = 0;
                bd = pb - b;
            }
            if (rd > gd)
                md = rd;
            else
                md = gd;
            if (bd > md)
                md = bd;
            // range is from cmin to pmax
            nxcmin = cmin;
            //nxcmax = cmax;
            nxcmax = pmin;

            frng = nxcmax - nxcmin;
            if (verb) SPRTF("%s: maxd %d, from %02X%02X%02X to %02X%02X%02X range %d-%d-%d-%d, %d steps (%d,%d)\n", module,
                md,
                r,g,b,
                pr,pg,pb,
                cmin, cmax, pmin, pmax, frng,
                nxcmin, nxcmax);
            colrdiff += md;
            unit = ((double)frng / (double)(md+1));
            nmin = nxcmin;
            nmax = nmin + (int)(unit + 0.5);
            if (verb) SPRTF("min/max %d %d %02X%02X%02X\n", nmin, nmax, r, g, b );
            vmmc.clear();
            mmc.max = nmax;
            mmc.min = nmin;
            mmc.colr = rgb(b,g,r);
            vmmc.push_back(mmc);
            for (i = 1; i < md; i++) {
                nmin = nxcmin + (int)((unit * (double)i) + 0.5);
                nmax = nmin + (int)(unit + 0.5);
                nr = r;
                if (rd) {
                    // move r to towards pr by an incremental amount
                    tmp = (int)((((double)rd / (double)md) * (double)i) + 0.5);
                    if (rdec)
                        nr = r - tmp;
                    else
                        nr = r + tmp;
                }
                ng = g;
                if (gd) {
                    // move g to towards pg by an incremental amount
                    tmp = (int)((((double)gd / (double)md) * (double)i) + 0.5);
                    if (gdec)
                        ng = g - tmp;
                    else
                        ng = g + tmp;
                }
                nb = b;
                if (bd) {
                    // move b to towards pb by an incremental amount
                    tmp = (int)((((double)bd / (double)md) * (double)i) + 0.5);
                    if (bdec)
                        nb = b - tmp;
                    else
                        nb = b + tmp;

                }
                if (verb) SPRTF("min/max %d %d %02X%02X%02X\n", nmin, nmax, nr, ng, nb );
                mmc.max = nmax;
                mmc.min = nmin;
                mmc.colr = rgb(nb,ng,nr);
                vmmc.push_back(mmc);
            }
            nmax = nxcmax;
            nmin = nmax - (int)(unit + 0.5);
            if (verb) SPRTF("min/max %d %d %02X%02X%02X\n", nmin, nmax, pr, pg, pb );
            mmc.max = nmax;
            mmc.min = nmin;
            mmc.colr = rgb(pb,pg,pr);
            vmmc.push_back(mmc);
            // DRAT, now need to reverse order
            max = (int)vmmc.size();
            for (ii = max - 1; ii >= 0; ii--) {
                mmc = vmmc[ii];
                if ((mmc.max == 0)&&(mmc.min == 0)) {
                    SPRTF("%s: Ugh! A zero entry!\n",module);
                    check_me();
                    continue;
                }
                table.push_back(mmc);
            }

            if (md && verb)
                SPRTF("\n");
        }
        ranges += rng;

        if (verb) SPRTF("min/max %d %d %02X%02X%02X\n", cmin, cmax, r, g, b );
        cnt++;
        if (ct->res3 == 1)  // NOTE: special terminator
            break;
        ct++;
    }

    max = (int)table.size();
    if (verb) SPRTF("%s: Output of expanded table of %d elements\n", module, max);
    nxcmin = nxcmax = 0;
    cnt = 0;
    for (ii = 0; ii < max; ii++) {
        cmin = nxcmin;
        cmax = nxcmax;
        mmc = table[ii];
        nxcmin = mmc.min;
        nxcmax = mmc.max;
        if ((nxcmin == cmin)&&
            (nxcmax == cmax))
            continue;
        colr = mmc.colr;
        r = getRVal(colr);
        g = getGVal(colr);
        b = getBVal(colr);
        if (verb) SPRTF("min/max %d %d %02X%02X%02X\n", nxcmin, nxcmax, r, g, b );
        cnt++;
    }

    if (verb) SPRTF("%s: Will build table with total %d records...\n", module, cnt );

    PHGT2COLOR phgtc = new HGT2COLOR[cnt];
    if (!phgtc) {
        SPRTF("%s: Memory allocation FAILED!\n");
        check_me();
        return;
    }
    // clear it all to zero
    memset(phgtc,0,sizeof(HGT2COLOR) * cnt);

    ct = phgtc; // using moving pointer
    cnt = 0;
    int zpos = 0;
    for (ii = 0; ii < max; ii++) {
        cmin = nxcmin;
        cmax = nxcmax;
        mmc = table[ii];    // get generated entry
        nxcmin = mmc.min;
        nxcmax = mmc.max;
        if ((nxcmin == cmin)&&
            (nxcmax == cmax)) {
            continue;
        }
        colr = mmc.colr;
        r = getRVal(colr);
        g = getGVal(colr);
        b = getBVal(colr);
        // FIX20140502 - Wow how did this BUG continue
        //NOT ct->min = cmin; ct->max = cmax; BUT
        ct->min = nxcmin;
        ct->max = nxcmax;
        // =====================================================
        // TODO: One day MUST fix this stupid REVERSAL of COLORS
        // =====================================================
        ct->color = rgb(b,g,r);
        // ct->colr = colr;
        // SPRTF("min/max %d %d %02X%02X%02X\n", nxcmin, nxcmax, r, g, b );
        if ((cmin == 0)&&(cmax == 0))
            zpos = cnt;
        cnt++;
        ct++;
    }

    // *********************************
    // ready to esablish new table
    SPRTF("%s: Expanded original table from %d entries to %d entries.\n", module, pc_table_cnt, cnt);
    pc_table_cnt = cnt;
    pc_zero_cnt = zpos;
    pct = phgtc;
    // *********************************
    done_table_expansion = true;
}

PHGT2COLOR get_current_table() 
{
    if (!done_table_expansion) {
        done_table_expansion = true;
        do_table_expansion();
    }
    return pct; 
}

#ifndef EOL
#define EOL << std::endl
#endif

std::string get_color_http()
{
    PHGT2COLOR pct = get_current_table();
    int i, min, max, colm;
    byte r,g,b;
    unsigned int colr;
    int wrap = 4;
    std::stringstream html;
    html << "<html>" EOL;
    html << " <head>" EOL;
    html << "  <title>Current Color Table</title>" EOL;
    html << "  <script language=\"JavaScript\" type=\"text/javascript\">" EOL;
    html << "// change background" EOL;
    html << "  function sbgc(hex) { document.bgColor=hex; }" EOL;
    html << "  </script>" EOL;
    html << " </head>" EOL;
    html << " <body>" EOL;
    html << " <div align=\"center\">" EOL;
    html << "  <table border\"1\" align=\"center\" summary=\"view of images\">" EOL;
    html << "  <caption>Current Color Table</caption>" EOL;
    char *cp = GetNxtBuf();
    char *tx = GetNxtBuf();
    html << "    <tr>" EOL;
    html << "     <th>Minimum</th>" EOL;
    html << "     <th>Maximum</th>" EOL;
    html << "     <th>Color</th>" EOL;
    if (wrap) {
        i = wrap - 1;
        while(i) {
            html << "     <th>Minimum</th>" EOL;
            html << "     <th>Maximum</th>" EOL;
            html << "     <th>Color</th>" EOL;
            i--;
        }
        html << "    </tr>" EOL;
    } else {
        html << "    </tr>" EOL;
    }
    colm = 0;
    for (i = 0; i < pc_table_cnt; i++) {
        colr = pct[i].color;
        min =  pct[i].min;
        max =  pct[i].max;
        r = getRVal(colr);
        g = getGVal(colr);
        b = getBVal(colr);
        //sprintf(cp,"#%02X%02X%02X", r, g, b);
        //sprintf(tx,"#%02X%02X%02X", 255 - r, 255 - g, 255 - b);
        sprintf(cp,"#%02X%02X%02X", b, g, r);
        sprintf(tx,"#%02X%02X%02X", 255 - b, 255 - g, 255 - r);
        if (wrap) {
            if (colm == 0) {
                html << "    <tr>" EOL;
            }
        } else {
            html << "    <tr>" EOL;
        }
        html << "     <td align=\"right\">" << min << "</td>" EOL;
        html << "     <td align=\"right\">" << max << "</td>" EOL;
        html << "     <td bgcolor=\"" << cp << "\">" EOL;
        html << "      <a href=\"#\" onmouseover=\"sbgc('" << cp << "')\" onmouseout=\"sbgc('#FFFFFF')\">" EOL;
        html << "        <tt><font color=\"" << tx << "\">***** " << cp << " ******</font></tt></a>" EOL;
        html << "     </td>" EOL;
        if (wrap) {
            colm++;
            if (colm == wrap) {
                html << "    </tr>" EOL;
                colm = 0;
            }

        } else {
            html << "    </tr>" EOL;
        }
    }
    if (wrap && colm) {
        while (colm < wrap) {
            html << "     <td>&nbsp;</td>" EOL;
            html << "     <td>&nbsp;</td>" EOL;
            html << "     <td>&nbsp;</td>" EOL;
            colm++;
        }
        html << "    </tr>" EOL;
    }
    html << "   </table>" EOL;
    html << " </div>" EOL;
    html << " </body>" EOL;
    html << "</html>" EOL;
    return html.str();
}


void test_color()
{
    show_distances();
    do_table_expansion();
    std::string html = get_color_http();
    const char *view = "tempcolor.html";
    FILE *fp = fopen(view,"w");
    if (fp) {
        fwrite(html.c_str(),html.size(),1,fp);
        fclose(fp);
        SPRTF("%s: Written %s to view colors...\n", module, view);
    } else {
        SPRTF("%s: Failed to open %s\n", module, view );
    }
    exit(1);
}

//////////////////////////////////////////////////////////////////
// eof = test-color.cxx
