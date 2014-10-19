/*\
 * test-trim.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _TEST_TRIM_HXX_
#define _TEST_TRIM_HXX_
#include <string>
#include <vector>

extern void test_trim();    // test various methods of trimming a std::string s

//////////////////////////////////////////////////////////////////
// from : http://stackoverflow.com/questions/14233065/remove-whitespace-in-stdstring
extern std::string remove_white_space(std::string &s);

//////////////////////////////////////////////////////////////////////////////
// very useful 'split' function
typedef std::vector<std::string> vSTG;

extern vSTG split_whitespace( const std::string &str, int maxsplit = 0 );
extern vSTG string_split( const std::string &str, const char* sep = 0, int maxsplit = 0 );


#endif // #ifndef _TEST_TRIM_HXX_
// eof - test-trim.hxx
