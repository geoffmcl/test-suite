test suite
----------

**This only contains 'test' code.**

When ever I need to test a functions, I frequently use this test suite. It seems 
simpler sometimes to write, debug and test it in this simple 'do nothing' app than 
in whatever project I am working on at the time.

It is mainly C, but mostly cast into a cxx module just so I do not have to take 
care when and where I create variables...

It is a cmake project so the 'build' is simple

> cd build  
> build-me  

As can be seen in this build-me.bat file the process is

> cmake ..  
> cmake --build . --config debug  
> cmake --build . --config release  

There is no install of this 'test' app... In fact most of the time I just run 
the app in the MSVC IDE Debugger...

Most tests are specifically turned on/off by modifying the test.cxx module, the OS 
entry point... some do depend on finding various 3rdParty dependencies, but the 
idea it should compile and run whether these are found or not...

There is also a build.x64 to build a window 64-bit application if you have the 
appropriate MSVC install.

While most 'tests' are in Windows, now this is in a repo I may do some work to 
get those test that are NOT specifically using the Windows API running in linux.

NONE of the tests are 'documented'. That only comes from reading the code.

Have fun ;=))

Geoff.
20140827
