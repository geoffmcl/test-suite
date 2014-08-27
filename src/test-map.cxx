/*\
 * test-map.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

// other includes
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
void test_map()
{
    double lat,lon;
    unsigned long ulid;
    APT apt;
    mNODESi iter;
    // generate a set of point

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
        //std::cout << ulid << " ";
        //std::cout << apt.lat << "," << apt.lon << MEOL;
    }
    std::cout << MEOL;

    ulid = 170290;
    //////////////////////////////////////////
    std::cout << "Check for value " << ulid << MEOL;
    std::cout << ".find(val)  ";
    iter = mNodes.find(ulid);
    if (iter == mNodes.end()) {
        std::cout << ulid << " NOT FOUND" << MEOL;
    } else {
        apt = (*iter).second;
        ulid = (*iter).first;
        std::cout << "Found " << ulid << " ";
        std::cout << apt.lat << "," << apt.lon << MEOL;
    }
    std::cout << ".count(val)  ";
    if (mNodes.count(ulid)) {
        apt = mNodes[ulid]; // extract 
        std::cout << ulid << " ";
        std::cout << apt.lat << "," << apt.lon << MEOL;
    } else {
        std::cout << ulid << " DOES NOT EXIST" << MEOL;
    }
    apt = mNodes[ulid]; // extract valid
    std::cout << "Extract[val] " << ulid << " ";
    std::cout << apt.lat << "," << apt.lon << MEOL;
    /////////////////////////////////////////////////////
    
    std::cout << MEOL;

    ulid++; // invalid
    ////////////////////////////////////////////////
    std::cout << "Check for value " << ulid << MEOL;
    std::cout << ".find(val)  ";
    iter = mNodes.find(ulid);
    if (iter == mNodes.end()) {
        std::cout << ulid << " NOT FOUND" << MEOL;
    } else {
        apt = (*iter).second;
        ulid = (*iter).first;
        std::cout << "Found " << ulid << " ";
        std::cout << apt.lat << "," << apt.lon << MEOL;
    }
    std::cout << ".count(val)  ";
    if (mNodes.count(ulid)) {
        apt = mNodes[ulid]; // extract 
        std::cout << ulid << " ";
        std::cout << apt.lat << "," << apt.lon << MEOL;
    } else {
        std::cout << ulid << " DOES NOT EXIST" << MEOL;
    }
    apt = mNodes[ulid]; // extract valid
    std::cout << "Extract[val] " << ulid << " ";
    std::cout << apt.lat << "," << apt.lon << MEOL;
    std::cout << "And TAKE CARE, it seems after this 'extraction' it DOES exist! But as zero/nuls" << MEOL;
    std::cout << ".find(val)  ";
    iter = mNodes.find(ulid);
    if (iter == mNodes.end()) {
        std::cout << ulid << " NOT FOUND" << MEOL;
    } else {
        apt = (*iter).second;
        ulid = (*iter).first;
        std::cout << "Found " << ulid << " ";
        std::cout << apt.lat << "," << apt.lon << MEOL;
    }
    std::cout << ".count(val)  ";
    if (mNodes.count(ulid)) {
        apt = mNodes[ulid]; // extract 
        std::cout << ulid << " ";
        std::cout << apt.lat << "," << apt.lon << MEOL;
    } else {
        std::cout << ulid << " DOES NOT EXIST" << MEOL;
    }

    ////////////////////////////////////////////////

    exit(1);
}

// eof = test-map.cxx
