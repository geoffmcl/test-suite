/*\
 * test-mmap.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
// other includes
#include "test-mmap.hxx"

static const char *module = "test-mmap";

typedef Osmium::Storage::ById::MmapFile<uint64_t> storage_mmap_t;

void test_mmap1()
{
    std::string file = "";
    //std::string file = "F:\\DTEMP\\mmap.log";
    uint64_t val = 1234;
    uint64_t id  = 4567;
    int set_cnt = 0;
    int get_cnt = 0;
    double start = get_seconds();
    const char *cpf = (file.length() ? file.c_str() : "TEMPORARY");
    SPRTF("Creating mapping file '%s'\n", cpf);

    storage_mmap_t mf(file,false);

    SPRTF("Writing id %s, with value %s\n", get_I64u_Stg(id), get_I64u_Stg(val) );
    mf.set(id,val);
    set_cnt++;
    SPRTF("Writing 4567 ids, down to 0, with new values...\n");
    while( id > 0 ) {
        id--;
        val++;
        mf.set(id,val);
        set_cnt++;
    }

    val = 4567;
    id = 2480851771;
    SPRTF("Writing id %s, with value %s\n", get_I64u_Stg(id), get_I64u_Stg(val) );
    mf.set(id,val);
    set_cnt++;

    val = 8901;
    id  = 938304475;
    SPRTF("Writing id %s, with value %s\n", get_I64u_Stg(id), get_I64u_Stg(val) );
    mf.set(id,val);
    set_cnt++;

    val = 1234;
    id  = 4567;
    SPRTF("Getting id %s value expect %s ", get_I64u_Stg(id), get_I64u_Stg(val) );
    uint64_t val2 = mf[id];
    get_cnt++;
    if (val2 == val) {
        SPRTF("got %s - SUCCESS!\n", get_I64u_Stg(val2));
        val = 4567;
        id = 2480851771;
        SPRTF("Getting id %s value expect %s ", get_I64u_Stg(id), get_I64u_Stg(val) );
        val2 = mf[id];
        get_cnt++;
        SPRTF("got %s - %s!\n", get_I64u_Stg(val2),
            ((val == val2) ? "SUCCESS" : "FAILED"));
        val = 8901;
        id  = 938304475;
        SPRTF("Getting id %s value expect %s ", get_I64u_Stg(id), get_I64u_Stg(val) );
        val2 = mf[id];
        get_cnt++;
        SPRTF("got %s - %s!\n", get_I64u_Stg(val2),
            ((val == val2) ? "SUCCESS" : "FAILED"));
        SPRTF("Did %d set, and %d get from backing store in %s secc\n", set_cnt, get_cnt,
            get_elapsed_stg(start));

    } else {
        SPRTF("got %s - FAILED to get same id value!\n", get_I64u_Stg(val2));
    }
    uint64_t used = mf.used_memory();
    char *src = get_I64u_Stg(used);
    char *dst = GetNxtBuf();
    nice_num(dst,src);
    SPRTF("Used memory is %s (%s)\n", dst, get_k_num(used));

    used = 0;
}

void test_mmap()
{
    test_mmap1();
    //exit(1);
}


// eof = test-mmap.cxx
