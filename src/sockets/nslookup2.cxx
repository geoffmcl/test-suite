/**program description:
 *
 * this is a simple nslook up program.
 * You simple enter the domain name
 * and the program gives you the ip
 * addresses associated with this
 * domain name, e.g. insert google.com,
 * you will get different set of IP
 * addresses that represents google.com
 * Works under Windows OS.
 *
 * exercise: do input validation
 *
 * Copyright 2008 - 2009
 * 20151219: from : http://www.rohitab.com/discuss/topic/34006-simple-nslookup-for-beginners-c/
 *
 */

#include <sys/types.h>
#include <iostream>
#include <string>
#include <sstream>
#ifdef WIN32
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h> // inet_ntop, ...
#include <string.h> // for strlen, ...
#endif
#include <vector>
#include "../utils.hxx"

using namespace std;

typedef std::vector<std::string> vSTG;

static struct addrinfo hints;	 
static struct addrinfo *target;		   // holds full address information about the target domain
static int add_pause = 0;

void initiateWinsockFacility()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int error;

	wVersionRequested = MAKEWORD(2, 2);

	error = WSAStartup(wVersionRequested, &wsaData);

	if (error != 0) 
	{
		cout << "WSAStartup failed with error: " << error << endl;
		system("pause");
		exit(1);
	}
#endif
}

int getFullAddressInformation(const char *ipAddress)
{
	// output is sent to &target
    // Success returns zero. Failure returns a nonzero Windows Sockets error code
	int status = getaddrinfo(ipAddress, NULL, &hints, &target);
	
	if ( status != 0 )
	{
		cout << "getaddrinfo() error: " << gai_strerror(status) << endl;
#ifdef WIN32
		WSACleanup();
        if (add_pause)
            system("pause");
#endif
		return 1;
	}
    return 0;
}

/* ==================================================================

//
// IPv4 Socket address, Internet style
//

typedef struct sockaddr_in {

#if(_WIN32_WINNT < 0x0600)
    short   sin_family;    
#else //(_WIN32_WINNT < 0x0600)
    ADDRESS_FAMILY sin_family;
#endif //(_WIN32_WINNT < 0x0600)

    USHORT sin_port;
    IN_ADDR sin_addr;
    CHAR sin_zero[8];
} SOCKADDR_IN, *PSOCKADDR_IN;

//
// Structure used to store most addresses.
//
typedef struct sockaddr {

#if (_WIN32_WINNT < 0x0600)
    u_short sa_family;
#else 
    ADDRESS_FAMILY sa_family;           // Address family.
#endif //(_WIN32_WINNT < 0x0600)

    CHAR sa_data[14];                   // Up to 14 bytes of direct address.
} SOCKADDR, *PSOCKADDR, FAR *LPSOCKADDR;

//
//  Structure used in getaddrinfo() call
//

typedef struct addrinfo
{
    int                 ai_flags;       // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
    int                 ai_family;      // PF_xxx
    int                 ai_socktype;    // SOCK_xxx
    int                 ai_protocol;    // 0 or IPPROTO_xxx for IPv4 and IPv6
    size_t              ai_addrlen;     // Length of ai_addr
    char *              ai_canonname;   // Canonical name for nodename
    __field_bcount(ai_addrlen) struct sockaddr *   ai_addr;        // Binary address
    struct addrinfo *   ai_next;        // Next structure in linked list
}
ADDRINFOA, *PADDRINFOA;

const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt)
{
    if (af == AF_INET) {
        struct sockaddr_in in;
        memset(&in, 0, sizeof(in));
        in.sin_family = AF_INET;
        memcpy(&in.sin_addr, src, sizeof(struct in_addr));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    } else if (af == AF_INET6) {
        struct sockaddr_in6 in;
        memset(&in, 0, sizeof(in));
        in.sin6_family = AF_INET6;
        memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
        getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
        return dst;
    }
    return NULL;
}

int inet_pton(int af, const char *src, void *dst)
{
    struct addrinfo hints, *res, *ressave;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = af;
    if (getaddrinfo(src, NULL, &hints, &res) != 0) {
        // dolog(LOG_ERR, "Couldn't resolve host %s\n", src);
        return -1;
    }
    ressave = res;
    while (res) {
        memcpy(dst, res->ai_addr, res->ai_addrlen);
        res = res->ai_next;
    }

    freeaddrinfo(ressave);
    return 0;
}


   ==================================================================== */
typedef std::vector<struct sockaddr> vSA;

// int getnameinfo(const struct sockaddr *restrict sa, socklen_t salen,
//       char *restrict node, socklen_t nodelen, char *restrict service,
//       socklen_t servicelen, int flags);
#define MX_NAME_BUF 2048
static char node[MX_NAME_BUF];
static char service[MX_NAME_BUF];
int getDomainName( const struct sockaddr *psa, socklen_t len, int flags, int verb = 0 );

int getDomainName( const struct sockaddr *psa, socklen_t len, int flags, int verb )
{
    node[0] = 0;
    service[0] = 0;
    int res = getnameinfo(psa, len, node, MX_NAME_BUF, service, MX_NAME_BUF, flags );
    if (res) {
        if (verb) {
#ifdef WIN32
            res = WSAGetLastError();
	    	cout << "getaddrinfo() error: " << gai_strerror(res) << endl;
#endif	    	
        }
    } else if (verb) {
        cout << "Node: " << node << ", Serv: " << service << endl;
    }
    return res;
}


string nslookup(struct addrinfo *fetchIPsFromTarget)
{
	char printableIP[INET6_ADDRSTRLEN];

	struct sockaddr_in *ipv4 = (struct sockaddr_in *)fetchIPsFromTarget->ai_addr;
	void *ipAddress = &(ipv4->sin_addr);
    int res = getDomainName( (struct sockaddr *)ipv4, sizeof(struct sockaddr), 0 );
	inet_ntop(ipv4->sin_family, ipAddress, printableIP, sizeof(printableIP));
	string ip(printableIP, strlen(printableIP));
    if (res == 0) {
        ip += " (";
        ip += node;
        ip += ")";
    }
	return ip;
}

int main(int argc, char **argv)
{
    int iret = 0;
    char *elap;

	string targetDomain;
    if (argc < 2) {
    	cout << "Please enter a domain name: ";
	    cin >> targetDomain;
    } else {
        targetDomain = argv[1]; // assume a domain name
    }

    size_t pos = targetDomain.find('.');
    if (pos == string::npos) {
    	cout << "Domain name must contain at least one point, like abc.com!"  << endl;
        return 1;
    }

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

    vSTG ips;

    initiateWinsockFacility();
    double begin = get_seconds(); // add timing

	iret = getFullAddressInformation(targetDomain.c_str());
    if (iret) {
        elap = get_elapsed_stg(begin);
        cout << "Elapsed: " << elap << " secs..." << endl;
        return iret;
    }

	//get all ip's associated with the target domain
	struct addrinfo *grabAllTargetIPs;
    string fetchedIP;
	for(grabAllTargetIPs = target; grabAllTargetIPs != NULL; grabAllTargetIPs = grabAllTargetIPs->ai_next)
	{
		fetchedIP = nslookup(grabAllTargetIPs);
		// cout << fetchedIP << endl;
		ips.push_back(fetchedIP);
	}

    elap = get_elapsed_stg(begin);
	freeaddrinfo(target);
#ifdef WIN32
	WSACleanup();
#endif
    size_t ii, max = ips.size();
    cout << "Got " << max << " ip" << ((max == 1) ? "" : "s") << " ..." << endl;
    for (ii = 0; ii < max; ii++) {
        fetchedIP = ips[ii];
		cout << fetchedIP << endl;
    }

    cout << "Elapsed: " << elap << " secs..." << endl;
#ifdef WIN32
    if (add_pause) {
    	system("pause");
    }
#endif
	return 0;
}

/* eof */
