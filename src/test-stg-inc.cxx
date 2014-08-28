/*\
 * test-stg-inc.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * Some ideas from : http://rosettacode.org/wiki/Increment_a_numerical_string
\*/

#include <stdio.h>
#ifndef _MSC_VER
#include <string.h> // for strcmp(), ...
#endif
#include <cstdlib>
#include <string>
#include <sstream>

#include "test-stg-inc.hxx"

static const char *module = "test-stg-inc";

// implementation
/*  Constraints: input is in the form of (\+|-)?[0-9]+
 *  and without leading zero (0 itself can be as "0" or "+0", but not "-0");
 *  input pointer is realloc'able and may change;
 *  if input has leading + sign, return may or may not keep it.
 *  The constranits conform to sprintf("%+d") and this function's own output.
 */
char * increment_string_in_buffer(char *s)
{
	int i, begin, tail, len;
	int neg = (*s == '-');
	char tgt = neg ? '0' : '9';
 
	/* special case: "-1" */
	if (!strcmp(s, "-1")) {
		s[0] = '0', s[1] = '\0';
		return s;
	}
 
	len = strlen(s);
	begin = (*s == '-' || *s == '+') ? 1 : 0;
 
	/* find out how many digits need to be changed */
	for (tail = len - 1; tail >= begin && s[tail] == tgt; tail--);
 
	if (tail < begin && !neg) {
		/* special case: all 9s, string will grow */
		if (!begin) {
            s = (char *)realloc(s, len + 2);
            if (!s) {
                printf("realloc(%d) FAILED!\n", len + 2 );
                exit(1);
            }
        }
		s[0] = '1';
		for (i = 1; i <= len - begin; i++) s[i] = '0';
		s[len + 1] = '\0';
	} else if (tail == begin && neg && s[1] == '1') {
		/* special case: -1000..., so string will shrink */
		for (i = 1; i < len - begin; i++) s[i] = '9';
		s[len - 1] = '\0';
	} else { /* normal case; change tail to all 0 or 9, change prev digit by 1*/
		for (i = len - 1; i > tail; i--)
			s[i] = neg ? '9' : '0';
		s[tail] += neg ? -1 : 1;
	}
 
	return s;
}

// Works with: g++ version 4.0.2 for an (almost) arbitrary large number
void increment_numerical_string(std::string &s)
{
    std::string::reverse_iterator iter = s.rbegin(), end = s.rend();
    int carry = 1;
    int insat = 0;
    while (carry && iter != end)
    {
        char c = *iter;
        if ((c == '-') || (c == '+')) {
            // ASSUMED is the FIRST character
            insat = 1;
            break;
        }
        int value = (c - '0') + carry;
        carry = (value / 10);
        *iter = '0' + (value % 10);
        ++iter;
    }
    if (carry) {
        s.insert(insat, "1");
    }
}

void string_test( std::string s )
{
    char *cp = (char *)malloc( s.length() + 2 );
    long long i;
    if (!cp) {
        printf("memory allocation FAILED!\n");
        exit(1);
    }
    std::string s2(s);
    strcpy(cp,s.c_str());
    std::istringstream(s) >> i;
    i++;
    printf("text: %s ", s.c_str());
    // NOTE LIMITS OF THIS METHOD
    std::ostringstream oss;
    if (oss << i) {
        s = oss.str();
        printf("res1: %s ", s.c_str());
    } else {
        printf("FAILED!");
    }
    printf("\n");

    cp = increment_string_in_buffer(cp);
    printf("res2: %s ", cp);
    increment_numerical_string(s2);
    printf("res3: %s ", s2.c_str());

    printf("\n");
    free(cp);
}

void test_string_inc()
{

	string_test("+0");
	string_test("-1");
	string_test("-41");
	string_test("+41");
	string_test("999");
	string_test("+999");
	string_test("109999999999999999999999999999999999999999");
	string_test("-100000000000000000000000000000000000000000000");
    std::string big_number = "123456789012345678901234567899";
    string_test(big_number);
}

// eof = test-stg-inc.cxx
