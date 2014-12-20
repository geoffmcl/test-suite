/*\
 * test-map.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#ifndef _MSC_VER
#include <stdlib.h> // for exit(), ...
#endif
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "sprtf.hxx"
#include "test-map.hxx"

static const char *module = "test-map";

typedef struct tagAPT {
    double lat,lon;
    unsigned long ulid;
}APT, *PAPT;

typedef std::map<unsigned long,APT> mNODES;
typedef mNODES::iterator mNODESi;

static mNODES mNodes;
#ifndef MEOL
#define MEOL std::endl
#endif

// implementation

// add strings to a map, and keep count of repeats
typedef std::map<std::string,int> mSTGINT;
typedef mSTGINT::iterator iSTGINT;
const char *map_stgs[] = {
    "one",
    "two",
    "three",
    "two",
    "three",
    "three",
    0 };


void test_map2() 
{
    mSTGINT *pms = new mSTGINT;
    iSTGINT ii;
    std::string s,s2;
    int i,cnt;
    bool fnd = false;
    for (i = 0; map_stgs[i]; i++) {
        s = map_stgs[i];
        fnd = false;
        for (ii = pms->begin(); ii != pms->end(); ii++) {
            cnt = (*ii).second;
            s2  = (*ii).first;
            if (s == s2) {
                fnd = true;
                cnt++;
                (*ii).second = cnt;
                break;
            }
        }
        if (!fnd) {
            (*pms)[s] = 1;
        }

    }
    SPRTF("%s: test_map2 results...\n", module);
    for (ii = pms->begin(); ii != pms->end(); ii++) {
        cnt = (*ii).second;
        s2  = (*ii).first;
        SPRTF("test_map2: string %s count %d\n", s2.c_str(), cnt );
    }
    SPRTF("%s: Done test_map2...\n", module);

}

void test_map()
{
    test_map2();
    double lat,lon;
    unsigned long ulid;
    APT apt;
    mNODESi iter;
    // generate a set of point
    std::stringstream ss;
    SPRTF("\n");
    SPRTF("%s: test map ...\n", module );
    for (lat = -80.0; lat < 90.0; lat += 10.0) {
        for (lon = -170.0; lon < 180.0; lon += 10.0) {
            apt.lat = lat;
            apt.lon = lon;
            apt.ulid = ((int)(lat + 90) * 1000) + (int)(lon + 180);
            mNodes[apt.ulid] = apt;
            //MyNodes[apt.ulid].
        }
    }
    for( iter = mNodes.begin(); iter != mNodes.end(); ++iter ) {
        apt = (*iter).second;
        ulid = (*iter).first;
        //ss << ulid << " ";
        //ss << apt.lat << "," << apt.lon << MEOL;
    }
    ss << MEOL;
    SPRTF("%s", ss.str().c_str());
    ss.str("");

    ulid = 170290;
    //////////////////////////////////////////
    ss << "Check for value " << ulid << MEOL;
    ss << ".find(val)  ";
    iter = mNodes.find(ulid);
    if (iter == mNodes.end()) {
        ss << ulid << " NOT FOUND" << MEOL;
    } else {
        apt = (*iter).second;
        ulid = (*iter).first;
        ss << "Found " << ulid << " ";
        ss << apt.lat << "," << apt.lon << MEOL;
    }
    ss << ".count(val)  ";
    if (mNodes.count(ulid)) {
        apt = mNodes[ulid]; // extract 
        ss << ulid << " ";
        ss << apt.lat << "," << apt.lon << MEOL;
    } else {
        ss << ulid << " DOES NOT EXIST" << MEOL;
    }
    apt = mNodes[ulid]; // extract valid
    ss << "Extract[val] " << ulid << " ";
    ss << apt.lat << "," << apt.lon << MEOL;
    /////////////////////////////////////////////////////
    
    ss << MEOL;
    SPRTF("%s", ss.str().c_str());
    ss.str("");

    ulid++; // invalid
    ////////////////////////////////////////////////
    ss << "Check for value " << ulid << MEOL;
    ss << ".find(val)  ";
    iter = mNodes.find(ulid);
    if (iter == mNodes.end()) {
        ss << ulid << " NOT FOUND" << MEOL;
    } else {
        apt = (*iter).second;
        ulid = (*iter).first;
        ss << "Found " << ulid << " ";
        ss << apt.lat << "," << apt.lon << MEOL;
    }
    ss << ".count(val)  ";
    if (mNodes.count(ulid)) {
        apt = mNodes[ulid]; // extract 
        ss << ulid << " ";
        ss << apt.lat << "," << apt.lon << MEOL;
    } else {
        ss << ulid << " DOES NOT EXIST" << MEOL;
    }
    apt = mNodes[ulid]; // extract valid
    ss << "Extract[val] " << ulid << " ";
    ss << apt.lat << "," << apt.lon << MEOL;
    ss << "And TAKE CARE, it seems after this 'extraction' it DOES exist! But as zero/nuls" << MEOL;
    ss << ".find(val)  ";
    iter = mNodes.find(ulid);
    if (iter == mNodes.end()) {
        ss << ulid << " NOT FOUND" << MEOL;
    } else {
        apt = (*iter).second;
        ulid = (*iter).first;
        ss << "Found " << ulid << " ";
        ss << apt.lat << "," << apt.lon << MEOL;
    }
    ss << ".count(val)  ";
    if (mNodes.count(ulid)) {
        apt = mNodes[ulid]; // extract 
        ss << ulid << " ";
        ss << apt.lat << "," << apt.lon << MEOL;
    } else {
        ss << ulid << " DOES NOT EXIST" << MEOL;
    }
    SPRTF("%s", ss.str().c_str());
    ////////////////////////////////////////////////

    SPRTF("%s: end test map ...\n", module );
}

// eof = test-map.cxx
