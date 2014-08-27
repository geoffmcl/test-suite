/*\
 * test-wildcard.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * from : http://www.codeproject.com/Articles/188256/A-Simple-Wildcard-Matching-Function
\*/

#include <stdio.h>
// other includes
#include "test-wildcard.hxx"
#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>

typedef std::vector<std::string> vSTG;

static const char *module = "test-wildcard";


//////////////////////////////////////////////////////////////////////////
//    WildcardMatch
//        pszString    - Input string to match
//        pszMatch    - Match mask that may contain wildcards like ? and *
//    
//        A ? sign matches any character, except an empty string.
//        A * sign matches any string inclusive an empty string.
//        Characters are compared caseless

bool WildcardMatch(const TCHAR *pszString, const TCHAR *pszMatch)
{
    // We have a special case where string is empty ("") and the mask is "*".
    // We need to handle this too. So we can't test on !*pszString here.
    // The loop breaks when the match string is exhausted.
    while (*pszMatch)
    {
        // Single wildcard character
        if (*pszMatch==_T('?'))
        {
            // Matches any character except empty string
            if (!*pszString)
                return false;

            // OK next
            ++pszString;
            ++pszMatch;
        }
        else if (*pszMatch==_T('*'))
        {
            // Need to do some tricks.

            // 1. The wildcard * is ignored. 
            //    So just an empty string matches. This is done by recursion.
            //      Because we eat one character from the match string, the
            //      recursion will stop.
            if (WildcardMatch(pszString,pszMatch+1))
                // we have a match and the * replaces no other character
                return true;

            // 2. Chance we eat the next character and try it again, with a
            //    wildcard * match. This is done by recursion. Because we eat
            //      one character from the string, the recursion will stop.
            if (*pszString && WildcardMatch(pszString+1,pszMatch))
                return true;

            // Nothing worked with this wildcard.
            return false;
        }
        else
        {
            // Standard compare of 2 chars. Note that *pszSring might be 0
            // here, but then we never get a match on *pszMask that has always
            // a value while inside this loop.
            if (::CharUpper(MAKEINTRESOURCE(MAKELONG(*pszString++,0)))
		!=::CharUpper(MAKEINTRESOURCE(MAKELONG(*pszMatch++,0))))
                return false;
        }
    }

    // Have a match? Only if both are at the end...
    return !*pszString && !*pszMatch;
}

bool has_wild_card( const char *file )
{
    int c;
    size_t ii, len = strlen(file);
    for (ii = 0; ii < len; ii++) {
        c = file[ii];
        if (( c == '?' ) || ( c == '*' ))
            return true;
    }
    return false;
}

#ifndef PATH_SEP
#define PATH_SEP "\\"
#endif

int expand_filenames( const char *path, int *pargc, char ***pargv )
{
    static char drive[_MAX_DRIVE];
    static char dir[_MAX_DIR];
    static char fname[_MAX_FNAME];
    static char ext[_MAX_EXT];
    errno_t err = _splitpath_s(path,drive,_MAX_DRIVE,dir,_MAX_DIR,fname,_MAX_FNAME,ext,_MAX_EXT);
    if (err)
        return -1;  // failed
    std::string s = fname;
    s += ext;
    if (!has_wild_card(s.c_str()))
        return 0;   // nothing to do
    std::string find = drive;
    find += dir;    // aleady has trailing path separator
    //find += PATH_SEP;
    std::string root = find;
    find += "*.*";  // find ALL items
    WIN32_FIND_DATA fd;
    HANDLE hf = FindFirstFile(find.c_str(), &fd);
    if (hf == INVALID_HANDLE_VALUE)
        return -1;  // failed
    vSTG found;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // deal with a DIRECTORY
        } else {
            std::string file = fd.cFileName;
            if (WildcardMatch( file.c_str(), s.c_str() )) {
                std::string full = root;
                full += file;
                found.push_back(full);  // got one
            }
        }
    } while (FindNextFile(hf,&fd));
    FindClose(hf);
    char **argv = 0;
    size_t ii, max = found.size();
    if (max) {
        argv = new char *[max+1];
        if (!argv) {
            printf("Memory failed!\n");
            return -1;
        }
    }
    //printf("%s: Found %d files matching '%s'\n", module, (int)max, path);
    for (ii = 0; ii < max; ii++) {
        s = found[ii];
        //printf("%d: %s\n", (int)(ii + 1), s.c_str());
        argv[ii] = _strdup(s.c_str());
    }
    if (argv) {
        argv[ii] = 0;
        *pargv = argv;
        *pargc = (int)max;
    }
    return (int)max;
}

// TODO: Would be even better IF wild cards could also be in the directory names, but that is for the future ;=))
void test_wildcard()
{
    const char *example = "C:\\FG\\17\\test\\build\\*.vcxproj";
    int argc = 0;
    char **argv;
    int res = expand_filenames(example,&argc,&argv);
    if (res > 0) {
        printf("%s: Got %d matches to '%s'\n", module, res, example);
        for (int i = 0; i < argc; i++) {
            printf("%s\n", argv[i]);
            free(argv[i]);
        }
        delete argv;
    }
}

int expand_args( int *pargc, char ***pargv )
{
    int i, argc = *pargc;
    char **argv = *pargv;
    size_t ii, len;
    int c;
    char *arg;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        len = strlen(arg);
        for (ii = 0; ii < len; ii++) {
            c = arg[ii];
            if ((c == '*') || (c == '?'))
                break;
        }
    }
    if (i == argc)
        return 0;   // no wild cards found in commands

    return 1;
}


#else   // !WIN32

#endif // WIN32 y/n
// eof = test-wildcard.cxx
