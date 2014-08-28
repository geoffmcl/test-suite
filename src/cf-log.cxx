// cf-log.cxx


#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#define ADD_PROPERTIES
// #define ADD_HEX_DEBUG

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <time.h>
#ifdef _MSC_VER
#include <io.h>
#endif
#include <map>

#ifdef ADD_CF_LOG_TEST
#include <simgear/compiler.h>
#include <simgear/math/SGGeometry.hxx>
#ifdef ADD_PROPERTIES
#include <simgear/props/props.hxx>
#endif

#include "cf-log.hxx"
#include "sprtf.hxx"
#include "test.hxx"

using namespace std;

static const char *mod_name = "cf-log.cxx";
#define module mod_name

#ifdef _MSC_VER
static const char *log_file = "C:\\Users\\user\\Downloads\\logs\\fgx-cf\\cf_raw.log";
#else
static const char *log_file = "/home/geoff/downloads/cf_raw.log";
#endif
#ifndef SPRTF
#define SPRTF printf
#endif

typedef struct tagPKT {
    char *cp;
    int  len;
}PKT, *PPKT;

typedef vector<PKT> vPKT;

vPKT vPackets;

// forward references
void out_hex(char *from, int hlen, int flag = 0);
double get_pct_1( int cnt, int tot );

bool IsLittleEndian()
{
    static const int EndianTest = 1;
    return (*((char *) &EndianTest ) != 0);
}

inline uint32_t bswap_32(uint32_t x) {
    x = ((x >>  8) & 0x00FF00FFL) | ((x <<  8) & 0xFF00FF00L);
    x = (x >> 16) | (x << 16);
    return x;
}
#define MSWAP32(a) IsLittleEndian() ? bswap_32(a) : a

void out_hex(char *from, int hlen, int flag)
{
    static char _s_hex_pad[16 * 5];
    static char _s_asc_pad[32];
    int i, c, off;
    char *ph = _s_hex_pad;
    char *pa = _s_asc_pad;
    *ph = 0;
    off = 0;
    if ((flag & 0x01)&&(hlen > 4)) {
        uint32_t *pui = (uint32_t *)from;
        i = MSWAP32(*pui);
        SPRTF("First uint32 = %u\n", i);
    }
    for (i = 0; i < hlen; i++) {
        c = ( from[i] & 0xff );
        sprintf(EndBuf(ph),"%02x ", c);
        if ((c < ' ')||(c >= 0x7f))
            c = '.';
        pa[off++] = (char)c;
        if (off == 16) {
            pa[off] = 0;
            SPRTF("%s %s\n", ph, pa);
            off = 0;
            *ph = 0;
        }
    }
    if (off) {
        pa[off] = 0;
        while(off < 16) {
            strcat(ph,"   ");
            off++;
        }
        SPRTF("%s %s\n", ph, pa);
    }
}



///////////////////////////////////////////////////////////////////////
//// crossfeed code
///////////////////////////////////////////////////////////////////////

time_t m_PlayerExpires = 10;     // standard expiration period (seconds)
double m_MinDistance_m = 2000.0;  // started at 100.0;   // got movement (meters)
int m_MinSpdChange_kt = 20;
int m_MinHdgChange_deg = 1;
int m_MinAltChange_ft = 100;

inline uint16_t sg_bswap_16(uint16_t x) {
    x = (x >> 8) | (x << 8);
    return x;
}

inline uint32_t sg_bswap_32(uint32_t x) {
    x = ((x >>  8) & 0x00FF00FFL) | ((x <<  8) & 0xFF00FF00L);
    x = (x >> 16) | (x << 16);
    return x;
}

inline uint64_t sg_bswap_64(uint64_t x) {
    x = ((x >>  8) & 0x00FF00FF00FF00FFLL) | ((x <<  8) & 0xFF00FF00FF00FF00LL);
    x = ((x >> 16) & 0x0000FFFF0000FFFFLL) | ((x << 16) & 0xFFFF0000FFFF0000LL);
    x = (x >> 32) | (x << 32);
    return x;
}


inline bool sgIsLittleEndian() {
    static const int sgEndianTest = 1;
    return (*((char *) &sgEndianTest ) != 0);
}

inline bool sgIsBigEndian() {
    static const int sgEndianTest = 1;
    return (*((char *) &sgEndianTest ) == 0);
}

inline void sgEndianSwap(uint16_t *x) { *x = sg_bswap_16(*x); }
inline void sgEndianSwap(uint32_t *x) { *x = sg_bswap_32(*x); }
inline void sgEndianSwap(uint64_t *x) { *x = sg_bswap_64(*x); }


#define SWAP16(arg) sgIsLittleEndian() ? sg_bswap_16(arg) : arg
#define SWAP32(arg) sgIsLittleEndian() ? sg_bswap_32(arg) : arg
#define SWAP64(arg) sgIsLittleEndian() ? sg_bswap_64(arg) : arg

// magic value for messages
const uint32_t MSG_MAGIC = 0x46474653;  // "FGFS"
// relay magic value
const uint32_t RELAY_MAGIC = 0x53464746;    // GSGF
// protocoll version
const uint32_t PROTO_VER = 0x00010001;  // 1.1

// Message identifiers
#define CHAT_MSG_ID             1
#define RESET_DATA_ID           6
#define POS_DATA_ID             7

// XDR demands 4 byte alignment, but some compilers use8 byte alignment
// so it's safe to let the overall size of a network message be a 
// multiple of 8!
#define MAX_CALLSIGN_LEN        8
#define MAX_CHAT_MSG_LEN        256
#define MAX_MODEL_NAME_LEN      96
#define MAX_PROPERTY_LEN        52

typedef uint32_t    xdr_data_t;      /* 4 Bytes */
typedef uint64_t    xdr_data2_t;     /* 8 Bytes */

// Header for use with all messages sent 
typedef struct tagT_MsgHdr {
    xdr_data_t  Magic;                  // Magic Value
    xdr_data_t  Version;                // Protocoll version
    xdr_data_t  MsgId;                  // Message identifier 
    xdr_data_t  MsgLen;                 // absolute length of message
    xdr_data_t  ReplyAddress;           // (player's receiver address
    xdr_data_t  ReplyPort;              // player's receiver port
    char Callsign[MAX_CALLSIGN_LEN];    // Callsign used by the player
}T_MsgHdr, *PT_MsgHdr;

// Chat message 
struct T_ChatMsg {
    char Text[MAX_CHAT_MSG_LEN];       // Text of chat message
};

// Position message
struct T_PositionMsg {
    char Model[MAX_MODEL_NAME_LEN];    // Name of the aircraft model

    // Time when this packet was generated
    xdr_data2_t time;
    xdr_data2_t lag;

    // position wrt the earth centered frame
    xdr_data2_t position[3];
    // orientation wrt the earth centered frame, stored in the angle axis
    // representation where the angle is coded into the axis length
    xdr_data_t orientation[3];

    // linear velocity wrt the earth centered frame measured in
    // the earth centered frame
    xdr_data_t linearVel[3];
    // angular velocity wrt the earth centered frame measured in
    // the earth centered frame
    xdr_data_t angularVel[3];

    // linear acceleration wrt the earth centered frame measured in
    // the earth centered frame
    xdr_data_t linearAccel[3];
    // angular acceleration wrt the earth centered frame measured in
    // the earth centered frame
    xdr_data_t angularAccel[3];
    // Padding. The alignment is 8 bytes on x86_64 because there are
    // 8-byte types in the message, so the size should be explicitly
    // rounded out to a multiple of 8. Of course, it's a bad idea to
    // put a C struct directly on the wire, but that's a fight for
    // another day...
    xdr_data_t pad;
};

// Property message
struct T_PropertyMsg {
    xdr_data_t id;
    xdr_data_t value;
};

/////////////////////////////////////////////////////////////////
/// PACKET TYPES
/////////////////////////////////////////////////////////////////
enum Packet_Type {
    pkt_Invalid,    // not used
    pkt_InvLen1,    // too short for header
    pkt_InvLen2,    // too short for position
    pkt_InvMag,     // Magic value error
    pkt_InvProto,   // not right protocol
    pkt_InvPos,     // linear value all zero - never seen one!
    pkt_InvHgt,     // alt <= -9990 feet - always when fgs starts
    pkt_InvStg1,    // no callsign after filtering
    pkt_InvStg2,    // no aircraft
    // all ABOVE are INVALID packets
    pkt_First,      // first pos packet
    pkt_Pos,        // pilot position packets
    pkt_Chat,       // chat packets
    pkt_Other,      // should be NONE!!!
    pkt_Discards,   // valid, but due to no time/dist...
    pkt_Max
};

// ==================================================
typedef struct tagPKTSTR {
    Packet_Type pt;
    const char *desc;
    int count;
    int totals;
    void *vp;
}PKTSTR, *PPKTSTR;

static PKTSTR sPktStr[pkt_Max] = {
    { pkt_Invalid, "Invalid",     0, 0, 0 },
    { pkt_InvLen1, "InvLen1",     0, 0, 0 },
    { pkt_InvLen2, "InvLen2",     0, 0, 0 },
    { pkt_InvMag, "InvMag",       0, 0, 0 },
    { pkt_InvProto, "InvPoto",    0, 0, 0 },
    { pkt_InvPos, "InvPos",       0, 0, 0 },
    { pkt_InvHgt, "InvHgt",       0, 0, 0 },
    { pkt_InvStg1, "InvCallsign", 0, 0, 0 },
    { pkt_InvStg2, "InvAircraft", 0, 0, 0 },
    { pkt_First, "FirstPos",      0, 0, 0 },
    { pkt_Pos, "Position",        0, 0, 0 },
    { pkt_Chat, "Chat",           0, 0, 0 },
    { pkt_Other, "Other",         0, 0, 0 },
    { pkt_Discards, "Discards",   0, 0, 0 }
};

PPKTSTR Get_Pkt_Str() { return &sPktStr[0]; }

//    if (seconds >= m_PlayerExpires) {
//        sprintf(tb,"TIME=%d", (int)seconds);
//        upd_by = tb;    // (char *)"TIME";
//    } else if (pp->dist_m > m_MinDistance_m) {
//        sprintf(tb,"DIST=%d/%d", (int)(pp->dist_m+0.5), (int)seconds);
//        upd_by = tb; // (char *)"DIST";
//    } else if (spdchg > m_MinSpdChange_kt) {
//        sprintf(tb,"SPDC=%d", spdchg);
//        upd_by = tb;    // (char *)"TIME";
//    } else if (hdgchg > m_MinHdgChange_deg) {
//        sprintf(tb,"HDGC=%d", hdgchg);
//        upd_by = tb;    // (char *)"TIME";
//    } else if (altchg > m_MinAltChange_ft) {
//        sprintf(tb,"ALTC=%d", altchg);
//        upd_by = tb;    // (char *)"TIME";
enum upd_Type {
    ut_New = 0,
    ut_Revived,
    ut_Time,
    ut_Dist,
    ut_Spd,
    ut_Hdg,
    ut_Alt,
    ut_Discard,
    ut_Bad,
    ut_Max
};

int ut_Stats[ut_Max];
typedef struct tagUPD_TYPES {
    upd_Type updt;
    const char *name;
}UPD_TYPES, *PUPD_TYPES;

static UPD_TYPES upd_Types[] = {
    { ut_New, "New" },
    { ut_Revived, "Revived" },
    { ut_Time, "Time" },
    { ut_Dist, "Dist" },
    { ut_Spd, "Speed" },
    { ut_Hdg, "Heading" },
    { ut_Alt, "Altitude" },
    { ut_Discard, "Discarded" },
    { ut_Bad, "Bad" },
    // LAST
    { ut_Max, 0 }
};

const char *get_upd_type_name(upd_Type ut)
{
    PUPD_TYPES put = &upd_Types[0];
    while (put->name) {
        if (put->updt == ut)
            return put->name;
        put++;
    }
    return "Unlisted";
}

void set_upd_type(upd_Type ut)
{
    if ((ut >= ut_New) && (ut < ut_Max))
        ut_Stats[ut]++;
}

void clear_upd_type_stats()
{
    int i;
    for (i = 0; i < ut_Max; i++) {
        ut_Stats[i] = 0;
    }
}

void show_upd_type_stats()
{
    int i, cnt;
    int total = 0;
    int tot2 = 0;
    double pct1 = 0;
    for (i = 0; i < ut_Max; i++) {
        total += ut_Stats[i];
    }
    for (i = 0; i < ut_Max; i++) {
        cnt = ut_Stats[i];
        const char *name = get_upd_type_name((upd_Type)i);
        double pct = get_pct_1( cnt, total );
        SPRTF("%6d of %6d: %10s %5.1f\n", cnt, total, name, pct);
        tot2 += cnt;
        pct1 += pct;
    }
    SPRTF("%6d of %6d: %10s %5.1f\n", tot2, total, "TOTALS", pct1);
}


/////////////////////////////////////////////////////////////////


/**
 * xdr decode 8, 16 and 32 Bit values
 */
template<typename TYPE>
TYPE XDR_decode ( xdr_data_t Val ) {
    union {
        xdr_data_t      encoded;
        TYPE            raw;
    } tmp;
    tmp.encoded = SWAP32(Val);
    return (tmp.raw);
}
/**
 * xdr decode 64 Bit values
 */
template<typename TYPE>
TYPE XDR_decode64 ( xdr_data2_t Val ) {
    union {
        xdr_data2_t     encoded;
        TYPE            raw;
    } tmp;
    tmp.encoded = SWAP64 (Val);
    return (tmp.raw);
}

enum { X, Y, Z };
enum { Lat, Lon, Alt };

static char _s_aircraft[MAX_MODEL_NAME_LEN];
static char _s_callsign[MAX_CALLSIGN_LEN];

//////////////////////////////////////////////////////////////////
enum Pilot_Type {
    pt_Unknown,
    pt_New,
    pt_Revived,
    pt_Pos,
    pt_Expired,
    pt_Stat
};

///////////////////////////////////////////////////////////////////////////////
// Pilot information kept in vector list
// =====================================
// NOTE: From 'simgear' onwards, ALL members MUST be value type
// That is NO classes, ie no reference types
// This is updated in the vector by a copy through a pointer - *pp2 = *pp;
// If kept as 'value' members, this is a blindingly FAST rep movsb esi edi;
// *** Please KEEP it that way! ***
typedef struct tagCF_Pilot {
    uint64_t flight_id; // unique flight ID = epoch*1000+index
    Pilot_Type      pt;
    bool            expired;
    time_t          curr_time, prev_time, first_time;    // rough seconds
    double          sim_time, prev_sim_time, first_sim_time; // sim time from packet
    char            callsign[MAX_CALLSIGN_LEN];
    char            aircraft[MAX_MODEL_NAME_LEN];
    double          px, py, pz;
    double          ppx, ppy, ppz;
    double          lat, lon;    // degrees
    double          alt;         // feet
    int             SenderAddress, SenderPort;
    int             packetCount, packetsDiscarded;
    double          heading, pitch, roll, speed;
    double          dist_m; // vector length since last - meters
    double          total_nm, cumm_nm;   // total distance since start
    time_t          exp_time;    // time expired - epoch secs
    time_t          last_seen;  // last packet seen - epoch secs
}CF_Pilot, *PCF_Pilot;

typedef vector<CF_Pilot> vCFP;

vCFP vPilots;


//////////////////////////////////////////////////////////////////
typedef struct tagMSGID2TYPE {
    int id;
    const char *name;
} MSGID2TYPE, *PMSGID2TYPE;

MSGID2TYPE sMsg2Type[] = {
    { CHAT_MSG_ID, "CHAT" },
    { RESET_DATA_ID, "RESET" },
    { POS_DATA_ID, "POS" },
    { 0, 0 }
};

char *msgid2stg( int id )
{
    PMSGID2TYPE pmid = &sMsg2Type[0];
    while (pmid->name) {
        if (pmid->id == id)
            return (char *)pmid->name;
        pmid++;
    }
    char *tb = GetNxtBuf();
    sprintf(tb,"Unlisted %d", id );
    return tb;
}

//////////////////////////////////////////////////////////////////////
// Rather rough service to remove leading PATH
// and remove trailing file extension
char *get_Model( char *pm )
{
    static char _s_buf[MAX_MODEL_NAME_LEN+4];
    int i, c, len;
    char *cp = _s_buf;
    char *model = pm;
    len = MAX_MODEL_NAME_LEN;
    for (i = 0; i < len; i++) {
        c = pm[i];
        if (c == '/')
            model = &pm[i+1];
        else if (c == 0)
            break;
    }
    strcpy(cp,model);
    len = (int)strlen(cp);
    model = 0;
    for (i = 0; i < len; i++) {
        c = cp[i];
        if (c == '.')
            model = &cp[i];
    }
    if (model)
        *model = 0;
    return cp;
}

//////////////////////////////////////////////////////////////////////
// Filter CALLSIGN to ONLY ALPHA-NUMERIC (English) characters
// 20130105 - allow lowercase, and '-' or '_'
#define ISUA(a)  ((a >= 'A') && (a <= 'Z'))
#define ISLA(a)  ((a >= 'a') && (a <= 'z'))
#define ISNUM(a) ((a >= '0') && (a <= '9'))
#define ISSPL(a) ((a == '-') || (a == '_'))
#define ISOK(a) (ISUA(a) || ISLA(a) || ISNUM(a) || ISSPL(a)) 

char *get_CallSign( char *pcs )
{
    static char _s_callsign[MAX_CALLSIGN_LEN+2];
    int i, c, off;
    char *cp = _s_callsign;
    off = 0;
    for (i = 0; i < MAX_CALLSIGN_LEN; i++) {
        c = pcs[i];
        if (!c) break; // end on a null
        if (ISOK(c)) { // is acceptable char
            cp[off++] = (char)c;
        }
    }
    cp[off] = 0; // ensure ZERO termination
    return cp;
}

////////////////////////////////////////////////////////
// uint64_t get_epoch_id()
//
// get UNIQUE ID for flight
// allows for up to 999 pilots joining in the same second
// ======================================================
static time_t ep_prev = 0;
static int eq_count = 0;
uint64_t get_epoch_id()
{
    time_t ep_curr = time(0);
    if (ep_curr == ep_prev)
        eq_count++;
    else {
        eq_count = 0;
        ep_prev = ep_curr;
    }
    uint64_t id = ep_curr * 1000;
    id += eq_count;
    return id;
}

///////////////////////////////////////////////////////////////////////////////////
#ifdef ADD_PROPERTIES
// ===================================================================
// properties
// ===================================================================
#define MAX_PACKET_SIZE 1200
#define MAX_TEXT_SIZE 128

struct IdPropertyList {
  unsigned id;
  const char* name;
  simgear::props::Type type;
};

// A static map of protocol property id values to property paths,
// This should be extendable dynamically for every specific aircraft ...
// For now only that static list
static const IdPropertyList sIdPropertyList[] = {
  {100, "surface-positions/left-aileron-pos-norm",  simgear::props::FLOAT},
  {101, "surface-positions/right-aileron-pos-norm", simgear::props::FLOAT},
  {102, "surface-positions/elevator-pos-norm",      simgear::props::FLOAT},
  {103, "surface-positions/rudder-pos-norm",        simgear::props::FLOAT},
  {104, "surface-positions/flap-pos-norm",          simgear::props::FLOAT},
  {105, "surface-positions/speedbrake-pos-norm",    simgear::props::FLOAT},
  {106, "gear/tailhook/position-norm",              simgear::props::FLOAT},
  {107, "gear/launchbar/position-norm",             simgear::props::FLOAT},
  {108, "gear/launchbar/state",                     simgear::props::STRING},
  {109, "gear/launchbar/holdback-position-norm",    simgear::props::FLOAT},
  {110, "canopy/position-norm",                     simgear::props::FLOAT},
  {111, "surface-positions/wing-pos-norm",          simgear::props::FLOAT},
  {112, "surface-positions/wing-fold-pos-norm",     simgear::props::FLOAT},

  {200, "gear/gear[0]/compression-norm",           simgear::props::FLOAT},
  {201, "gear/gear[0]/position-norm",              simgear::props::FLOAT},
  {210, "gear/gear[1]/compression-norm",           simgear::props::FLOAT},
  {211, "gear/gear[1]/position-norm",              simgear::props::FLOAT},
  {220, "gear/gear[2]/compression-norm",           simgear::props::FLOAT},
  {221, "gear/gear[2]/position-norm",              simgear::props::FLOAT},
  {230, "gear/gear[3]/compression-norm",           simgear::props::FLOAT},
  {231, "gear/gear[3]/position-norm",              simgear::props::FLOAT},
  {240, "gear/gear[4]/compression-norm",           simgear::props::FLOAT},
  {241, "gear/gear[4]/position-norm",              simgear::props::FLOAT},

  {300, "engines/engine[0]/n1",  simgear::props::FLOAT},
  {301, "engines/engine[0]/n2",  simgear::props::FLOAT},
  {302, "engines/engine[0]/rpm", simgear::props::FLOAT},
  {310, "engines/engine[1]/n1",  simgear::props::FLOAT},
  {311, "engines/engine[1]/n2",  simgear::props::FLOAT},
  {312, "engines/engine[1]/rpm", simgear::props::FLOAT},
  {320, "engines/engine[2]/n1",  simgear::props::FLOAT},
  {321, "engines/engine[2]/n2",  simgear::props::FLOAT},
  {322, "engines/engine[2]/rpm", simgear::props::FLOAT},
  {330, "engines/engine[3]/n1",  simgear::props::FLOAT},
  {331, "engines/engine[3]/n2",  simgear::props::FLOAT},
  {332, "engines/engine[3]/rpm", simgear::props::FLOAT},
  {340, "engines/engine[4]/n1",  simgear::props::FLOAT},
  {341, "engines/engine[4]/n2",  simgear::props::FLOAT},
  {342, "engines/engine[4]/rpm", simgear::props::FLOAT},
  {350, "engines/engine[5]/n1",  simgear::props::FLOAT},
  {351, "engines/engine[5]/n2",  simgear::props::FLOAT},
  {352, "engines/engine[5]/rpm", simgear::props::FLOAT},
  {360, "engines/engine[6]/n1",  simgear::props::FLOAT},
  {361, "engines/engine[6]/n2",  simgear::props::FLOAT},
  {362, "engines/engine[6]/rpm", simgear::props::FLOAT},
  {370, "engines/engine[7]/n1",  simgear::props::FLOAT},
  {371, "engines/engine[7]/n2",  simgear::props::FLOAT},
  {372, "engines/engine[7]/rpm", simgear::props::FLOAT},
  {380, "engines/engine[8]/n1",  simgear::props::FLOAT},
  {381, "engines/engine[8]/n2",  simgear::props::FLOAT},
  {382, "engines/engine[8]/rpm", simgear::props::FLOAT},
  {390, "engines/engine[9]/n1",  simgear::props::FLOAT},
  {391, "engines/engine[9]/n2",  simgear::props::FLOAT},
  {392, "engines/engine[9]/rpm", simgear::props::FLOAT},

  {800, "rotors/main/rpm", simgear::props::FLOAT},
  {801, "rotors/tail/rpm", simgear::props::FLOAT},
  {810, "rotors/main/blade[0]/position-deg",  simgear::props::FLOAT},
  {811, "rotors/main/blade[1]/position-deg",  simgear::props::FLOAT},
  {812, "rotors/main/blade[2]/position-deg",  simgear::props::FLOAT},
  {813, "rotors/main/blade[3]/position-deg",  simgear::props::FLOAT},
  {820, "rotors/main/blade[0]/flap-deg",  simgear::props::FLOAT},
  {821, "rotors/main/blade[1]/flap-deg",  simgear::props::FLOAT},
  {822, "rotors/main/blade[2]/flap-deg",  simgear::props::FLOAT},
  {823, "rotors/main/blade[3]/flap-deg",  simgear::props::FLOAT},
  {830, "rotors/tail/blade[0]/position-deg",  simgear::props::FLOAT},
  {831, "rotors/tail/blade[1]/position-deg",  simgear::props::FLOAT},

  {900, "sim/hitches/aerotow/tow/length",                       simgear::props::FLOAT},
  {901, "sim/hitches/aerotow/tow/elastic-constant",             simgear::props::FLOAT},
  {902, "sim/hitches/aerotow/tow/weight-per-m-kg-m",            simgear::props::FLOAT},
  {903, "sim/hitches/aerotow/tow/dist",                         simgear::props::FLOAT},
  {904, "sim/hitches/aerotow/tow/connected-to-property-node",   simgear::props::BOOL},
  {905, "sim/hitches/aerotow/tow/connected-to-ai-or-mp-callsign",   simgear::props::STRING},
  {906, "sim/hitches/aerotow/tow/brake-force",                  simgear::props::FLOAT},
  {907, "sim/hitches/aerotow/tow/end-force-x",                  simgear::props::FLOAT},
  {908, "sim/hitches/aerotow/tow/end-force-y",                  simgear::props::FLOAT},
  {909, "sim/hitches/aerotow/tow/end-force-z",                  simgear::props::FLOAT},
  {930, "sim/hitches/aerotow/is-slave",                         simgear::props::BOOL},
  {931, "sim/hitches/aerotow/speed-in-tow-direction",           simgear::props::FLOAT},
  {932, "sim/hitches/aerotow/open",                             simgear::props::BOOL},
  {933, "sim/hitches/aerotow/local-pos-x",                      simgear::props::FLOAT},
  {934, "sim/hitches/aerotow/local-pos-y",                      simgear::props::FLOAT},
  {935, "sim/hitches/aerotow/local-pos-z",                      simgear::props::FLOAT},

  {1001, "controls/flight/slats",  simgear::props::FLOAT},
  {1002, "controls/flight/speedbrake",  simgear::props::FLOAT},
  {1003, "controls/flight/spoilers",  simgear::props::FLOAT},
  {1004, "controls/gear/gear-down",  simgear::props::FLOAT},
  {1005, "controls/lighting/nav-lights",  simgear::props::FLOAT},
  {1006, "controls/armament/station[0]/jettison-all",  simgear::props::BOOL},

  {1100, "sim/model/variant", simgear::props::INT},
  {1101, "sim/model/livery/file", simgear::props::STRING},

  {1200, "environment/wildfire/data", simgear::props::STRING},
  {1201, "environment/contrail", simgear::props::INT},

  {1300, "tanker", simgear::props::INT},

  {1400, "scenery/events", simgear::props::STRING},

  {10001, "sim/multiplay/transmission-freq-hz",  simgear::props::STRING},
  {10002, "sim/multiplay/chat",  simgear::props::STRING},

  {10100, "sim/multiplay/generic/string[0]", simgear::props::STRING},
  {10101, "sim/multiplay/generic/string[1]", simgear::props::STRING},
  {10102, "sim/multiplay/generic/string[2]", simgear::props::STRING},
  {10103, "sim/multiplay/generic/string[3]", simgear::props::STRING},
  {10104, "sim/multiplay/generic/string[4]", simgear::props::STRING},
  {10105, "sim/multiplay/generic/string[5]", simgear::props::STRING},
  {10106, "sim/multiplay/generic/string[6]", simgear::props::STRING},
  {10107, "sim/multiplay/generic/string[7]", simgear::props::STRING},
  {10108, "sim/multiplay/generic/string[8]", simgear::props::STRING},
  {10109, "sim/multiplay/generic/string[9]", simgear::props::STRING},
  {10110, "sim/multiplay/generic/string[10]", simgear::props::STRING},
  {10111, "sim/multiplay/generic/string[11]", simgear::props::STRING},
  {10112, "sim/multiplay/generic/string[12]", simgear::props::STRING},
  {10113, "sim/multiplay/generic/string[13]", simgear::props::STRING},
  {10114, "sim/multiplay/generic/string[14]", simgear::props::STRING},
  {10115, "sim/multiplay/generic/string[15]", simgear::props::STRING},
  {10116, "sim/multiplay/generic/string[16]", simgear::props::STRING},
  {10117, "sim/multiplay/generic/string[17]", simgear::props::STRING},
  {10118, "sim/multiplay/generic/string[18]", simgear::props::STRING},
  {10119, "sim/multiplay/generic/string[19]", simgear::props::STRING},

  {10200, "sim/multiplay/generic/float[0]", simgear::props::FLOAT},
  {10201, "sim/multiplay/generic/float[1]", simgear::props::FLOAT},
  {10202, "sim/multiplay/generic/float[2]", simgear::props::FLOAT},
  {10203, "sim/multiplay/generic/float[3]", simgear::props::FLOAT},
  {10204, "sim/multiplay/generic/float[4]", simgear::props::FLOAT},
  {10205, "sim/multiplay/generic/float[5]", simgear::props::FLOAT},
  {10206, "sim/multiplay/generic/float[6]", simgear::props::FLOAT},
  {10207, "sim/multiplay/generic/float[7]", simgear::props::FLOAT},
  {10208, "sim/multiplay/generic/float[8]", simgear::props::FLOAT},
  {10209, "sim/multiplay/generic/float[9]", simgear::props::FLOAT},
  {10210, "sim/multiplay/generic/float[10]", simgear::props::FLOAT},
  {10211, "sim/multiplay/generic/float[11]", simgear::props::FLOAT},
  {10212, "sim/multiplay/generic/float[12]", simgear::props::FLOAT},
  {10213, "sim/multiplay/generic/float[13]", simgear::props::FLOAT},
  {10214, "sim/multiplay/generic/float[14]", simgear::props::FLOAT},
  {10215, "sim/multiplay/generic/float[15]", simgear::props::FLOAT},
  {10216, "sim/multiplay/generic/float[16]", simgear::props::FLOAT},
  {10217, "sim/multiplay/generic/float[17]", simgear::props::FLOAT},
  {10218, "sim/multiplay/generic/float[18]", simgear::props::FLOAT},
  {10219, "sim/multiplay/generic/float[19]", simgear::props::FLOAT},

  {10300, "sim/multiplay/generic/int[0]", simgear::props::INT},
  {10301, "sim/multiplay/generic/int[1]", simgear::props::INT},
  {10302, "sim/multiplay/generic/int[2]", simgear::props::INT},
  {10303, "sim/multiplay/generic/int[3]", simgear::props::INT},
  {10304, "sim/multiplay/generic/int[4]", simgear::props::INT},
  {10305, "sim/multiplay/generic/int[5]", simgear::props::INT},
  {10306, "sim/multiplay/generic/int[6]", simgear::props::INT},
  {10307, "sim/multiplay/generic/int[7]", simgear::props::INT},
  {10308, "sim/multiplay/generic/int[8]", simgear::props::INT},
  {10309, "sim/multiplay/generic/int[9]", simgear::props::INT},
  {10310, "sim/multiplay/generic/int[10]", simgear::props::INT},
  {10311, "sim/multiplay/generic/int[11]", simgear::props::INT},
  {10312, "sim/multiplay/generic/int[12]", simgear::props::INT},
  {10313, "sim/multiplay/generic/int[13]", simgear::props::INT},
  {10314, "sim/multiplay/generic/int[14]", simgear::props::INT},
  {10315, "sim/multiplay/generic/int[15]", simgear::props::INT},
  {10316, "sim/multiplay/generic/int[16]", simgear::props::INT},
  {10317, "sim/multiplay/generic/int[17]", simgear::props::INT},
  {10318, "sim/multiplay/generic/int[18]", simgear::props::INT},
  {10319, "sim/multiplay/generic/int[19]", simgear::props::INT}
};

const unsigned int numProperties = (sizeof(sIdPropertyList)
                                 / sizeof(sIdPropertyList[0]));


struct IdPropertyStats {
  unsigned int id;
  int count;
  simgear::props::Type type;
};

// A static map of protocol property id values to property paths,
// This should be extendable dynamically for every specific aircraft ...
// For now only that static list
static IdPropertyStats sIdPropertyStat[] = {
  {100, 0,  simgear::props::FLOAT},
  {101, 0, simgear::props::FLOAT},
  {102, 0,      simgear::props::FLOAT},
  {103, 0,        simgear::props::FLOAT},
  {104, 0,          simgear::props::FLOAT},
  {105, 0,    simgear::props::FLOAT},
  {106, 0,              simgear::props::FLOAT},
  {107, 0,             simgear::props::FLOAT},
  {108, 0,                     simgear::props::STRING},
  {109, 0,    simgear::props::FLOAT},
  {110, 0,                     simgear::props::FLOAT},
  {111, 0,          simgear::props::FLOAT},
  {112, 0,     simgear::props::FLOAT},

  {200, 0,           simgear::props::FLOAT},
  {201, 0,              simgear::props::FLOAT},
  {210, 0,           simgear::props::FLOAT},
  {211, 0,              simgear::props::FLOAT},
  {220, 0,           simgear::props::FLOAT},
  {221, 0,              simgear::props::FLOAT},
  {230, 0,           simgear::props::FLOAT},
  {231, 0,              simgear::props::FLOAT},
  {240, 0,           simgear::props::FLOAT},
  {241, 0,              simgear::props::FLOAT},

  {300, 0,  simgear::props::FLOAT},
  {301, 0,  simgear::props::FLOAT},
  {302, 0, simgear::props::FLOAT},
  {310, 0,  simgear::props::FLOAT},
  {311, 0,  simgear::props::FLOAT},
  {312, 0, simgear::props::FLOAT},
  {320, 0,  simgear::props::FLOAT},
  {321, 0,  simgear::props::FLOAT},
  {322, 0, simgear::props::FLOAT},
  {330, 0,  simgear::props::FLOAT},
  {331, 0,  simgear::props::FLOAT},
  {332, 0, simgear::props::FLOAT},
  {340, 0,  simgear::props::FLOAT},
  {341, 0,  simgear::props::FLOAT},
  {342, 0, simgear::props::FLOAT},
  {350, 0,  simgear::props::FLOAT},
  {351, 0,  simgear::props::FLOAT},
  {352, 0, simgear::props::FLOAT},
  {360, 0,  simgear::props::FLOAT},
  {361, 0,  simgear::props::FLOAT},
  {362, 0, simgear::props::FLOAT},
  {370, 0,  simgear::props::FLOAT},
  {371, 0,  simgear::props::FLOAT},
  {372, 0, simgear::props::FLOAT},
  {380, 0,  simgear::props::FLOAT},
  {381, 0,  simgear::props::FLOAT},
  {382, 0, simgear::props::FLOAT},
  {390, 0, simgear::props::FLOAT},
  {391, 0,  simgear::props::FLOAT},
  {392, 0, simgear::props::FLOAT},

  {800, 0, simgear::props::FLOAT},
  {801, 0, simgear::props::FLOAT},
  {810, 0,  simgear::props::FLOAT},
  {811, 0,  simgear::props::FLOAT},
  {812, 0,  simgear::props::FLOAT},
  {813, 0,  simgear::props::FLOAT},
  {820, 0,  simgear::props::FLOAT},
  {821, 0,  simgear::props::FLOAT},
  {822, 0,  simgear::props::FLOAT},
  {823, 0,  simgear::props::FLOAT},
  {830, 0,  simgear::props::FLOAT},
  {831, 0,  simgear::props::FLOAT},

  {900, 0,                       simgear::props::FLOAT},
  {901, 0,             simgear::props::FLOAT},
  {902, 0,            simgear::props::FLOAT},
  {903, 0,                         simgear::props::FLOAT},
  {904, 0,   simgear::props::BOOL},
  {905, 0,   simgear::props::STRING},
  {906, 0,                  simgear::props::FLOAT},
  {907, 0,                  simgear::props::FLOAT},
  {908, 0,                  simgear::props::FLOAT},
  {909, 0,                  simgear::props::FLOAT},
  {930, 0,                         simgear::props::BOOL},
  {931, 0,           simgear::props::FLOAT},
  {932, 0,                             simgear::props::BOOL},
  {933, 0,                      simgear::props::FLOAT},
  {934, 0,                      simgear::props::FLOAT},
  {935, 0,                      simgear::props::FLOAT},

  {1001, 0,  simgear::props::FLOAT},
  {1002, 0,  simgear::props::FLOAT},
  {1003, 0,  simgear::props::FLOAT},
  {1004, 0,  simgear::props::FLOAT},
  {1005, 0,  simgear::props::FLOAT},
  {1006, 0,  simgear::props::BOOL},

  {1100, 0, simgear::props::INT},
  {1101, 0, simgear::props::STRING},

  {1200, 0, simgear::props::STRING},
  {1201, 0, simgear::props::INT},

  {1300, 0, simgear::props::INT},

  {1400, 0, simgear::props::STRING},

  {10001, 0,  simgear::props::STRING},
  {10002, 0,  simgear::props::STRING},

  {10100, 0, simgear::props::STRING},
  {10101, 0, simgear::props::STRING},
  {10102, 0, simgear::props::STRING},
  {10103, 0, simgear::props::STRING},
  {10104, 0, simgear::props::STRING},
  {10105, 0, simgear::props::STRING},
  {10106, 0, simgear::props::STRING},
  {10107, 0, simgear::props::STRING},
  {10108, 0, simgear::props::STRING},
  {10109, 0, simgear::props::STRING},
  {10110, 0, simgear::props::STRING},
  {10111, 0, simgear::props::STRING},
  {10112, 0, simgear::props::STRING},
  {10113, 0, simgear::props::STRING},
  {10114, 0, simgear::props::STRING},
  {10115, 0, simgear::props::STRING},
  {10116, 0, simgear::props::STRING},
  {10117, 0, simgear::props::STRING},
  {10118, 0, simgear::props::STRING},
  {10119, 0, simgear::props::STRING},

  {10200, 0, simgear::props::FLOAT},
  {10201, 0, simgear::props::FLOAT},
  {10202, 0, simgear::props::FLOAT},
  {10203, 0, simgear::props::FLOAT},
  {10204, 0, simgear::props::FLOAT},
  {10205, 0, simgear::props::FLOAT},
  {10206, 0, simgear::props::FLOAT},
  {10207, 0, simgear::props::FLOAT},
  {10208, 0, simgear::props::FLOAT},
  {10209, 0, simgear::props::FLOAT},
  {10210, 0, simgear::props::FLOAT},
  {10211, 0, simgear::props::FLOAT},
  {10212, 0, simgear::props::FLOAT},
  {10213, 0, simgear::props::FLOAT},
  {10214, 0, simgear::props::FLOAT},
  {10215, 0, simgear::props::FLOAT},
  {10216, 0, simgear::props::FLOAT},
  {10217, 0, simgear::props::FLOAT},
  {10218, 0, simgear::props::FLOAT},
  {10219, 0, simgear::props::FLOAT},

  {10300, 0, simgear::props::INT},
  {10301, 0, simgear::props::INT},
  {10302, 0, simgear::props::INT},
  {10303, 0, simgear::props::INT},
  {10304, 0, simgear::props::INT},
  {10305, 0, simgear::props::INT},
  {10306, 0, simgear::props::INT},
  {10307, 0, simgear::props::INT},
  {10308, 0, simgear::props::INT},
  {10309, 0, simgear::props::INT},
  {10310, 0, simgear::props::INT},
  {10311, 0, simgear::props::INT},
  {10312, 0, simgear::props::INT},
  {10313, 0, simgear::props::INT},
  {10314, 0, simgear::props::INT},
  {10315, 0, simgear::props::INT},
  {10316, 0, simgear::props::INT},
  {10317, 0, simgear::props::INT},
  {10318, 0, simgear::props::INT},
  {10319, 0, simgear::props::INT}
};

const unsigned int numPropStats = (sizeof(sIdPropertyStat)
                                 / sizeof(sIdPropertyStat[0]));

void clear_prop_stats()
{
    unsigned int i;
    for (i = 0; i < numPropStats; i++) {
        sIdPropertyStat[i].count = 0;
    }
}

void bump_prop_stats(unsigned int id) {
    unsigned int i;
    for (i = 0; i < numPropStats; i++) {
        if (sIdPropertyStat[i].id == id) {
            sIdPropertyStat[i].count++;
            return;
        }
    }
}

const char *getPropertyName(unsigned int id)
{
    unsigned int i;
    for (i = 0; i < numProperties; i++) {
        if (sIdPropertyList[i].id == id)
            return sIdPropertyList[i].name;
    }
    return "Not FOUND!";
}


void show_prop_stats() {
    unsigned int i;
    for (i = 0; i < numPropStats; i++) {
        const char *name = getPropertyName( sIdPropertyStat[i].id );
        SPRTF( "%d %s %d\n", sIdPropertyStat[i].id, name, sIdPropertyStat[i].count );
    }
}

/**
 * The buffer that holds a multi-player message, suitably aligned.
 */
//union FGMultiplayMgr::MsgBuf
union MsgBuf
{
    MsgBuf()
    {
        memset(&Msg, 0, sizeof(Msg));
    }

    T_MsgHdr* msgHdr()
    {
        return &Header;
    }

    const T_MsgHdr* msgHdr() const
    {
        return reinterpret_cast<const T_MsgHdr*>(&Header);
    }

    T_PositionMsg* posMsg()
    {
        return reinterpret_cast<T_PositionMsg*>(Msg + sizeof(T_MsgHdr));
    }

    const T_PositionMsg* posMsg() const
    {
        return reinterpret_cast<const T_PositionMsg*>(Msg + sizeof(T_MsgHdr));
    }

    xdr_data_t* properties()
    {
        return reinterpret_cast<xdr_data_t*>(Msg + sizeof(T_MsgHdr)
                                             + sizeof(T_PositionMsg));
    }

    const xdr_data_t* properties() const
    {
        return reinterpret_cast<const xdr_data_t*>(Msg + sizeof(T_MsgHdr)
                                                   + sizeof(T_PositionMsg));
    }
    /**
     * The end of the properties buffer.
     */
    xdr_data_t* propsEnd()
    {
        return reinterpret_cast<xdr_data_t*>(Msg + MAX_PACKET_SIZE);
    };

    const xdr_data_t* propsEnd() const
    {
        return reinterpret_cast<const xdr_data_t*>(Msg + MAX_PACKET_SIZE);
    };
    /**
     * The end of properties actually in the buffer. This assumes that
     * the message header is valid.
     */
#ifdef _MSC_VER
    xdr_data_t* propsRecvdEnd()
    {
        return reinterpret_cast<xdr_data_t*>(Msg + XDR_decode<uint32_t>(Header.MsgLen));
    }

    const xdr_data_t* propsRecvdEnd() const
    {
        return reinterpret_cast<const xdr_data_t*>(Msg + XDR_decode<uint32_t>(Header.MsgLen));
    }
#else
    xdr_data_t* propsRecvdEnd()
    {
        return reinterpret_cast<xdr_data_t*>(Msg + Header.MsgLen);
    }

    const xdr_data_t* propsRecvdEnd() const
    {
        return reinterpret_cast<const xdr_data_t*>(Msg + Header.MsgLen);
    }
#endif

    xdr_data2_t double_val;
    char Msg[MAX_PACKET_SIZE];
    T_MsgHdr Header;
};

static MsgBuf *pmsgBuf = 0;
void init_cf_props();

char *set_MsgBuf( char *packet, int len )
{
    if (pmsgBuf == 0) {
        pmsgBuf = new MsgBuf;
        init_cf_props();
    }
    memset(&pmsgBuf->Msg,0,MAX_PACKET_SIZE);
    memcpy(&pmsgBuf->Msg[0],packet,len);
    return &pmsgBuf->Msg[len]; // end of data
}

int8_t
XDR_decode_int8 ( const xdr_data_t & n_Val )
{
    return (static_cast<int8_t> (SWAP32(n_Val)));
}

int32_t
XDR_decode_int32 ( const xdr_data_t & n_Val )
{
    return (static_cast<int32_t> (SWAP32(n_Val)));
}

uint32_t
XDR_decode_uint32 ( const xdr_data_t & n_Val )
{
    return (static_cast<uint32_t> (SWAP32(n_Val)));
}

float
XDR_decode_float ( const xdr_data_t & f_Val )
{
    union {
        xdr_data_t x;
        float f;
    } tmp;

    tmp.x = XDR_decode_int32 (f_Val);
    return tmp.f;
}

  struct ComparePropertyId
  {
    bool operator()(const IdPropertyList& lhs,
                    const IdPropertyList& rhs)
    {
      return lhs.id < rhs.id;
    }
    bool operator()(const IdPropertyList& lhs,
                    unsigned id)
    {
      return lhs.id < id;
    }
    bool operator()(unsigned id,
                    const IdPropertyList& rhs)
    {
      return id < rhs.id;
    }
  };    

const IdPropertyList* findProperty(unsigned id)
{
  std::pair<const IdPropertyList*, const IdPropertyList*> result
    = std::equal_range(sIdPropertyList, sIdPropertyList + numProperties, id,
                       ComparePropertyId());
  if (result.first == result.second) {
    return 0;
  } else {
    return result.first;
  }
}


bool verifyProperties(const xdr_data_t* data, const xdr_data_t* end)
{
    using namespace simgear;
    const xdr_data_t* xdr = data;
    while (xdr < end) {
        unsigned id = XDR_decode_uint32(*xdr);
        const IdPropertyList* plist = findProperty(id);
    
        if (plist) {
            xdr++;
            // How we decode the remainder of the property depends on the type
            switch (plist->type) {
            case props::INT:
            case props::BOOL:
            case props::LONG:
                xdr++;
                break;
            case props::FLOAT:
            case props::DOUBLE:
                {
                float val = XDR_decode_float(*xdr);
                if (SGMisc<float>::isNaN(val))
                    return false;
                xdr++;
                break;
                }
            case props::STRING:
            case props::UNSPECIFIED:
                {
                // String is complicated. It consists of
                // The length of the string
                // The string itself
                // Padding to the nearest 4-bytes.
                // XXX Yes, each byte is padded out to a word! Too late
                // to change...
                uint32_t length = XDR_decode_uint32(*xdr);
                xdr++;
                // Old versions truncated the string but left the length
                // unadjusted.
                if (length > MAX_TEXT_SIZE)
                    length = MAX_TEXT_SIZE;
                xdr += length;
                // Now handle the padding
                while ((length % 4) != 0)
                    {
                    xdr++;
                    length++;
                    //cout << "0";
                    }
                }
                break;
            default:
                // cerr << "Unknown Prop type " << id << " " << type << "\n";
                xdr++;
                break;
            }            
        } else {
            // give up; this is a malformed property list.
            return false;
        }
    }
    return true;
}

struct FGPropertyData {
  unsigned id;
  
  // While the type isn't transmitted, it is needed for the destructor
  simgear::props::Type type;
  union { 
    int int_value;
    float float_value;
    char* string_value;
  }; 
  
  ~FGPropertyData() {
    if ((type == simgear::props::STRING) || (type == simgear::props::UNSPECIFIED))
    {
      delete [] string_value;
    }
  }
};

typedef struct tagPT2STG {
    simgear::props::Type type;
    const char *name;
}PT2STG, *PPT2STG;

PT2STG sPT2Stg[] = {
    { simgear::props::NONE, "NONE" },
    { simgear::props::ALIAS, "ALIAS" }, /**< The node "points" to another node. */
    { simgear::props::BOOL, "BOOL" },
    { simgear::props::INT, "INT" },
    { simgear::props::LONG, "LONG" },
    { simgear::props::FLOAT, "FLOAT" },
    { simgear::props::DOUBLE, "DOUBLE" },
    { simgear::props::STRING, "STRING" },
    { simgear::props::UNSPECIFIED, "UNSPECIFIED" },
    { simgear::props::EXTENDED, "EXTENDED" }, /**< The node's value is not stored in the property;
                                                * the actual value and type is retrieved from an
                                                * SGRawValue node. This type is never returned by @see
                                                * SGPropertyNode::getType.
                                                */
    // Extended properties
    { simgear::props::VEC3D, "VEC3D" },
    { simgear::props::VEC4D, "VEC4D" },
    // ALWAYS THE LAST
    { (simgear::props::Type)0, 0 }
};

const char *get_PTStg( simgear::props::Type type )
{
    PPT2STG ppt = &sPT2Stg[0];
    while (ppt->name) {
        if (ppt->type == type)
            return ppt->name;
        ppt++;
    }
    return "Unknown";
}

//     if (strcmp(pData->string_value, "Hello")) {
//         if (strcmp(pData->string_value, "IFF: unknown")) {
//             if (strcmp(pData->string_value, "Comm Established")) {

static int msg_cnt, ce_cnt, ifun_cnt, he_cnt, bl_cnt;

void init_cf_props()
{
    msg_cnt = ce_cnt = ifun_cnt = he_cnt = bl_cnt = 0;
}


double get_pct_1( int cnt, int tot )
{
    double pct = (((double)cnt / (double)tot) * 100.0);
    int ipct = (int)( (pct + 0.05) * 10.0 );
    pct = (double)ipct / 10.0;
    return pct;
}

void show_chat_stats()
{
    int tot = msg_cnt;
    tot += ce_cnt;
    tot += ifun_cnt;
    tot += he_cnt;
    tot += bl_cnt;
    if (tot) {
        double mspct = get_pct_1( msg_cnt, tot );
        double hepct = get_pct_1( he_cnt, tot );
        double blpct = get_pct_1( bl_cnt, tot ); 
        double cepct = get_pct_1( ce_cnt, tot );
        double ifpct = get_pct_1( ifun_cnt, tot );
        SPRTF("Total chats %d: msg %d (%.1f%%), hello %d (%.1f%%), blank %d (%.1f%%), others %d (%.1f%%)\n",
            tot, 
            msg_cnt, mspct,
            he_cnt, hepct,
            bl_cnt, blpct,
            (ifun_cnt + ce_cnt), (ifpct + cepct)
            
            );

    }
}


//static FGPropertyData FGpd;
//static bool dninit = false;
void process_Props( char *end )
{
    int count = 0;
    std::vector<FGPropertyData*> properties;
    xdr_data_t *xend2 = (xdr_data_t *)end;
    xdr_data_t *xend = pmsgBuf->propsRecvdEnd();
    T_PositionMsg*  PosMsg = pmsgBuf->posMsg();

    const xdr_data_t* xdr = pmsgBuf->properties();
    if (PosMsg->pad != 0) {
        if (verifyProperties(&PosMsg->pad, pmsgBuf->propsRecvdEnd()))
            xdr = &PosMsg->pad;
        else if (!verifyProperties(xdr, pmsgBuf->propsRecvdEnd()))
            return; // goto noprops;
    }
    FGPropertyData* pData; // = &FGpd;
    xdr_data_t *prev_xdr = 0;
    xdr_data_t *curr_xdr = 0;
    int prev_len = 0;
    int last_len = 0;
    //while (xdr < pmsgBuf->propsRecvdEnd()) {
    while ((xdr+1) < pmsgBuf->propsRecvdEnd()) {
        if ((xdr+1) >= xend) {
            SPRTF("WARNING: packet buffer OVERRUN!\n");
            break;
        }

        // First element is always the ID
        prev_xdr = curr_xdr;
        curr_xdr = (xdr_data_t*) xdr;
        prev_len = last_len;
        last_len = 4;

        unsigned id = XDR_decode_uint32(*xdr);
        //cout << pData->id << " ";
        xdr++;
        // Check the ID actually exists and get the type
        const IdPropertyList* plist = findProperty(id);
    
        if (plist) {
            count++;
            bump_prop_stats(id);

              pData = new FGPropertyData;
              //FGPropertyData* pData = &FGpd;
              pData->id = id;
              pData->type = plist->type;
              // How we decode the remainder of the property depends on the type
              switch (pData->type) {
                case simgear::props::INT:
                case simgear::props::BOOL:
                case simgear::props::LONG:
                  pData->int_value = XDR_decode_uint32(*xdr);
                  xdr++;
                  last_len += 4;
                  //cout << pData->int_value << "\n";
                  if (VERB9) SPRTF("%d %s = %u\n", pData->id, plist->name, pData->int_value );
                  break;
                case simgear::props::FLOAT:
                case simgear::props::DOUBLE:
                  pData->float_value = XDR_decode_float(*xdr);
                  xdr++;
                  last_len += 4;
                  //cout << pData->float_value << "\n";
                  if (VERB9) SPRTF("%d %s = %f\n", pData->id, plist->name, pData->float_value );
                  break;
                case simgear::props::STRING:
                case simgear::props::UNSPECIFIED:
                  {
                    // String is complicated. It consists of
                    // The length of the string
                    // The string itself
                    // Padding to the nearest 4-bytes.    
                    uint32_t length = XDR_decode_uint32(*xdr);
                    xdr++;
                    //cout << length << " ";
                    // Old versions truncated the string but left the length unadjusted.
                    if (length > MAX_TEXT_SIZE)
                      length = MAX_TEXT_SIZE;
                    pData->string_value = new char[length + 1];
                    //cout << " String: ";
                    for (unsigned i = 0; i < length; i++)
                      {
                        pData->string_value[i] = (char) XDR_decode_int8(*xdr);
                        xdr++;
                        last_len += 4;
                        if ((xdr+1) >= xend) break;
                        //cout << pData->string_value[i];
                      }

                    pData->string_value[length] = '\0';
                    if (VERB9) {
                        SPRTF("%d %s = %s\n", pData->id, plist->name, pData->string_value );
                    } else if (pData->id == 10002) {
                        if (pData->string_value[0]) {
                            if (strcmp(pData->string_value, "Hello")) {
                                if (strcmp(pData->string_value, "IFF: unknown")) {
                                    if (strcmp(pData->string_value, "Comm Established")) {
                                        SPRTF("%d %s = %s\n", pData->id, plist->name, pData->string_value );
                                        msg_cnt++;
                                    } else {
                                        ce_cnt++;
                                    }
                                } else {
                                    ifun_cnt++;
                                }
                            } else {
                                he_cnt++;
                            }
                        } else {
                            bl_cnt++;
                        }
                    }

                    // Now handle the padding
                    while ((length % 4) != 0)
                      {
                        xdr++;
                        length++;
                        last_len += 4;
                        //cout << "0";
                      }
                    //cout << "\n";
                  }
                  break;

                default:
                  pData->float_value = XDR_decode_float(*xdr);
                  //SG_LOG(SG_NETWORK, SG_DEBUG, "Unknown Prop type " << pData->id << " " << pData->type);
                  SPRTF("WARNING: Unknown property id! type %d (%s)\n", id, pData->type,
                      get_PTStg( pData->type) );
                  xdr++;
                  break;
              }
              // motionInfo.properties.push_back(pData);
              properties.push_back(pData);

        } else {
            if (VERB9) SPRTF("WARNING: Unknown property id %d!\n", id);
#ifdef ADD_HEX_DEBUG
            if (VERB9)
            {
                char *from = (char *) (prev_xdr ? prev_xdr : curr_xdr);
                int hlen = end - from;
                if (hlen > 32)
                    hlen = 32;
                if (prev_xdr && (hlen < (prev_len+last_len)))
                    hlen = prev_len+last_len;
                if (hlen > (end - from))
                    hlen = end - from;
                out_hex(from,hlen,1);
            }
#endif
            break;
        }
        //if ((xdr+1) >= xend) {
        //    SPRTF("WARNING: packet buffer OVERRUN!\n");
        //    break;
        //}
    }
    size_t max = properties.size();
    size_t ii;
    for (ii = 0; ii < max; ii++) {
        pData = properties[ii];
        delete pData;
    }
    if (VERB9) SPRTF("Shown %d properties in packet...(%d)\n", count, (int)max );
// noprops:
}

// ===================================================================
#endif // #ifdef ADD_PROPERTIES
//////////////////////////////////////////////////////////////////////////////////
//// TRACKER
typedef struct tagPosn {
    double      lat, lon;
    int         alt_ft, spd_kts, dist_nm, hdg;
    time_t      last;
} Posn, *PPosn;

typedef vector<Posn> vPSN;

typedef struct tagCF_Pilot2 {
    uint64_t    fid;
    Posn        p;
    int         dist_nm;
    char        callsign[MAX_CALLSIGN_LEN];
    char        aircraft[MAX_MODEL_NAME_LEN];
    time_t      began, updated;
    bool        valid;
    vPSN       *vpPsn;
}CF_Pilot2, * PCF_Pilot2;

typedef vector<CF_Pilot2> vCFP2;

vCFP2 vFlights;   // flights list from crossfeed data

void out_flight_json()
{
    ostringstream msg;
    //msg << "Added Bucket " << loaded_bucket_list.size() << " " << b << " " << b.gen_index_str() << endl;
    size_t max = vFlights.size();
    size_t ii;
    PCF_Pilot2 pp;
    int i2;
    size_t m2;
    for (ii = 0; ii < max; ii++)
    {
        pp = &vFlights[ii];
        msg << "{\"fid\":" << pp->fid;
        msg << ",\"cs\":\"" << pp->callsign << "\"";
        msg << ",\"model\":\"" << pp->aircraft << "\"";
        msg << ",\"lat\":" << pp->p.lat;
        msg << ",\"lon\":" << pp->p.lon;
        msg << ",\"alt_f\":" << pp->p.alt_ft;
        msg << ",\"hdg\":" << pp->p.hdg;
        msg << ",\"spd\":" << pp->p.spd_kts;
        if (pp->vpPsn) {
            i2 = 0;
            m2 = pp->vpPsn->size();
            msg << ",\"wpts\":" << m2;
            msg << ",\"posns\":[\n";
            if (m2 > 0)
                i2 = (int)(m2 - 1);
            else
                i2 = -1;
            for ( ; i2 >= 0; i2--) {
                PPosn ppsn = &pp->vpPsn->at(i2);
                msg << "{\"lat\":" << ppsn->lat;
                msg << ",\"lon\":" << ppsn->lon;
                msg << ",\"alt_f\":" << ppsn->alt_ft;
                msg << ",\"hdg\":" << ppsn->hdg;
                msg << ",\"spd_kt\":" << ppsn->spd_kts;
                msg << "}";
                if (i2 > 0)
                    msg << ",";
                msg << "\n";
                if (msg.str().size() > 1000) {
                    SPRTF((char *)msg.str().c_str());
                    msg.str(""); // clear the stream

                }
            }
            msg << "]\n";
        }
        msg << "}";
        if ((ii + 1) < max)
            msg << ",";
        msg << "\n";
        SPRTF((char *)msg.str().c_str());
        msg.str(""); // clear the stream
    }

    msg.str(""); // clear the stream
    i2 = 0;
    for (ii = 0; ii < max; ii++)
    {
        pp = &vFlights[ii];
        m2 = 0;
        if (pp->vpPsn) {
            m2 = pp->vpPsn->size();
        }
        i2 += (int)m2;
        msg << "Callsign: " << pp->callsign;
        msg << ", positions " << m2;
        msg << "\n";
        SPRTF((char *)msg.str().c_str());
        msg.str(""); // clear the stream
    }

    msg << "Accumuated " << max << " flights, with " << i2 << " positions.\n";
    SPRTF((char *)msg.str().c_str());
}



int add_2_flights( CF_Pilot2 &p2 )
{
    size_t max = vFlights.size();
    size_t ii;
    for (ii = 0; ii < max; ii++) {
        PCF_Pilot2 pp2 = &vFlights[ii];
        if (pp2->fid == p2.fid) {
            if (pp2->vpPsn == 0) {
                pp2->vpPsn = new vPSN;
                if (pp2->vpPsn == 0) {
                    SPRTF("ERROR: Memory allocation FAILURE!\n");
                    exit(1);
                }
            }
            pp2->vpPsn->push_back(pp2->p); // store previous in vector
            pp2->p.lat = p2.p.lat;
            pp2->p.lon = p2.p.lon;
            pp2->p.alt_ft = p2.p.alt_ft;
            pp2->p.hdg = p2.p.hdg;
            pp2->dist_nm = p2.dist_nm;
            pp2->valid = p2.valid;
            pp2->updated = p2.updated;  // last json update time
            return 1;
        }
    }
    vFlights.push_back(p2);
    return 0;
}

void update_flights ( PCF_Pilot pp )
{
    CF_Pilot2 p2;
    p2.fid = pp->flight_id;
    p2.p.alt_ft = pp->alt;
    p2.p.dist_nm = pp->dist_m * SG_METER_TO_NM;
    p2.p.hdg = pp->heading;
    p2.p.last = pp->last_seen; //time(0);
    p2.p.lat = pp->lat;
    p2.p.lon = pp->lon;
    p2.p.spd_kts = pp->speed;
    p2.dist_nm = pp->dist_m * SG_METER_TO_NM;
    strncpy(p2.callsign, pp->callsign, MAX_CALLSIGN_LEN);
    strncpy(p2.aircraft, pp->aircraft, MAX_MODEL_NAME_LEN);
    p2.began = pp->first_time;
    p2.updated = pp->curr_time;
    p2.valid = pp->expired ? false : true;
    p2.vpPsn = 0;
    add_2_flights( p2 );
}


/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
#define SAME_FLIGHT(pp1,pp2)  \
    ((strcmp(pp2->callsign, pp1->callsign) == 0) && \
     (strcmp(pp2->aircraft, pp1->aircraft) == 0))
// Hmmm, in a testap it appears abs() can take 0.1 to 30% longer than test and subtract in _MSC_VER, Sooooooo
#ifdef _MSC_VER
#define SPD_CHANGE(pp1,pp2) (int)(((pp1->speed > pp2->speed) ? pp1->speed - pp2->speed : pp2->speed - pp1->speed ) + 0.5)
#define HDG_CHANGE(pp1,pp2) (int)(((pp1->heading > pp2->heading) ? pp1->heading - pp2->heading : pp2->heading - pp1->heading) + 0.5)
#define ALT_CHANGE(pp1,pp2) (int)(((pp1->alt > pp2->alt) ? pp1->alt - pp2->alt : pp2->alt - pp1->alt) + 0.5)
#else // seem in unix abs() is more than 50% FASTER
#define SPD_CHANGE(pp1,pp2) (int)(abs(pp1->speed - pp2->speed) + 0.5)
#define HDG_CHANGE(pp1,pp2) (int)(abs(pp1->heading - pp2->heading) + 0.5)
#define ALT_CHANGE(pp1,pp2) (int)(abs(pp1->alt - pp2->alt) + 0.5)
#endif // _MSC_VER
#define SETPREVPOS(p1,p2) { p1->ppx = p2->px; p1->ppy = p2->py; p1->ppz = p2->pz; }

static CF_Pilot pi;
static char _s_tmp_buf[128];

Packet_Type Show_Pos_Packet( char *packet, int len )
{
    Packet_Type pt = pkt_Invalid;
    uint32_t        MsgId;
    uint32_t        MsgMagic;
    uint32_t        MsgLen;
    uint32_t        MsgProto;
    T_PositionMsg*  PosMsg;
    PT_MsgHdr       MsgHdr;
    double          lat,lon,alt;
    double          px, py, pz;
    float           ox, oy, oz;
    char           *tb = _s_tmp_buf;
    time_t          curr_time = time(0);

    //pp = &new_pilot;
    MsgHdr    = (PT_MsgHdr)packet;
    MsgMagic  = XDR_decode<uint32_t> (MsgHdr->Magic);
    MsgId     = XDR_decode<uint32_t> (MsgHdr->MsgId);
    MsgLen    = XDR_decode<uint32_t> (MsgHdr->MsgLen);
    MsgProto  = XDR_decode<uint32_t> (MsgHdr->Version);
    if ( !((MsgMagic == RELAY_MAGIC)||(MsgMagic == MSG_MAGIC)) ) {
        return pkt_InvMag;
    } else if ( !(MsgProto == PROTO_VER) ) {
        return pkt_InvProto;
    }
    if (MsgId == POS_DATA_ID)
    {
        if (MsgLen < sizeof(T_MsgHdr) + sizeof(T_PositionMsg)) {
            return pkt_InvLen2;
        }
        PosMsg = (T_PositionMsg *) (packet + sizeof(T_MsgHdr));
        px = XDR_decode64<double> (PosMsg->position[X]);
        py = XDR_decode64<double> (PosMsg->position[Y]);
        pz = XDR_decode64<double> (PosMsg->position[Z]);
        if ( (px == 0.0) || (py == 0.0) || (pz == 0.0)) {   
            return pkt_InvPos;
        }
        ox = XDR_decode<float> (PosMsg->orientation[X]);
        oy = XDR_decode<float> (PosMsg->orientation[Y]);
        oz = XDR_decode<float> (PosMsg->orientation[Z]);

        SGVec3d position(px,py,pz);
        SGGeod GeodPoint;
        SGGeodesy::SGCartToGeod ( position, GeodPoint );
        alt = GeodPoint.getElevationFt();
        if (alt <= -9990.0) {
            return pkt_InvHgt;
        }
        lat = GeodPoint.getLatitudeDeg();
        lon = GeodPoint.getLongitudeDeg();

        SGVec3f angleAxis(ox,oy,oz);
        SGQuatf ecOrient = SGQuatf::fromAngleAxis(angleAxis);
        SGQuatf qEc2Hl = SGQuatf::fromLonLatRad((float)GeodPoint.getLongitudeRad(),
                                          (float)GeodPoint.getLatitudeRad());
        // The orientation wrt the horizontal local frame
        SGQuatf hlOr = conj(qEc2Hl)*ecOrient;
        float hDeg, pDeg, rDeg;
        hlOr.getEulerDeg(hDeg, pDeg, rDeg);

        SGVec3f linearVel;
        for (unsigned i = 0; i < 3; ++i)
            linearVel(i) = XDR_decode<float> (PosMsg->linearVel[i]);
        float spd = norm(linearVel) * SG_METER_TO_NM * 3600.0;

        strcpy(_s_aircraft,get_Model(PosMsg->Model));
        strcpy(_s_callsign,get_CallSign(MsgHdr->Callsign));
        if (_s_callsign[0] == 0) {
            return pkt_InvStg1;
        } else if (_s_aircraft[0] == 0) {
            return pkt_InvStg2;
        }

        pi.flight_id = 0;
        pi.pt = pt_New;
        pi.expired = false;
        pi.curr_time = pi.prev_time = pi.first_time = time(0);    // rough seconds
        pi.sim_time = pi.prev_sim_time = pi.first_sim_time = XDR_decode64<double> (PosMsg->time); // get SIM time
        strncpy(pi.callsign, _s_callsign, MAX_CALLSIGN_LEN);
        strncpy(pi.aircraft, _s_aircraft, MAX_MODEL_NAME_LEN);
        pi.px = pi.ppx = px;
        pi.py = pi.ppy = py;
        pi.pz = pi.ppz = pz;
        pi.lat = lat;
        pi.lon = lon;    // degrees
        pi.alt = alt;        // feet
        pi.SenderAddress = XDR_decode<uint32_t> (MsgHdr->ReplyAddress);
        pi.SenderPort    = XDR_decode<uint32_t> (MsgHdr->ReplyPort);
        pi.packetCount = 1;
        pi.packetsDiscarded  = 0;
        pi.heading = hDeg;
        pi.pitch   = pDeg;
        pi.roll    = rDeg;
        pi.speed   = spd;
        pi.dist_m  = 0; // vector length since last - meters
        pi.total_nm = 0;
        pi.cumm_nm  = 0;   // total distance since start
        pi.exp_time = pi.curr_time;    // time expired - epoch secs
        pi.last_seen = pi.curr_time;  // last packet seen - epoch secs

        PCF_Pilot pp = &pi;
        PCF_Pilot pp2;
        size_t max = vPilots.size();
        char *upd_by = 0;
        size_t ii;
        time_t seconds;
        bool revived;
        bool found = false;
        for (ii = 0; ii < max; ii++) {
            pp2 = &vPilots[ii]; // search list for this pilots
            if (SAME_FLIGHT(pp,pp2)) {
                found   = true;
                seconds = pp->sim_time - pp2->sim_time; // curr packet sim time minus last packet sim time
                revived = false;
                if (pp2->expired) {
                    pp->pt = pt_Revived; // revival of previous flight record
                    revived = true;
                    pp->dist_m = 0.0;
                    sprintf(tb,"REVIVED=%d", (int)seconds);
                    upd_by = tb;    // (char *)"TIME";
                } else {
                    pp->pt = pt_Pos;
                    SGVec3d p1(pp->px,pp->py,pp->pz);       // current position
                    SGVec3d p2(pp2->px,pp2->py,pp2->pz);    // previous position
                    pp->dist_m = length(p2 - p1); // * SG_METER_TO_NM;
                    int spdchg = SPD_CHANGE(pp,pp2); // change_in_speed( pp, pp2 );
                    int hdgchg = HDG_CHANGE(pp,pp2); // change_in_heading( pp, pp2 );
                    int altchg = ALT_CHANGE(pp,pp2); // change_in_altitude( pp, pp2 );
                    if (seconds >= m_PlayerExpires) {
                        sprintf(tb,"TIME=%d", (int)seconds);
                        upd_by = tb;    // (char *)"TIME";
                    } else if (pp->dist_m > m_MinDistance_m) {
                        sprintf(tb,"DIST=%d/%d", (int)(pp->dist_m+0.5), (int)seconds);
                        upd_by = tb; // (char *)"DIST";
                    } else if (spdchg > m_MinSpdChange_kt) {
                        sprintf(tb,"SPDC=%d", spdchg);
                        upd_by = tb;    // (char *)"TIME";
                    } else if (hdgchg > m_MinHdgChange_deg) {
                        sprintf(tb,"HDGC=%d", hdgchg);
                        upd_by = tb;    // (char *)"TIME";
                    } else if (altchg > m_MinAltChange_ft) {
                        sprintf(tb,"ALTC=%d", altchg);
                        upd_by = tb;    // (char *)"TIME";
                    }
                }
                if (upd_by) {
                    if (revived) {
                        pp->flight_id      = get_epoch_id(); // establish NEW UNIQUE ID for flight
                        pp->first_sim_time = pp->sim_time;   // restart first sim time
                        pp->cumm_nm       += pp2->total_nm;  // get cummulative nm
                        pp2->total_nm      = 0.0;            // restart nm
                    } else {
                        pp->flight_id      = pp2->flight_id; // use existing FID
                        pp->first_sim_time = pp2->first_sim_time; // keep first sim time
                        pp->first_time     = pp2->first_time; // keep first epoch time
                    }
                    pp->expired          = false;
                    pp->packetCount      = pp2->packetCount + 1;
                    pp->packetsDiscarded = pp2->packetsDiscarded;
                    pp->prev_sim_time    = pp2->sim_time;
                    pp->prev_time        = pp2->curr_time;
                    // accumulate total distance travelled (in nm)
                    pp->total_nm         = pp2->total_nm + (pp->dist_m * SG_METER_TO_NM);
                    SETPREVPOS(pp,pp2);  // copy POS to PrevPos to get distance travelled
                    pp->curr_time        = curr_time; // set CURRENT packet time
                    /// *pp2 = *pp;     // UPDATE the RECORD with latest info
                    //print_pilot(pp2,upd_by,pt_Pos);
                    //if (revived)
                    //    Pilot_Tracker_Connect(pp2);
                    //else
                    //    Pilot_Tracker_Position(pp2);
                    pt = pkt_Pos;
                } else {
                    pp2->packetsDiscarded++;
                    return pkt_Discards;
                }
                break;
            }   // if (SAME_FLIGHT(pp,pp2)) {
        }   // for (ii = 0; ii < max; ii++)
        if (!found) {
            pp->flight_id        = get_epoch_id(); // establish NEW UNIQUE ID for flight
            pp->packetCount      = 1;
            pp->packetsDiscarded = 0;
            pp->first_sim_time   = pp->prev_sim_time = pp->sim_time;
            SETPREVPOS(pp,pp); // set as SAME as current
            pp->curr_time        = curr_time; // set CURRENT packet time
            pp->pt               = pt_New;
            pp->dist_m           = 0.0;
            pp->total_nm         = 0.0;
            // ================================================
            ///// vPilots.push_back(*pp); // *** STORE NEW RECORD ***
            // ================================================
            //print_pilot(pp,(char *)"NEW",pt_New);
            //Pilot_Tracker_Connect(pp);
            pt = pkt_First;
            strcpy(tb,"NEW");
            upd_by = tb;
        }
        SPRTF("%8s pos: %15.8f,%15.8f alt: %6d hdg: %3d spd: %4d %20s %s\n",
            _s_callsign, lat, lon, (int)alt, (int)hDeg, (int)spd, _s_aircraft, tb );
    } else if (MsgId == CHAT_MSG_ID) {
        pt = pkt_Chat;
    } else {
        pt = pkt_Other;
    }
    return pt;


}

Packet_Type Deal_With_Packet( char *packet, int len )
{
    Packet_Type pt = pkt_Invalid;
    uint32_t        MsgId;
    uint32_t        MsgMagic;
    uint32_t        MsgLen;
    uint32_t        MsgProto;
    T_PositionMsg*  PosMsg;
    PT_MsgHdr       MsgHdr;
    double          lat,lon,alt;
    double          px, py, pz;
    float           ox, oy, oz;
    char           *tb = _s_tmp_buf;
    time_t          curr_time = time(0);
    upd_Type        updt;

    //pp = &new_pilot;
    MsgHdr    = (PT_MsgHdr)packet;
    MsgMagic  = XDR_decode<uint32_t> (MsgHdr->Magic);
    MsgId     = XDR_decode<uint32_t> (MsgHdr->MsgId);
    MsgLen    = XDR_decode<uint32_t> (MsgHdr->MsgLen);
    MsgProto  = XDR_decode<uint32_t> (MsgHdr->Version);
    updt = ut_Bad;
    if (len < (int)MsgLen) {
        SPRTF("Packet length %d, expected %d (but min.Pos is %d)! MsgId = %d, %s\n",
            len, (int)MsgLen,
            sizeof(T_MsgHdr) + sizeof(T_PositionMsg),
            (int)MsgId, msgid2stg(MsgId));

        if (MsgId == CHAT_MSG_ID) {
            set_upd_type(updt);
            return pkt_Chat;
        } else if (MsgId == POS_DATA_ID) {
            if (MsgLen < sizeof(T_MsgHdr) + sizeof(T_PositionMsg)) {
                set_upd_type(updt);
                return pkt_InvLen2;
            }
            Show_Pos_Packet( packet, len );
        }
        set_upd_type(updt);
        return pkt_InvLen1;
    } else if ( !((MsgMagic == RELAY_MAGIC)||(MsgMagic == MSG_MAGIC)) ) {
        set_upd_type(updt);
        return pkt_InvMag;
    } else if ( !(MsgProto == PROTO_VER) ) {
        set_upd_type(updt);
        return pkt_InvProto;
    }
    if (MsgId == POS_DATA_ID)
    {
        if (MsgLen < sizeof(T_MsgHdr) + sizeof(T_PositionMsg)) {
            set_upd_type(updt);
            return pkt_InvLen2;
        }
        PosMsg = (T_PositionMsg *) (packet + sizeof(T_MsgHdr));
        px = XDR_decode64<double> (PosMsg->position[X]);
        py = XDR_decode64<double> (PosMsg->position[Y]);
        pz = XDR_decode64<double> (PosMsg->position[Z]);
        if ( (px == 0.0) || (py == 0.0) || (pz == 0.0)) {   
            set_upd_type(updt);
            return pkt_InvPos;
        }
        ox = XDR_decode<float> (PosMsg->orientation[X]);
        oy = XDR_decode<float> (PosMsg->orientation[Y]);
        oz = XDR_decode<float> (PosMsg->orientation[Z]);

        SGVec3d position(px,py,pz);
        SGGeod GeodPoint;
        SGGeodesy::SGCartToGeod ( position, GeodPoint );
        alt = GeodPoint.getElevationFt();
        if (alt <= -9990.0) {
            set_upd_type(updt);
            return pkt_InvHgt;
        }
        lat = GeodPoint.getLatitudeDeg();
        lon = GeodPoint.getLongitudeDeg();

        SGVec3f angleAxis(ox,oy,oz);
        SGQuatf ecOrient = SGQuatf::fromAngleAxis(angleAxis);
        SGQuatf qEc2Hl = SGQuatf::fromLonLatRad((float)GeodPoint.getLongitudeRad(),
                                          (float)GeodPoint.getLatitudeRad());
        // The orientation wrt the horizontal local frame
        SGQuatf hlOr = conj(qEc2Hl)*ecOrient;
        float hDeg, pDeg, rDeg;
        hlOr.getEulerDeg(hDeg, pDeg, rDeg);

        SGVec3f linearVel;
        for (unsigned i = 0; i < 3; ++i)
            linearVel(i) = XDR_decode<float> (PosMsg->linearVel[i]);
        float spd = norm(linearVel) * SG_METER_TO_NM * 3600.0;

        strcpy(_s_aircraft,get_Model(PosMsg->Model));
        strcpy(_s_callsign,get_CallSign(MsgHdr->Callsign));
        if (_s_callsign[0] == 0) {
            set_upd_type(updt);
            return pkt_InvStg1;
        } else if (_s_aircraft[0] == 0) {
            set_upd_type(updt);
            return pkt_InvStg2;
        }

        updt = ut_New;
        pi.flight_id = 0;
        pi.pt = pt_New;
        pi.expired = false;
        pi.curr_time = pi.prev_time = pi.first_time = time(0);    // rough seconds
        pi.sim_time = pi.prev_sim_time = pi.first_sim_time = XDR_decode64<double> (PosMsg->time); // get SIM time
        strncpy(pi.callsign, _s_callsign, MAX_CALLSIGN_LEN);
        strncpy(pi.aircraft, _s_aircraft, MAX_MODEL_NAME_LEN);
        pi.px = pi.ppx = px;
        pi.py = pi.ppy = py;
        pi.pz = pi.ppz = pz;
        pi.lat = lat;
        pi.lon = lon;    // degrees
        pi.alt = alt;        // feet
        pi.SenderAddress = XDR_decode<uint32_t> (MsgHdr->ReplyAddress);
        pi.SenderPort    = XDR_decode<uint32_t> (MsgHdr->ReplyPort);
        pi.packetCount = 1;
        pi.packetsDiscarded  = 0;
        pi.heading = hDeg;
        pi.pitch   = pDeg;
        pi.roll    = rDeg;
        pi.speed   = spd;
        pi.dist_m  = 0; // vector length since last - meters
        pi.total_nm = 0;
        pi.cumm_nm  = 0;   // total distance since start
        pi.exp_time = pi.curr_time;    // time expired - epoch secs
        pi.last_seen = pi.curr_time;  // last packet seen - epoch secs

        PCF_Pilot pp = &pi;
        PCF_Pilot pp2;
        size_t max = vPilots.size();
        char *upd_by = 0;
        size_t ii;
        time_t seconds;
        bool revived;
        bool found = false;
        for (ii = 0; ii < max; ii++) {
            pp2 = &vPilots[ii]; // search list for this pilots
            if (SAME_FLIGHT(pp,pp2)) {
                found   = true;
                seconds = pp->sim_time - pp2->sim_time; // curr packet sim time minus last packet sim time
                revived = false;
                if (pp2->expired) {
                    pp->pt = pt_Revived; // revival of previous flight record
                    revived = true;
                    pp->dist_m = 0.0;
                    sprintf(tb,"REVIVED=%d", (int)seconds);
                    upd_by = tb;    // (char *)"TIME";
                    updt = ut_Revived;
                } else {
                    pp->pt = pt_Pos;
                    SGVec3d p1(pp->px,pp->py,pp->pz);       // current position
                    SGVec3d p2(pp2->px,pp2->py,pp2->pz);    // previous position
                    pp->dist_m = length(p2 - p1); // * SG_METER_TO_NM;
                    int spdchg = SPD_CHANGE(pp,pp2); // change_in_speed( pp, pp2 );
                    int hdgchg = HDG_CHANGE(pp,pp2); // change_in_heading( pp, pp2 );
                    int altchg = ALT_CHANGE(pp,pp2); // change_in_altitude( pp, pp2 );
                    updt = ut_Discard;
                    if (seconds >= m_PlayerExpires) {
                        sprintf(tb,"TIME=%d", (int)seconds);
                        upd_by = tb;    // (char *)"TIME";
                        updt = ut_Time;
                    } else if (pp->dist_m > m_MinDistance_m) {
                        sprintf(tb,"DIST=%d/%d", (int)(pp->dist_m+0.5), (int)seconds);
                        upd_by = tb; // (char *)"DIST";
                        updt = ut_Dist;
                    } else if (spdchg > m_MinSpdChange_kt) {
                        sprintf(tb,"SPDC=%d", spdchg);
                        upd_by = tb;    // (char *)"TIME";
                        updt = ut_Spd;
                    } else if (hdgchg > m_MinHdgChange_deg) {
                        sprintf(tb,"HDGC=%d", hdgchg);
                        upd_by = tb;    // (char *)"TIME";
                        updt = ut_Hdg;
                    } else if (altchg > m_MinAltChange_ft) {
                        sprintf(tb,"ALTC=%d", altchg);
                        upd_by = tb;    // (char *)"TIME";
                        updt = ut_Alt;
                    }
                }
                if (upd_by) {
                    if (revived) {
                        pp->flight_id      = get_epoch_id(); // establish NEW UNIQUE ID for flight
                        pp->first_sim_time = pp->sim_time;   // restart first sim time
                        pp->cumm_nm       += pp2->total_nm;  // get cummulative nm
                        pp2->total_nm      = 0.0;            // restart nm
                    } else {
                        pp->flight_id      = pp2->flight_id; // use existing FID
                        pp->first_sim_time = pp2->first_sim_time; // keep first sim time
                        pp->first_time     = pp2->first_time; // keep first epoch time
                    }
                    pp->expired          = false;
                    pp->packetCount      = pp2->packetCount + 1;
                    pp->packetsDiscarded = pp2->packetsDiscarded;
                    pp->prev_sim_time    = pp2->sim_time;
                    pp->prev_time        = pp2->curr_time;
                    // accumulate total distance travelled (in nm)
                    pp->total_nm         = pp2->total_nm + (pp->dist_m * SG_METER_TO_NM);
                    SETPREVPOS(pp,pp2);  // copy POS to PrevPos to get distance travelled
                    pp->curr_time        = curr_time; // set CURRENT packet time
                    // ****************************************************
                    *pp2 = *pp;     // UPDATE the RECORD with latest info
                    // ****************************************************
                    //print_pilot(pp2,upd_by,pt_Pos);
                    //if (revived)
                    //    Pilot_Tracker_Connect(pp2);
                    //else
                    //    Pilot_Tracker_Position(pp2);
                    pt = pkt_Pos;
                } else {
                    pp2->packetsDiscarded++;
                    set_upd_type(updt);
                    return pkt_Discards;
                }
                break;
            }   // if (SAME_FLIGHT(pp,pp2)) {
        }   // for (ii = 0; ii < max; ii++)
        set_upd_type(updt);
        if (found) {
            update_flights(pp2);
        } else {
            pp->flight_id        = get_epoch_id(); // establish NEW UNIQUE ID for flight
            pp->packetCount      = 1;
            pp->packetsDiscarded = 0;
            pp->first_sim_time   = pp->prev_sim_time = pp->sim_time;
            SETPREVPOS(pp,pp); // set as SAME as current
            pp->curr_time        = curr_time; // set CURRENT packet time
            pp->pt               = pt_New;
            pp->dist_m           = 0.0;
            pp->total_nm         = 0.0;
            // ================================================
            vPilots.push_back(*pp); // *** STORE NEW RECORD ***
            // ================================================
            //print_pilot(pp,(char *)"NEW",pt_New);
            //Pilot_Tracker_Connect(pp);
            pt = pkt_First;
            strcpy(tb,"NEW");
            upd_by = tb;
            update_flights(pp);
        }

        SPRTF("%8s pos: %15.8f,%15.8f alt: %6d hdg: %3d spd: %4d %20s %s\n",
            _s_callsign, lat, lon, (int)alt, (int)hDeg, (int)spd, _s_aircraft, tb );
#ifdef ADD_PROPERTIES
        char *end = set_MsgBuf( packet, len );
        process_Props( end );
#endif // ADD_PROPERTIES

    } else if (MsgId == CHAT_MSG_ID) {
        pt = pkt_Chat;
    } else {
        pt = pkt_Other;
    }
    return pt;
}



void show_packet_stats()
{
    int i, PacketCount, Bad_Packets, DiscardCount;
    PPKTSTR pps = Get_Pkt_Str();
    PacketCount = 0;
    Bad_Packets = 0;
    DiscardCount = pps[pkt_Discards].count;
    SPRTF("\n");
    for (i = 0; i < pkt_Max; i++) {
        PacketCount += pps[i].count;
        if (i < pkt_First)
            Bad_Packets += pps[i].count;
        SPRTF("%s=%d ", pps[i].desc, pps[i].count );
    }
    SPRTF("\n");
    SPRTF("%s: Summary: Packets %d, bad %d, discards %d\n", mod_name, 
        PacketCount, Bad_Packets, DiscardCount );
}

std::string GetProtocolVerString()
{
    char buf[128];
    sprintf(buf,"%d.%d",
        PROTO_VER >> 16,
        PROTO_VER & 0xffff);
    return std::string(buf);
}


int load_cf_log_all()
{
    struct stat buf;
    const char *tf = log_file;
    if (stat(tf,&buf)) {
        SPRTF("stat of %s file failed!\n",tf);
        return 1;
    }
	SPRTF("%s: Creating %ld byte buffer...\n", mod_name, buf.st_size + 2 );
    char *tb = (char *)malloc( buf.st_size + 2 );
    if (!tb) {
        SPRTF("malloc(%d) file failed!\n",(int)buf.st_size);
        return 2;
    }
    FILE *fp = fopen(tf,"rb");
    if (!fp) {
        SPRTF("open of %s file failed!\n",tf);
        free(tb);
        return 3;
    }
	SPRTF("%s: Reading whole file into buffer...\n", mod_name );
    int len = fread(tb,1,buf.st_size,fp);
    if (len != (int)buf.st_size) {
        SPRTF("read of %s file failed!\n",tf);
        fclose(fp);
        free(tb);
        return 4;
    }
    fclose(fp);
	SPRTF("%s: Processing the raw buffer...\n", mod_name );
    int i, c;
    char *pbgn = 0;
    int packets = 0;
    int cnt = 0;
    PKT pkt;
    out_hex(tb, 16);
    for (i = 0; i < len; i++) {
        c = tb[i];
        // 53 46 47 46 00 01 00 01 00 00 00 07
        //if ((c == 'S') && (tb[i+1] == 'F') && (tb[i+2] == 'G') && (tb[i+3] == 'F')) 
        if ((tb[i+0] == 0x53)&&(tb[i+1] == 0x46)&&(tb[i+2] == 0x47)&&(tb[i+3] == 0x46)&&
            (tb[i+4] == 0x00)&&(tb[i+5] == 0x01)&&(tb[i+6] == 0x00)&&(tb[i+7] == 0x01)&&
            (tb[i+8] == 0x00)&&(tb[i+9] == 0x00)&&(tb[i+10]== 0x00)&&(tb[i+11]== 0x07)) {
            if (pbgn && cnt) {
                packets++;
                pkt.cp = pbgn;
                pkt.len = cnt;
                vPackets.push_back(pkt);
            }
            pbgn = &tb[i];
            cnt = 0;
        }
        cnt++;
    }
	SPRTF("%s: Processed the raw buffer... found %d packets.\n", mod_name, packets );
    clear_prop_stats();

    size_t max, ii;
    Packet_Type pt;
    max = vPackets.size();
    PPKTSTR ppt = Get_Pkt_Str();
    for (ii = 0; ii < max; ii++) {
        pkt = vPackets[ii];
        SPRTF("Deal with packet len %d\n", pkt.len);
        pt = Deal_With_Packet( pkt.cp, pkt.len );
        ppt[pt].count++;
    }

    show_prop_stats();
    show_chat_stats();
    show_packet_stats();
    out_flight_json();
    vPackets.clear();
    free(tb);
    return 0;
}

// stream read file
int load_cf_log()
{
    struct stat buf;
    const char *tf = log_file;
    int file_size, file_read, len;
    if (stat(tf,&buf)) {
        SPRTF("stat of %s file failed!\n",tf);
        return 1;
    }
    file_size = (int)buf.st_size;
	SPRTF("%s: Creating %d byte buffer...\n", mod_name, MAX_PACKET_SIZE+2 );
    char *tb = (char *)malloc( MAX_PACKET_SIZE+2 );
    if (!tb) {
        SPRTF("malloc(%d) file failed!\n",(int) MAX_PACKET_SIZE+2);
        return 2;
    }
    FILE *fp = fopen(tf,"rb");
    if (!fp) {
        SPRTF("open of %s file failed!\n",tf);
        free(tb);
        return 3;
    }

	SPRTF("%s: Processing file %d bytes, buffer by buffer...\n", mod_name, file_size );
    file_read = 0;
    int i, c;
    char *pbgn = 0;
    int packets = 0;
    int cnt = 0;
    int off = 0;
    //PKT pkt;
    Packet_Type pt;
    PPKTSTR ppt = Get_Pkt_Str();
    clear_prop_stats();
    clear_upd_type_stats();
    while ( file_read < file_size ) {
        len = fread(tb+off,1,MAX_PACKET_SIZE-off,fp);
        if (len <= 0)
            break;
        file_read += len;
        len += off; // add remainder from last read
        for (i = 0; i < len; i++) {
            c = tb[i];
            // 53 46 47 46 00 01 00 01 00 00 00 07
            //if ((c == 'S') && (tb[i+1] == 'F') && (tb[i+2] == 'G') && (tb[i+3] == 'F')) 
            if ((tb[i+0] == 0x53)&&(tb[i+1] == 0x46)&&(tb[i+2] == 0x47)&&(tb[i+3] == 0x46)&&
                (tb[i+4] == 0x00)&&(tb[i+5] == 0x01)&&(tb[i+6] == 0x00)&&(tb[i+7] == 0x01)&&
                (tb[i+8] == 0x00)&&(tb[i+9] == 0x00)&&(tb[i+10]== 0x00)&&(tb[i+11]== 0x07)) {
                if (pbgn && cnt) {
                    packets++;
                    if (VERB9) SPRTF("Deal with packet len %d\n", cnt);
                    pt = Deal_With_Packet( pbgn, cnt );
                    ppt[pt].count++;
                    off = len - cnt;
                    break;
                }
                pbgn = &tb[i];
                cnt = 0;
            }
            cnt++;
        }
        c = 0;
        for ( ; i < len; i++) {
            tb[c++] = tb[i];    // move remaining data to head
        }
        pbgn = 0;
        cnt = 0;
    }
    fclose(fp);
	SPRTF("%s: Processed the raw buffer... found %d packets.\n", mod_name, packets );

    show_prop_stats();
    show_chat_stats();
    show_packet_stats();
    show_upd_type_stats();
    out_flight_json();
    vPackets.clear();
    free(tb);
    return 0;
}

void test_cflog()
{
    SPRTF("\n");
    SPRTF("%s: doing cf log test...\n", module );
    load_cf_log();
    SPRTF("%s: done cf log test...\n", module );
}

#endif // #ifdef ADD_CF_LOG_TEST

// eof - cf-log.cxx
