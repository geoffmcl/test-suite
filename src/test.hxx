// test.hxx
#ifndef _TEST_HXX_
#define _TEST_HXX_

#ifdef __cplusplus
extern "C" {
#endif

extern int verbosity;

#define VERB1 (verbosity >= 1)
#define VERB2 (verbosity >= 2)
#define VERB5 (verbosity >= 5)
#define VERB9 (verbosity >= 9)

#ifdef __cplusplus
}
#endif

#endif // #ifndef _TEST_HXX_
// eof - test.hxx

