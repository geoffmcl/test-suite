/*\
 * test-ofstream.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <fstream>  // for ofstream
#include <iostream> // for cout,...
#include <string>
#include "sprtf.hxx"
#include "test-ofstream.hxx"

using namespace std;

static const char *module = "test-ofstream";

// from : http://stackoverflow.com/questions/9459136/how-can-i-make-an-ostream-reference-an-ofstream-c
class Log {
private:
    std::ostream* os;
    bool dynamic;
public:
    Log(std::ostream& os = std::cout): os(&os), dynamic(false) { }
    Log(std::string filename) : dynamic(true) {
#ifdef WIN32
        std::ofstream* ofs = new std::ofstream(filename);
#else
        std::ofstream* ofs = new std::ofstream(filename.c_str());
#endif
        if (!ofs->is_open()) {
            // do errorry things and deallocate ofs if necessary
            delete ofs;
        } else {
            os = ofs;
        }
    }

    ~Log() { if (dynamic) delete os; }
};

// this creates a WARNING
// warning C4355: 'this' : used in base member initializer list
#if 0 // 00000000000000000000000000000000000000000000000000000
class Log2
{
    std::ofstream byname;
    std::ostream& os;
public:
    Log2(std::ostream& stream = std::cout) : byname(), os(stream) { }
    Log2(std::string filename) : byname(filename), os(this->byname)
    {
        if (!os) {
            // handle errors
        }
    }
};
#endif // 0000000000000000000000000000000000000000000000000000

void set_cout_to_file( std::string outfile )
{
    std::ofstream file(outfile.c_str(),ios::out);
    if (file.is_open()) {
        std::cout.rdbuf(file.rdbuf());
        std::cout << "this goes to the file" << std::endl;
        SPRTF("%s: Sent a string to cout. Should be in file %s\n", module, outfile.c_str());
        std::cout.rdbuf(std::cout.rdbuf());
        SPRTF("%s: Returned cout to cout...\n", module);
        std::cout << "this sould go to the console" << std::endl;
        file.close();
    } else {
        SPRTF("%s: Failed to open file %s!\n", module, outfile.c_str());
    }
}

void test_ofstream()
{
    std::string m_File("tempofs.txt");
    SPRTF("%s - testing ofstream...\n", module);
    ofstream m_LogFile;
    if (m_LogFile.is_open()) {
        SPRTF("%s: m_LogFile.is_open() is aleady OPEN!?\n", module );
    } else {
        SPRTF("%s: m_LogFile.is_open() is not open yet.\n", module );
    }
#if defined(_MSC_VER) && (_MSC_VER < 1400)
    if (m_LogFile != cerr) {
        SPRTF("%s: m_LogFile is not cerr.\n", module );
    }
#endif // _MSC_VER 
    set_cout_to_file(m_File);
    m_LogFile.open( m_File.c_str(), ios::out|ios::app );
    if (m_LogFile.is_open()) {
        SPRTF("%s: Open file %s for appending...\n", module, m_File.c_str());
        m_LogFile << "This should be appended" << endl;
        m_LogFile.close();
    } else {
        SPRTF("%s: Failed to open file %s\n", module, m_File.c_str());
    }
    SPRTF("%s: end test_ofstream...\n", module );

}

// eof = test-ofstream.cxx
