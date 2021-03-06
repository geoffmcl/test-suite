/*
    This source code was extracted from the Q8 package created and
    placed in the PUBLIC DOMAIN by Doug Gwyn <gwyn@arl.mil>
    last edit:	1999/11/05	gwyn@arl.mil

	Implements subclause 7.8.2 of ISO/IEC 9899:1999 (E).

	This particular implementation requires the matching <inttypes.h>.
	It also assumes that character codes for A..Z and a..z are in
	contiguous ascending order; this is true for ASCII but not EBCDIC.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string> // this should be already included in <sstream>
#ifdef _MSC_VER
//////////////////////////////////////////////////////////////////////////////
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;
#ifndef INTMAX_MAX
#define INTMAX_MAX LLONG_MAX    // INT_MAX
#endif
#ifndef INTMAX_MIN
#define INTMAX_MIN LLONG_MIN    // INT_MIN
#endif
#define strtoll _strtoi64
#define strtoull _strtoui64
///////////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////////
#include <inttypes.h>
#include <algorithm>    // for std::reverse
//////////////////////////////////////////////////////////////////////////////
#endif
#include "sprtf.cxx"

static const char *module = "strtoimax";

// from : http://www.cplusplus.com/forum/general/10898/
// Helios_dec2bin
static std::string dec2bin(unsigned n)
{
    const int size = sizeof(n) * 8;
    std::string res;
    bool s = 0;
    for (int a = 0; a < size; a++) {
        bool bit = (n >> (size - 1)) ? true : false;
        if (bit)
            s = 1;
        if (s)
            res.push_back(bit + '0');
        n <<= 1;
    }
    if (!res.size())
        res.push_back('0');
    return res;
}

// other methods 
// the fastest it seems....
std::string Duoas_dec2bin(unsigned n) 
{
    char result[(sizeof(unsigned) * 8) + 1];
    unsigned index = sizeof(unsigned) * 8;
    result[index] = '\0';
    do result[--index] = '0' + (n & 1);
    while (n >>= 1);
    return std::string(result + index);
}

std::string PanGalactic_dec2bin(unsigned n) {
    std::string res;
    while (n) {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }
    if (res.empty())
        res = "0";
    else
        std::reverse(res.begin(), res.end());
    return res;
}
typedef std::string(*func_p)(unsigned);

unsigned rnd32() {
    return rand() | (rand() << 15) | (rand() << 30);
}

int time_test() 
{
    std::stringstream ss;
    unsigned loops = 10000000;  // 100000;
    srand(time(0));
    func_p array[] = {
        &Duoas_dec2bin,
        &PanGalactic_dec2bin,
        &dec2bin,   // &Helios_dec2bin,
        0
    };
    const char *names[] = {
        "Duoas_dec2bin",
        "PanGalactic_dec2bin",
        "Helios_dec2bin",
        0
    };
    SPRTF("Timing loops on 3 functions... %u iterations each... in CLOCKS_PER_SEC %u\n", loops, (unsigned)CLOCKS_PER_SEC);
    for (int a = 0; array[a]; a++) 
    {   // perform each function
        unsigned t0 = clock();  // begin
        for (unsigned b = 0; b < loops; b++)
            array[a](rnd32());  // run function...
        unsigned t1 = clock();  // end
        unsigned diff = t1 - t0;
        double secs = (double)diff / (double)CLOCKS_PER_SEC;
        ss.str(""); // clear
        ss << (a + 1) << ": " << (t1 - t0) << " ticks ... " << secs << " secs - " << names[a];
        std::string s = ss.str();
        SPRTF("%s\n", s.c_str());
    }
    return 0;
}
// stringstream format tests
static void stringstream_tests()
{
    unsigned i = 123;
    std::stringstream ss;
    std::string s, s2, s3;
    ss << "iomanip std::setfill('0') << std::setw(5) '" << std::setfill('0') << std::setw(5) << i << "'";
    s = ss.str();
    SPRTF("%s\n", s.c_str());
    ss.str("");
    ss << "iomanip std::setfill(' ') << std::setw(5) '" << std::setfill(' ') << std::setw(5) << i << "'";
    s = ss.str();
    SPRTF("%s\n", s.c_str());

    s = dec2bin(i);
    s2 = Duoas_dec2bin(i);
    s3 = PanGalactic_dec2bin(i);
    SPRTF("dec2bin(123) = '%s', '%s', '%s'\n", s.c_str(), s2.c_str(), s3.c_str());

    i = 256;
    s = dec2bin(i);
    s2 = Duoas_dec2bin(i);
    s3 = PanGalactic_dec2bin(i);
    SPRTF("dec2bin(256) = '%s', '%s', '%s'\n", s.c_str(), s2.c_str(), s3.c_str());
    i = 128;
    s = dec2bin(i);
    s2 = Duoas_dec2bin(i);
    s3 = PanGalactic_dec2bin(i);
    SPRTF("dec2bin(128) = '%s', '%s', '%s'\n", s.c_str(), s2.c_str(), s3.c_str());
    i = -1;
    s = dec2bin(i);
    s2 = Duoas_dec2bin(i);
    s3 = PanGalactic_dec2bin(i);
    SPRTF("dec2bin(-1) = '%s', '%s', '%s'\n", s.c_str(), s2.c_str(), s3.c_str());

    time_test();
}


template <typename T>
  std::string NumberToString ( T Number )
  {
     std::ostringstream ss;
     ss << Number;
     return ss.str();
  }



/* Helper macros */

/* convert digit character to number, in any base */
#define ToNumber(c)	(isdigit(c) ? (c) - '0' : \
			 isupper(c) ? (c) - 'A' + 10 : \
			 islower(c) ? (c) - 'a' + 10 : \
			 -1		/* "invalid" flag */ \
			)
/* validate converted digit character for specific base */
#define valid(n, b)	((n) >= 0 && (n) < (b))

intmax_t
strtoimax(const char *nptr, char **endptr, int base)
{
	uintmax_t	accum;	/* accumulates converted value */
	int		n;	/* numeral from digit character */
	int		minus;	/* set iff minus sign seen */
	int		toobig;	/* set iff value overflows */

	if ( endptr != NULL )
		*endptr = (char *)nptr;	/* in case no conversion's performed */

	if ( base < 0 || base == 1 || base > 36 )
	{
		errno = EDOM;
		return 0;		/* unspecified behavior */
	}

	/* skip initial, possibly empty sequence of white-space characters */

	while ( isspace(*nptr) )
		++nptr;

	/* process subject sequence: */

	/* optional sign */
	if ( (minus = *nptr == '-') || *nptr == '+' )
		++nptr;

	if ( base == 0 ) {
		if ( *nptr == '0' ) {
			if ( nptr[1] == 'X' || nptr[1] == 'x' )
				base = 16;
			else
				base = 8;
		}
		else
				base = 10;
	}
	/* optional "0x" or "0X" for base 16 */

	if ( base == 16 && *nptr == '0' && (nptr[1] == 'X' || nptr[1] == 'x') )
		nptr += 2;		/* skip past this prefix */

	/* check whether there is at least one valid digit */

	n = ToNumber(*nptr);
	++nptr;

	if ( !valid(n, base) )
		return 0;		/* subject seq. not of expected form */

	accum = n;

	for ( toobig = 0; n = ToNumber(*nptr), valid(n, base); ++nptr )
		if ( accum > (uintmax_t)(INTMAX_MAX / base + 2) ) /* major wrap-around */
			toobig = 1;	/* but keep scanning */
		else
			accum = base * accum + n;

	if ( endptr != NULL )
		*endptr = (char *)nptr;	/* points to first not-valid-digit */

	if ( minus )
		{
		if ( accum > (uintmax_t)INTMAX_MAX + 1 )
			toobig = 1;
		}
	else
	if ( accum > (uintmax_t)INTMAX_MAX )
		toobig = 1;

	if ( toobig )
		{
		errno = ERANGE;
		return minus ? INTMAX_MIN : INTMAX_MAX;
		}
	else
		return (intmax_t)(minus ? -accum : accum);
}

//long long __attribute__ ((alias ("strtoimax")))
//strtoll (const char* __restrict__ nptr, char ** __restrict__ endptr, int base);


char *NumberToText(intmax_t i, char *b)
{
    char const digit[] = "0123456789";
    char* p = b;
    if( i < 0 )
    {
        *p++ = '-';
        i *= -1;
    }
    if (i < 0) {
        //strcpy(p," FAILED");
        //return b;
        i = INTMAX_MAX;
    }
    intmax_t shifter = i;

    do { //Move to where representation ends
        ++p;
        shifter = shifter/10;
    } while(shifter);

    *p = '\0';

    do { //Move back, inserting digits as u go
        *--p = digit[ i % 10 ];
        i = i / 10;
    } while(i);

    return b;
}

void test_strtoimax()
{
    int i, j;
    intmax_t res, res2;
    char *end, *end2;
    char buf[256];
    char buf2[256];
    char *stg1 = buf;
    char *stg2 = buf2;
    const char *bs = "-9999999999";
    std::string s, s2, s3, s4;

    SPRTF("\n%s: testing strtoimax function...\n", module );
    for (i = 0; i < 12; i++) {
        j = i;
        strcpy(stg1,bs);
        while (j--)
            strcat(stg1,"9");
        strcat(stg1,"S");
        res = strtoimax( stg1, &end, 10 );
        res2 = strtoll( stg1, &end2, 10 );
        //s = NumberToString<intmax_t>(res);
        //s2 = NumberToString<intmax_t>(res2);
        s = NumberToString(res);
        s2 = NumberToString(res2);
        s3 = NumberToText(res, stg2);
        s4 = NumberToText(res2, stg2);

        SPRTF("\nstrtoimax: %-25s,\ngot      : %-22lld, end %c\ngot      : %s\n", stg1, res, 
            (end && *end) ? *end : '0',
            s.c_str());
        SPRTF("NumtoText: %s\n", s3.c_str());
        sprintf(stg2,"%lld",(long long)res);
        SPRTF("sprintf  : %s\n", stg2);

        SPRTF("strtoll  : %-25s,\ngot      : %-22lld, end %c\ngot      : %s\n", stg1, res2, 
            (end2 && *end2) ? *end2 : '0',
            s2.c_str());
        SPRTF("NumtoText: %s\n", s4.c_str());
        sprintf(stg2,"%lld",(long long)res2);
        SPRTF("sprintf  : %s\n", stg2);
    }

    // stringstream format tests
    stringstream_tests();

    SPRTF("%s: end test strtoimax function...\n", module );

}

// eof
