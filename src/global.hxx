// file : global.hxx
#ifndef _GLOBAL_HXX_
#define _GLOBAL_HXX_

class global {
public:
	global();
	~global();
	int var;
	int foo();

};

extern global *pg;

#endif // _GLOBAL_HXX_
// eof
