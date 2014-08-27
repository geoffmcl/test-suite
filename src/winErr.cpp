
#include <Windows.h>
#include "winErr.hpp"

DWORD GetLastErrorMsg( LPTSTR lpm, DWORD dwLen, DWORD dwErr )
{
   PVOID lpMsgBuf = 0;
   DWORD    dwr;

   dwr = FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErr,   //	GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
		0,
		NULL );
   
   //dwr = strlen(lpMsgBuf);
   if( ( dwr == 0 ) || ( dwr >= dwLen ) )
      dwr = (DWORD)-1;
   else
      strcat(lpm, (LPTSTR)lpMsgBuf);

   //	printf("%s:%s\n",lpm,(LPCTSTR)lpMsgBuf);
   // Free the buffer.
   if( lpMsgBuf )
      LocalFree( lpMsgBuf );

   return dwr;
}
