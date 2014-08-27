// mmap1.cpp
// Testing creating a SHAREABLE memory block

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "winErr.hpp"

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[]  = TEXT("Message from first process.");
#define MX_MSG_BUF  264
TCHAR szMsgBuf[MX_MSG_BUF];

int _tmain()
{
    HANDLE hMapFile;
    LPCTSTR pBuf;
    DWORD err, res;
    LPTSTR lps = szMsgBuf;

    hMapFile = CreateFileMapping(
                 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // maximum object size (high-order DWORD)
                 BUF_SIZE,                // maximum object size (low-order DWORD)
                 szName);                 // name of mapping object

    if (hMapFile == NULL)
    {
        err = GetLastError();
        *lps = 0;
        res = GetLastErrorMsg(lps,MX_MSG_BUF,err);
        _tprintf(TEXT("Could not create file mapping object (%d) - %s\n"), err, lps);
        return 1;
   }
   pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        BUF_SIZE);

   if (pBuf == NULL)
   {
        err = GetLastError();
        *lps = 0;
        res = GetLastErrorMsg(lps,MX_MSG_BUF,err);
        _tprintf(TEXT("Could not map view of file (%d) - %s\n"), err, lps);

        CloseHandle(hMapFile);

        return 1;
   }


    CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

    _tprintf(TEXT("Time to run mmap2... Any char to exit... : "));
    _getch();

    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);

    return 0;
}

// eof
