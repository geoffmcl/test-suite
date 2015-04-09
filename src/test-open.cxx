/*\
 * test-open.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h> // for close(), ...
#endif
#include <fcntl.h>
#include "sprtf.hxx"
#include "utils.hxx"
#include "test-open.hxx"

static const char *module = "test-open";

#ifndef _MSC_VER
#define is_file_or_directory64 is_file_or_directory32
#endif

void test_open()
{
    SPRTF("%s: test io open...\n", module );
    const char *dir = "CMakeFiles";
    if (is_file_or_directory64(dir) == MDT_DIR) {
        SPRTF("dir %s exists...\n",dir);
    } else {
        SPRTF("dir %s does not exists...\n",dir);
    }
    int fd = open(dir,O_RDONLY);
    if (fd <= 0) {
        perror("open failed");
        SPRTF("Failed to open directory '%s'\n",dir);

    } else {
        SPRTF("Open directory '%s'\n",dir);
        close(fd);
    }
    SPRTF("%s: end test io open...\n", module );
}


// eof = test-open.cxx
