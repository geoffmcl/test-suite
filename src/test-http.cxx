/*\
 * test-http.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <algorithm>
#ifndef _MSC_VER
#include <string.h> // for strcmp(), ...
#endif
#include <stdio.h>
#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif
#ifdef HAVE_CURL_LIB
#include <curl/curl.h>
#endif // HAVE_CURL_LIB
#include "sprtf.hxx"
#include "test-trim.hxx"    // for remove_white_space(line)
#include "test-http.hxx"

static const char *module = "test-http";

// Define to get extra output on CURL http GET test
// #define EXTRA_DEBUG


#ifndef EOL
#define EOL std::endl;
#endif

#ifndef MX_BUF_SIZE
#define MX_BUF_SIZE 10000
#endif
static char recv_buffer[MX_BUF_SIZE+2];


#ifndef DEF_HTTP_URL
#define DEF_HTTP_URL "www.google.com"
#endif
#ifndef DEF_HTTP_PORT
#define DEF_HTTP_PORT 4567
#endif
#ifndef DEF_CONF_FILE
#define DEF_CONF_FILE "http.conf"
#endif

static const char *host_url  = DEF_HTTP_URL;
static const char *http_conf = DEF_CONF_FILE;
static int http_port         = DEF_HTTP_PORT;

#define  it_None        0     // end of list
#define  it_Version     1
#define  it_String      2
#define  it_Int         3
#define  it_Bool        4
#define  it_WinSize     5     // special WINDOWPLACEMENT
#define  it_Rect        6
#define  it_Color       7

#ifndef uint
typedef unsigned int uint;
#endif

typedef struct	tagINILST {	/* i */
	const char *i_Item;
	uint	i_Type;
	void *  i_Void;
	uint    i_Res1;
} INILST, * PINILST;

const char *szPort = "port";
const char *szUrl  = "url";

INILST sIniLst[] = {
    { szPort, it_Int, &http_port, 0 },
    { szUrl,  it_String, &host_url, 0 },
   // last entry
   { 0,  it_None, 0, 0 }
};

void strip_string_quotes(std::string &val)
{
    if (val.size() < 3)
        return;
    if (val[0] == '"')
        val = val.substr(1);
    if (val[val.size()-1] == '"')
        val = val.substr(0,val.size()-1);
}

int set_conf_item( std::string &item, std::string &val )
{
    int typ;
    PINILST pini = sIniLst;
    int *pi;
    const char **cp;
    while (pini->i_Item) {
        if (strcmp(pini->i_Item,item.c_str()) == 0) {
            typ = pini->i_Type;
            switch (typ) {
            case it_Int:
                pi = (int *)pini->i_Void;
                *pi = atoi(val.c_str());
                break;
            case it_String:
                cp = (const char **)pini->i_Void;
                strip_string_quotes(val);
                *cp = strdup(val.c_str());
                break;
            default:
                SPRTF("%s: Type value %d NOT YET IMPLEMENTED! *** FIX ME ***\n", module,typ);
                exit(1);
            }
            return 0;
        }
        pini++;
    }
    SPRTF("%s: Unknown pair '%s=%s'! Ignoring...\n", module, item.c_str(), val.c_str());
    SPRTF("Maybe this item needs to be added to the INILST table?\n");
    return 1;
}

void read_conf_ini_file()
{
    std::ifstream ifs;
    std::string line, oline;
    vSTG vstg;
    size_t off;
    char *env = getenv("HTTP_CONF");
    if (env) {
        http_conf = strdup(env);
    }
    ifs.open(http_conf, std::ifstream::in);
    // these are NOT valid ??? ifs.bad() || ifs.eof()
    if (!ifs.good()) {
        SPRTF("%s: Failed to open '%s'\n", module, http_conf);
        return;
    }
    while (std::getline(ifs,oline)) {
        line = oline;
        off = line.find("#");
        if (off != std::string::npos) {
            line = line.substr(0,off);
        }
        remove_white_space(line);
        if (line.empty())
            continue;
        if (line[0] == '#')
            continue;
        vstg = string_split(line,"=");
        if (vstg.size() != 2) {
            SPRTF("%s: BAD config line! Did not split into 2 on '='. Got %d.\n", module, (int)vstg.size());
            SPRTF("Ignoring '%s'\n", oline.c_str());
            continue;
        }
        set_conf_item( vstg[0], vstg[1] );
    }
}


int process_data( char *cp, size_t nDataLength, int header )
{
    int iret = 0;
    int res, len;
    char *bgn;
    size_t i;
    bgn = cp;
    len = 0;
    for (i = 0; i < nDataLength; i++) {
        res = cp[i];
        if ((res == '\n')||(res == '\r')) {
            cp[i] = 0;
            if (len < 1000) {
                SPRTF("%s\n", bgn);
            } else {
                SPRTF("Received %d bytes\n", len );
            }
            cp[i] = (char)res;
            i++;    // bump to next char
            len = 1;    // had 1 of /r or /n
            bgn = &cp[i];
            for (; i < nDataLength; i++) {
                res = cp[i];
                if (res > ' ') {
                    bgn = &cp[i];
                    break;
                } else if ((res == '\n')||(res == '\r')) {
                    len++;
                }
            }
            if ((len >= 4)&& !header) {
                SPRTF("\nHad http GET header...\n\n");
                header = 1;
                iret = 1;
            }
            len = 0;
        }
        len++;
    }
    return iret;
}

#ifdef HAVE_CURL_LIB
////////////////////////////////////////////////////////////////////////////////////
static int had_curl_hdr = 0;
static bool done_hdr_msg = false;
size_t header_callback(char *ptr,   size_t size,   size_t nitems,   void *userdata)
{
    size_t len = (size * nitems);
#ifdef EXTRA_DEBUG
    SPRTF("%s: CURL header callback with len %d\n", module, (int)len);
#endif
    if ((len == 2) && (ptr[0] == '\r') && (ptr[1] == '\n')) {
        had_curl_hdr = 1;
        SPRTF("\nHad http GET header...\n\n");
    } else {
        process_data( ptr, len, had_curl_hdr);
    }
    return len;
}

size_t write_callback(char *ptr, size_t size, size_t nitems, void *userdata)
{
    size_t len = (size * nitems);
#ifdef EXTRA_DEBUG
    SPRTF("%s: CURL data callback with len %d\n", module, (int)len);
#endif
    process_data( ptr, len, 0);
    return len;
}
 

// CURLcode curl_easy_setopt(CURL *handle, CURLOPT_WRITEFUNCTION, write_callback);
void curl_fetch()
{
    CURL* c;
    c = curl_easy_init();
    curl_easy_setopt( c, CURLOPT_URL, host_url );
    curl_easy_setopt( c, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt( c, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_perform( c );
    curl_easy_cleanup( c );
}
///////////////////////////////////////////////////////////////////////////////
#endif // HAVE_CURL_LIB


#ifdef WIN32
// using namespace std;
int net_init() 
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed. " << EOL;
        return 1;
    }
    return 0;
}

const char *get_req = 
    "GET / HTTP/1.1\r\n"
    "Host: www.google.com\r\n"
    "Connection: close\r\n"
    "\r\n";

int win_fetch()
{
    int res, nDataLength, had_header;
    char *cp = recv_buffer;
    DWORD err;
    if (net_init())
        return 1;

    SOCKET Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (Socket == INVALID_SOCKET) {
        err =  WSAGetLastError();
        std::cerr << "Failed to create socket! (" << err << ")" << EOL;
        WSACleanup();
        return 1;
    }
    struct hostent *host;
    host = gethostbyname(host_url);
    if (!host) {
        err =  WSAGetLastError();
        std::cerr << "gethostbyname(" << host_url << ") failed! (" << err << ")" << EOL;
        closesocket(Socket);
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN SockAddr;
    SockAddr.sin_port        = htons(80);
    SockAddr.sin_family      = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    std::cout << "Connecting..." << EOL;

    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
        err =  WSAGetLastError();
        std::cerr << "Could not connect! (" << err << ")" << EOL;
        closesocket(Socket);
        WSACleanup();
        return 1;
    }
    std::cout << "Connected..." << EOL;
    res = send(Socket,get_req,strlen(get_req),0);
    if (res == SOCKET_ERROR) {
        err =  WSAGetLastError();
        std::cerr << "Send of GET failed! (" << err << ")" << EOL;
        closesocket(Socket);
        WSACleanup();
        return 1;
    }

    had_header = 0;
    while ((nDataLength = recv(Socket,cp,MX_BUF_SIZE,0)) > 0) {
        //cp[nDataLength] = 0;
        res = process_data( cp, nDataLength, had_header );
        if (res && !had_header) {
            had_header = 1;
        }
    }

    closesocket(Socket);
    WSACleanup();
    return 0;
}

#endif // WIN32



void test_http()
{
#if !defined(HAVE_CURL_LIB) && !defined(WIN32)
/////////////////////////////////////////////////////////////////////////////////////
    SPRTF("%s: Not WIN32, and NO CURL lib found, so not tests here!\n", module );
/////////////////////////////////////////////////////////////////////////////////////
#else
/////////////////////////////////////////////////////////////////////////////////////

    // we have 1 or both these tests
    read_conf_ini_file();

#ifdef HAVE_CURL_LIB
    SPRTF("\n%s: Doing a GET from %s using CURL...\n", module, host_url);
    curl_fetch();
#endif

#ifdef WIN32
    SPRTF("\n%s: Doing a GET from %s using WinSock2...\n", module, host_url);
    int res = win_fetch();
#endif

/////////////////////////////////////////////////////////////////////////////////////
#endif // got some tests y/n

}

// eof = test-http.cxx
