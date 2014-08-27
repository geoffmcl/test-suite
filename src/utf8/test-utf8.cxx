/*\
 * test-utf8.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * source from : http://sourceforge.net/projects/utfcpp/postdownload?source=dlp
 * docs from : http://utfcpp.sourceforge.net/
 *
 * from : http://www.columbia.edu/~kermit/utf8.html
 * UTF-8 is an ASCII-preserving encoding method for Unicode (ISO 10646), the Universal Character Set (UCS). 
 * The UCS encodes most of the world's writing systems in a single character set, allowing you to mix languages 
 * and scripts within a document without needing any tricks for switching character sets. 
 * Currencies :  ¥ · £ · € · $ · ¢ · ? · ? · ? · ? · ? · ? · ? · ? · ? · ? · ? · ? · ? · ? · ?
 *
\*/

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../sprtf.hxx"
#include "../utils.hxx"
#include "utf8.h"
#include "test-utf8.hxx"
#ifndef iconst
#define iconst const
#endif

using namespace std;

static const char *module = "test-utf8";
static const char *sample = "D:\\SAVES\\OSM\\pern-b2.295-48.7508-2.305-48.7608.osm";
// A SAMPLE UTF-8 FILE from : http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
static const char *sample2 = "C:\\FG\\17\\test\\utf8\\UTF-8-test.txt";
// from : http://www.w3.org/2001/06/utf-8-test/postscript-utf-8.html
static const char *sample3 = "C:\\FG\\17\\test\\utf8\\ps-utf-8.txt";

//static const char *twochars = "\xe6\x97\xa5\xd1\x88";
static bool do_line_test = false;
static bool show_line_lenght = false;

bool valid_utf8_file(iconst char* file_name)
{
    ifstream ifs(file_name);
    if (!ifs)
        return false; // even better, throw here

    istreambuf_iterator<char> it(ifs.rdbuf());
    istreambuf_iterator<char> eos;

    return utf8::is_valid(it, eos);
}

int line_by_line_main(const char *file)
{
    //if (argc != 2) {
    //    cout << "\nUsage: docsample filename\n";
    //    return 0;
    //}

    const char* test_file_path = file;  // argv[1];
    // Open the test file (contains UTF-8 encoded text)
    ifstream fs8(test_file_path);
    if (!fs8.is_open()) {
        SPRTF("%s: Could not open '%s'\n", module, test_file_path );
        return 1;
    }

    unsigned int line_count = 1;
    unsigned int failed_lines = 0;
    string line;
    // Play with all the lines in the file
    while (getline(fs8, line)) {
       // check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
        string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
        if (end_it != line.end()) {
            SPRTF("%s: Invalid UTF-8 encoding detected at line %d!\n", module, line_count );
            SPRTF("This part is fine: '%s'\n", string(line.begin(), end_it).c_str() );
            failed_lines++;
        }

        if (show_line_lenght) {
            // Get the line length (at least for the valid part)
            int length = utf8::distance(line.begin(), end_it);
            SPRTF("%s: Length of line %d is %d\n", module, line_count, length );
        }

        // Convert it to utf-16
        vector<unsigned short> utf16line;
        utf8::utf8to16(line.begin(), end_it, back_inserter(utf16line));

        // And back to utf-8
        string utf8line; 
        utf8::utf16to8(utf16line.begin(), utf16line.end(), back_inserter(utf8line));

        // Confirm that the conversion went OK:
        if (utf8line != string(line.begin(), end_it))
            SPRTF("%s: Error in UTF-16 conversion at line: %d\n", line_count );        

        line_count++;
    }

    SPRTF("%s: Processed %d line, %d FAILED UTF-8 test.\n", module, line_count, failed_lines);

    return 0;
}

// implementation
void test_utf8()
{
    if (valid_utf8_file(sample)) {
        SPRTF("%s: File '%s' is valid UTF-8\n", module, sample);
    } else {
        SPRTF("%s: File '%s' is NOT valid UTF-8\n", module, sample);
    }
    if (do_line_test)
        line_by_line_main(sample);

    if (is_file_or_directory32(sample3) == DT_FILE) {
        SPRTF("%s: Found and testing '%s'\n", module, sample3 );
        line_by_line_main(sample3);
    } else if (is_file_or_directory32(sample2) == DT_FILE) {
        SPRTF("%s: Found and testing '%s'\n", module, sample2 );
        line_by_line_main(sample2);
    } else {
        SPRTF("%s: Unable to locate sample file '%s'! *** FIX ME ***\n", module, sample2);
    }
}

// eof = test-utf8.cxx
