// file : global.cxx

#include "global.hxx"

global *pg = 0;

global::global() {
	var = 0;
}

global::~global() {

}

int global::foo() {
    return 5;
}

// eof - global.cxx
