/*\
 * test-secs.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <string.h>
#include "utils.hxx"
#include "sprtf.hxx"
#include "test-secs.hxx"

static const char *module = "test-secs";

/* -------------------------------------------------------------------
   Assumes the buffer length is LARGER than the string length + 1
   ------------------------------------------------------------------- */
char *increment_string_in_buffer2(char *s)
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
        /* ASSUME BUFFER IS LARGE ENOUGH
           *****************************
		if (!begin) {
            s = (char *)realloc(s, len + 2);
            if (!s) {
                printf("realloc(%d) FAILED!\n", len + 2 );
                exit(1); // memory realloc failure
            }
        }
          ============================= */
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


void trim_secs_buffer( char *cp )
{
    size_t ii, len = strlen(cp);
    while (len) {
        len--;
        if (cp[len] == '.') {
            strcat(cp,"0");
            break;
        } else if (cp[len] > '0') {
            break;
        }
        cp[len] = 0;
    }
    if (len) {
        int c;
        size_t zi;
        for (ii = 0; ii < len; ii++) {
            if (cp[ii] == '.') {
                zi = ii;
                ii++;
                int digs = 0;
                int zeros = 0;
                for (; ii < len; ii++) {
                    c = cp[ii];
                    if (c == '0')
                        zeros++;
                    else
                        digs++;
                    if (digs > 3) {
                        c = cp[ii+1];
                        cp[ii+1] = 0;
                        if (c == '0') {
                            cp[ii] = 0;
                            ii--;
                            while (ii > zi) {
                                if (cp[ii] > '0')
                                    break;
                                cp[ii] = 0;
                                ii--;
                            }
                        } else if (c >= '5') {
                            while (ii > zi) {
                                c = cp[ii];
                                if (c == '9') {
                                    cp[ii] = 0;
                                } else {
                                    c++;
                                    cp[ii] = c;
                                    c = 0;
                                    break;
                                }
                                ii--;
                            }
                            if ((c == '9') && (ii == zi)) {
                                // must increment the value before the decimal
                                cp[ii] = 0; // lose the decimal
                                increment_string_in_buffer2(cp);
                            }
                        }
                        ii = len;
                        break;
                    } else if (digs && (zeros >= 3) && (c == '0')) {
                        // had some digits after decimal, and 3 or more zeros, and this is one of them
                        cp[ii] = 0; // stop it here
                        ii--;
                        while (ii > zi) {
                            if (cp[ii] > '0')
                                break;
                            cp[ii] = 0;
                            ii--;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
}

char *seconds_to_hhmmss_stg(double secs)
{
    char *cp = GetNxtBuf();
    int i;
    if (secs < 1.0) {
        // got for near max, but clean it up a bit
        sprintf(cp,"%.15lf", secs );
        size_t len = strlen(cp);
        while (len) {
            len--;
            if (cp[len] == '.') {
                strcat(cp,"0");
                break;
            } else if (cp[len] > '0')
                break;
            cp[len] = 0;
        }
        strcat(cp," secs");
        return cp;
    }
    if (secs < 60.0) {
        // just show 2 decimal places
        i = (int)((secs + 0.005) * 100.0);
        sprintf(cp,"%.2lf secs", ((double)i / 100.0));
        return cp;
    }
    // now have minutes, no decimal on seconds
    int mins = (int)(secs / 60.0);
    secs -= (double)(mins * 60);
    i = (int)(secs + 0.5);
    if (mins < 60.0) {
        secs -= (mins * 60.0);
        sprintf(cp, "%d:%02d mm:ss", mins, i);
        return cp;
    }
    int hour = (int)(mins / 60.0);
    mins -= hour * 60;
    sprintf(cp,"%d:%02d:%02d", hour, mins, i);
    return cp;
}

void test_trim_secs()
{
    const char *s1 = "0.189000129699707";
    const char *s2 = "0.352999925613403";
    const char *s3 = "0.525000095367432";
    const char *s4 = "0.700000047683716";
    const char *s5 = "0.909090947683716";
    const char *s6 = "0.909099947683716";
    const char *s7 = "9.999999947683716";
    char *cp = GetNxtBuf();
    strcpy(cp,s5);
    trim_secs_buffer(cp);
    SPRTF("From '%s' to '%s'\n", s5, cp);
    strcpy(cp,s6);
    trim_secs_buffer(cp);
    SPRTF("From '%s' to '%s'\n", s6, cp);
    strcpy(cp,s7);
    trim_secs_buffer(cp);
    SPRTF("From '%s' to '%s'\n", s7, cp);
    strcpy(cp,s2);
    trim_secs_buffer(cp);
    SPRTF("From '%s' to '%s'\n", s2, cp);
    strcpy(cp,s3);
    trim_secs_buffer(cp);
    SPRTF("From '%s' to '%s'\n", s3, cp);
    strcpy(cp,s4);
    trim_secs_buffer(cp);
    SPRTF("From '%s' to '%s'\n", s4, cp);
    strcpy(cp,s1);
    trim_secs_buffer(cp);
    SPRTF("From '%s' to '%s'\n", s1, cp);

}

void test_secs()
{
    //const char *s1 = "0.247109359793416";
    SPRTF("\n%s: Doing seconds output...\n", module);
    double secs = 0.00000001;
    test_trim_secs();
    //for ( ; secs < 1; secs += 0.0000001) {
    //    SPRTF("Secs %lf in hh:mm:ss %s\n", secs, seconds_to_hhmmss_stg(secs));
    //}
    for ( ; secs < 4000; secs += 30.0) {
        SPRTF("Secs %lf in hh:mm:ss %s\n", secs, seconds_to_hhmmss_stg(secs));
    }

    SPRTF("%s: Done seconds test...\n", module);
}
// eof = test-secs.cxx
