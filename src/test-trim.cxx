/*\
 * test-trim.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * Test various methods for 'trimming' a std::string
 *
 * from : http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
 *
\*/

#include <stdio.h>
#include <cassert>
#include <string>
#include <algorithm> // find_if() 
#include <functional> // not1(), ptr_fun<...>, ..
#include <cctype>
#include <iostream>
#include "test-trim.hxx"

static const char *module = "test-trim";

/////////////////////////////////////////////////////////
// example 1
// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &lrtrim(std::string &s) {
        return ltrim(rtrim(s));
}


// modifies input string, returns input
std::string& trim_left_in_place(std::string& str) {
    size_t i = 0;
    while(i < str.size() && isspace(str[i])) { ++i; };
    return str.erase(0, i);
}

std::string& trim_right_in_place(std::string& str) {
    size_t i = str.size();
    while(i > 0 && isspace(str[i - 1])) { --i; };
    return str.erase(i, str.size());
}

std::string& trim_in_place(std::string& str) {
    return trim_left_in_place(trim_right_in_place(str));
}

// returns newly created strings

std::string trim_right(std::string str) {
    return trim_right_in_place(str);
}

std::string trim_left(std::string str) {
    return trim_left_in_place(str);
}

std::string strim(std::string str) {
    return trim_left_in_place(trim_right_in_place(str));
}

std::string trim_by_ptr(char const *str)
{
  // Trim leading non-letters
  while(!isalnum(*str)) str++;

  // Trim trailing non-letters
  char const *end = str + strlen(str) - 1;
  while(end > str && !isalnum(*end)) end--;

  return std::string(str, end+1);
}

void test1() 
{

    std::string s1(" \t\r\n  ");
    std::string s2("  \r\nc");
    std::string s3("c \t");
    std::string s4("  \rc ");

    // strim returns a new string, NOT modifying the original
    assert(strim(s1) == "");
    assert(strim(s2) == "c");
    assert(strim(s3) == "c");
    assert(strim(s4) == "c");

    assert(s1 == " \t\r\n  ");
    assert(s2 == "  \r\nc");
    assert(s3 == "c \t");
    assert(s4 == "  \rc ");

    // trim_in_place modifies the current string
    assert(trim_in_place(s1) == "");
    assert(trim_in_place(s2) == "c");
    assert(trim_in_place(s3) == "c");
    assert(trim_in_place(s4) == "c");

    assert(s1 == "");
    assert(s2 == "c");
    assert(s3 == "c");
    assert(s4 == "c");  
}

std::string choppa(const std::string &t, const std::string &ws)
{
    std::string str = t;
    size_t found;
    found = str.find_last_not_of(ws);
    if (found != std::string::npos)
    	str.erase(found+1);
    else
    	str.clear();            // str is all whitespace

    return str;
}

//////////////////////////////////////////////////////////////////////////////
// Example 4
class StringUtils
{
public:
    StringUtils() { WHITESPACE = " \n\r\t"; }
    ~StringUtils() { }

    std::string WHITESPACE;   // = " \n\r\t";
    //const std::string StringUtils::WHITESPACE = " \n\r\t";
    std::string Trim(const std::string& s);
    std::string TrimLeft(const std::string& s);
    std::string TrimRight(const std::string& s);
};

//const std::string StringUtils::WHITESPACE = " \n\r\t";

std::string StringUtils::Trim(const std::string& s)
{
    return TrimRight(TrimLeft(s));
}

std::string StringUtils::TrimLeft(const std::string& s)
{
    size_t startpos = s.find_first_not_of(StringUtils::WHITESPACE);
    return (startpos == std::string::npos) ? "" : s.substr(startpos);
}

std::string StringUtils::TrimRight(const std::string& s)
{
    size_t endpos = s.find_last_not_of(StringUtils::WHITESPACE);
    return (endpos == std::string::npos) ? "" : s.substr(0, endpos+1);
}

void test2()
{
    std::string s1(" \t\r\n  ");
    std::string s2("  \r\nc");
    std::string s3("c \t");
    std::string s4("  \rc ");
    std::string s5("");
    std::string tmp;
    StringUtils su;

    tmp = su.Trim(s1);
    assert(tmp == "");
    tmp = su.Trim(s2);
    assert(tmp == "c");
    tmp = su.Trim(s3);
    assert(tmp == "c");
    tmp = su.Trim(s4);
    assert(tmp == "c");
    tmp = su.Trim(s5);
    assert(tmp == "");

}

//////////////////////////////////////////////////////////////////
// from : http://stackoverflow.com/questions/14233065/remove-whitespace-in-stdstring

std::string remove_white_space(std::string &s)
{
    s.erase( std::remove_if( s.begin(), s.end(), ::isspace ), s.end() );
    return s;
}

void test3()
{
    std::string s("\t\tHELLO WORLD\r\nHELLO\t\nWORLD     \t");
    std::string r = remove_white_space(s);
    printf("Got '%s'\n", s.c_str() );

}

//////////////////////////////////////////////////////////////////////////
// from : http://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string

std::string trim_ws(const std::string& str,
                 const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string reduce(const std::string& str,
                   const std::string& fill = " ",
                   const std::string& whitespace = " \t")
{
    // trim first
    auto result = trim_ws(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)
    {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}

void test4()
{
    const std::string foo = "    too much\t   \tspace\t\t\t  ";
    const std::string bar = "one\ntwo";
    std::string r;

    r = reduce(foo);
    printf("Got '%s'\n", r.c_str() );
    r = reduce(bar);
    printf("Got '%s'\n", r.c_str() );

}

void test5()
{
    std::string foo = "    too much  space   ";
    std::remove(foo.begin(), foo.end(), ' ');
    printf("Got '%s'\n", foo.c_str() );

}

/////////////////////////////////////////////////////////////////////////////
// just right and left trimming
// from : http://panthema.net/2007/0530-StdString-Trim.html

static inline void string_trim_left_inplace(std::string &str)
{
    str.erase(0, str.find_first_not_of(' '));
}

static inline void string_trim_right_inplace(std::string &str)
{
    str.erase(str.find_last_not_of(' ') + 1, std::string::npos);
}

static inline std::string string_trim_left(const std::string &str)
{
    std::string::size_type pos = str.find_first_not_of(' ');
    if (pos == std::string::npos) return std::string();

    return str.substr(pos, std::string::npos);
}

static inline std::string string_trim_right(const std::string &str)
{
    std::string::size_type pos = str.find_last_not_of(' ');
    if (pos == std::string::npos) return std::string();

    return str.substr(0, pos + 1);
}

static inline std::string string_trim(const std::string& str)
{
    std::string::size_type pos1 = str.find_first_not_of(' ');
    if (pos1 == std::string::npos) return std::string();

    std::string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 == std::string::npos) return std::string();

    return str.substr(pos1 == std::string::npos ? 0 : pos1,
                      pos2 == std::string::npos ? (str.length() - 1) : (pos2 - pos1 + 1));
}

static inline void string_trim_inplace(std::string& str)
{
    std::string::size_type pos = str.find_last_not_of(' ');
    if(pos != std::string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if(pos != std::string::npos) str.erase(0, pos);
    }
    else
        str.erase(str.begin(), str.end());
}

void test6()
{
    // string-copy functions
    assert( string_trim_left("  abc  ") == "abc  " );
    assert( string_trim_left("abc  ") == "abc  " );
    assert( string_trim_left("  ") == "" );

    assert( string_trim_right("  abc  ") == "  abc" );
    assert( string_trim_right("  abc") == "  abc" );
    assert( string_trim_right("  ") == "" );

    assert( string_trim("  abc  ") == "abc" );
    assert( string_trim("abc  ") == "abc" );
    assert( string_trim("  abc") == "abc" );
    assert( string_trim("  ") == "" );

    // in-place functions
    std::string str1 = "  abc  ";
    std::string str2 = "abc  ";
    std::string str3 = "  ";

    string_trim_left_inplace(str1);
    string_trim_left_inplace(str2);
    string_trim_left_inplace(str3);

    assert( str1 == "abc  " );
    assert( str2 == "abc  " );
    assert( str3 == "" );

    str1 = "  abc  ";
    str2 = "  abc";
    str3 = "  ";

    string_trim_right_inplace(str1);
    string_trim_right_inplace(str2);
    string_trim_right_inplace(str3);

    assert( str1 == "  abc" );
    assert( str2 == "  abc" );
    assert( str3 == "" );

    str1 = "  abc  ";
    str2 = "  abc";
    str3 = "abc  ";
    std::string str4 = "  ";

    string_trim_inplace(str1);
    string_trim_inplace(str2);
    string_trim_inplace(str3);
    string_trim_inplace(str4);

    assert( str1 == "abc" );
    assert( str2 == "abc" );
    assert( str3 == "abc" );
    assert( str4 == "" );

}

/////////////////////////////////////////////////////////////////
// simple trim
// from : http://www.toptip.ca/2010/03/trim-leading-or-trailing-white-spaces.html

static std::string ws(" \t\r\n");
void simple_trim(std::string & s)
{
   size_t p = s.find_first_not_of(ws);
   s.erase(0, p);

   p = s.find_last_not_of(ws);
   if (std::string::npos != p)
      s.erase(p+1);
}

void test7()
{
   std::string a = "   a   ";
   std::string b = "";
   std::string c = "       ";
   std::string d = "   \t  ";

   std::cout << "inputs "
       << ":" << a
       << ":" << b
       << ":" << c
       << ":" << d
       << ":" << std::endl;

   simple_trim(a);
   simple_trim(b);
   simple_trim(c);
   simple_trim(d);

   std::cout << "result "
       << ":" << a
       << ":" << b
       << ":" << c
       << ":" << d
       << ":" << std::endl;

}

void test_trim()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
}


// eof = test-trim.cxx
