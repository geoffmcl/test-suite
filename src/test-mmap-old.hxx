/*\
 * test-mmap.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _TEST_MMAP_HXX_
#define _TEST_MMAP_HXX_
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#ifdef _MSC_VER
#include <Windows.h>
#include <stdint.h>
#include "byid.hpp"
#include "winErr.hpp"
#include "sprtf.hxx"
#include "utils.hxx"
#else
#include <sys/mman.h>
#include <unistd.h>
#include <osmium/storage/byid.hpp>
#endif


#ifdef _MSC_VER
static void showlasterr(const char *msg)
{
    TCHAR emsg[264];
    DWORD res, err = GetLastError();
    LPTSTR lps = emsg;
    *lps = 0;
    res = GetLastErrorMsg( lps, 256, err ); 
    if (res != -1) {
        SPRTF("Error: %s (%d) - %s\n", msg, err, lps );
    } else {
        SPRTF("Error: %s (%d)\n", msg, err );
    }
}
static std::string gettempfilename()
{
    char szTmpFile[256];
    char szMMName[256];
    /* Create temporary file for mapping. */
    if (!GetTempPath (256, szTmpFile)) {
        showlasterr("GetTempPath FAILED!");
        throw std::bad_alloc();
    }
    if (!GetTempFileName (szTmpFile,"MM",0,szMMName )) {
        showlasterr("GetTempFileName FAILED!");
        throw std::bad_alloc();
    }
    std::string file = szMMName;
    SPRTF("TEMP file is '%s'\n", file.c_str());
    return file;
}
static DWORD getpagesize()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SPRTF("SystemInfo PageSize = %u\n", si.dwPageSize);
    return si.dwPageSize;
}
static DWORD getgranuality()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SPRTF("SystemInfo Allocation Granularity = %u\n", si.dwAllocationGranularity);
    return si.dwAllocationGranularity;
}
#endif // _MSC_VER

namespace Osmium {

    namespace Storage {

        namespace ById {

            /**
            * MmapFile stores data in files using the mmap() system call.
            * It will grow automatically.
            *
            * If you have enough memory it is preferred to use the in-memory
            * version MmapAnon. If you don't have enough memory or want the
            * data to persist, use this version. Note that in any case you need
            * substantial amounts of memory for this to work efficiently.
            */
            template <typename TValue>
            class MmapFile : public Osmium::Storage::ById::Base<TValue> {

            public:

                static const uint64_t size_increment = 10 * 1024 * 1024;

                /**
                * Create mapping backed by file. If filename is empty, a temporary
                * file will be created.
                *
                * @param filename The filename (including the path) for the storage.
                * @param remove Should the file be removed after use?
                * @exception std::bad_alloc Thrown when there is not enough memory or some other problem.
                */
                MmapFile(const std::string& filename="", bool remove=true) :
                    Base<TValue>(),
                    m_size(1) 
#ifdef _MSC_VER
                    , m_fd(0), hMap(0), m_items(0)
#endif
                    {
#ifdef _MSC_VER
                    file = filename;
                    is_temp = false;
                    ps = getpagesize();
                    gran = getgranuality(); // only get this ONCE
#endif

                    if (filename == "") {
#ifdef _MSC_VER
                        is_temp = true;
                        file = gettempfilename();
                        //char *temp = getenv("TEMP");
                        //char *tn = _tempnam(temp,"tempmap");
                        //if (!tn) {
                        //    throw std::bad_alloc();
                        //}
                        //file = tn;
                    } 
                    {
#else
                        FILE* file = tmpfile();
                        if (!file) {
                            throw std::bad_alloc();
                        }
                        m_fd = (HANDLE)fileno(file);
                        m_fd = fileno(file);
                    } else {
#endif
#ifdef _MSC_VER
                        DWORD createDisp = CREATE_ALWAYS;
                        DWORD accessMode = GENERIC_READ|GENERIC_WRITE;
                        DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
                        DWORD winHint = FILE_FLAG_RANDOM_ACCESS;
                        if ( is_file_or_directory64(file.c_str()) == DT_FILE) {
                            // createDisp = OPEN_EXISTING; // this FAILED, so try
                            createDisp = TRUNCATE_EXISTING;
                        }
                        m_fd = CreateFile(file.c_str(), 
                            accessMode, shareMode, NULL, createDisp, winHint, NULL);
                            //GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, winHint, NULL);
#else
                        m_fd = open(filename.c_str(), O_RDWR | O_CREAT, 0600);
#endif
                    }
#ifdef _MSC_VER
                    if (m_fd == INVALID_HANDLE_VALUE) {
                        showlasterr("CreateFile FAILED!");
                        clear();
                        throw std::bad_alloc();
                    }
                    uint64_t sz = (sizeof(TValue) * m_size);
                    distToMoveLo = (sz & 0xffffffff);
                    distToMoveHi = (sz >> 8);
#else
                    if (m_fd < 0) {
                        throw std::bad_alloc();
                    }

                    // now that the file is open we can immediately remove it
                    // (temporary files are always removed)
                    if (remove && filename != "") {
                        if (unlink(filename.c_str()) < 0) {
                            // XXX what to do here?
                        }
                    }
#endif

                    // make sure the file is at least as large as the initial size
                    if (get_file_size() < sizeof(TValue) * m_size) {
#ifdef _MSC_VER
                        // TODO: truncate
                        // SetFilePoiner or SetFilePoinerEx to set the current position to the size you want, 
                        // then call SetEndOfFile.
                        //  INVALID_SET_FILE_POINTER
                        //if (SetFilePointer(m_fd, distToMoveLo, &distToMoveHi, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
                        //    showlasterr("SetFilePointer FAILED!");
                        //    throw std::bad_alloc();
                        //}
                        LARGE_INTEGER li, nli;
                        li.QuadPart = sz;
                        if (!SetFilePointerEx( m_fd, li, &nli, FILE_BEGIN )) {
                            showlasterr("SetFilePointer FAILED!");
                            clear();
                            throw std::bad_alloc();
                        }
                        if ( ! SetEndOfFile(m_fd) ) {
                            showlasterr("SetEndOfFile FAILED!");
                            clear();
                            throw std::bad_alloc();
                        }
#else
                        if (ftruncate(m_fd, sizeof(TValue) * m_size) < 0) {
                            throw std::bad_alloc();
                        }
#endif
                    }
#ifdef _MSC_VER
                    DWORD dwProtect = PAGE_READWRITE;
                    hMap = CreateFileMapping(
                        m_fd,           // _In_      HANDLE hFile,
                        NULL,           // _In_opt_  LPSECURITY_ATTRIBUTES lpAttributes,
                        dwProtect,      // _In_      DWORD flProtect,
                        distToMoveHi,   // _In_      DWORD dwMaximumSizeHigh,
                        distToMoveLo,   // _In_      DWORD dwMaximumSizeLow,
                        NULL );         // _In_opt_  LPCTSTR lpName
                    if (hMap == NULL) {
                        showlasterr("CreateFileMapping FAILED!");
                        clear();
                        throw std::bad_alloc();
                    }
                    /* A handle to a file mapping object. The CreateFileMapping and OpenFileMapping functions return this handle. */
                    /* got ERROR: (1132) - The base address or the file offset specified does not have the proper alignment. */
                    m_items = static_cast<TValue*>(MapViewOfFileEx(
                        hMap,                   // HANDLE hFileMappingObject,
                        FILE_MAP_ALL_ACCESS,    // DWORD dwDesiredAccess,
                        0,                      // DWORD dwFileOffsetHigh,
                        0,                      // DWORD dwFileOffsetLow,
                        0,                      // SIZE_T dwNumberOfBytesToMap,
                        NULL ));                 // PVOID pvBaseAddress
                    //m_items = static_cast<TValue*>(MapViewOfFile(hMap,FILE_MAP_ALL_ACCESS,0,0,0));

                    if (m_items == NULL) {
                        showlasterr("MapViewOfFileEx FAILED!");
                        clear();
                        throw std::bad_alloc();
                    }

#else
                    m_items = static_cast<TValue*>(mmap(NULL, sizeof(TValue) * m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
                    if (m_items == MAP_FAILED) {
                        throw std::bad_alloc();
                    }
#endif
                }

                ~MmapFile() {
                    clear();
                }

                void set(const uint64_t id, const TValue value) {
                    if (id >= m_size) {
                        uint64_t new_siz = id + size_increment;
                        uint64_t offset = sizeof(TValue) * id;  // this is where this value will be added
                        // if the file backing this mmap is smaller than needed, increase its size
                        if (get_file_size() < sizeof(TValue) * new_siz) {
#ifdef _MSC_VER
                            // TODO: ftruncate...
                            uint64_t file_size = sizeof(TValue) * new_siz;
                            SPRTF("set: For offset %s, increase file size to %s\n", get_nice_number64u(offset),
                                get_nice_number64u(file_size) );
                            UnmapViewOfFile(m_items);
                            m_items = 0;
                            CloseHandle(hMap);
                            hMap = 0;

                            LARGE_INTEGER li, nli;
                            li.QuadPart = file_size;
                            if (!SetFilePointerEx( m_fd, li, &nli, FILE_BEGIN )) {
                                showlasterr("SetFilePointer FAILED!");
                                throw std::bad_alloc();
                            }
                            //LONG distToMoveLo = (new_size & 0xffffffff);
                            //LONG distToMoveHi = (new_size >> 8);
                            //if (SetFilePointer(m_fd, distToMoveLo, &distToMoveHi, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
                            //    showlasterr("SetFilePointer FAILED!");
                            //    throw std::bad_alloc();
                            //}
                            if ( ! SetEndOfFile(m_fd) ) {
                                showlasterr("SetEndOfFile FAILED!");
                                clear();
                                throw std::bad_alloc();
                            }
                            uint64_t curr_size = get_file_size();
                            if (file_size != curr_size) {
                                SPRTF("Warning: Failed to exactly set new file size!\n");
                            }
                            /* must be an integer multiple of the allocation granularity of the operating system for the 
                               Windows operating system, fixed allocation granularity for 64KB. */
                            DWORD dwProtect = PAGE_READWRITE;
                            hMap = CreateFileMapping(
                                m_fd,           // _In_      HANDLE hFile,
                                NULL,           // _In_opt_  LPSECURITY_ATTRIBUTES lpAttributes,
                                dwProtect,      // _In_      DWORD flProtect,
                                0, //distToMoveHi,   // _In_      DWORD dwMaximumSizeHigh,
                                0, //distToMoveLo,   // _In_      DWORD dwMaximumSizeLow,
                                NULL );         // _In_opt_  LPCTSTR lpName
                            if (hMap == NULL) {
                                showlasterr("CreateFileMapping FAILED!");
                                clear();
                                throw std::bad_alloc();
                            }
                            /* need a map view where this id is valid 
                                the offset must be a multiple of the allocation granularity. */
                            uint64_t noff = 0;
                            uint64_t nxtoff = 0;
                            while (nxtoff < offset) {
                                noff = nxtoff;
                                nxtoff += (uint64_t)gran;
                            }
                            dwFileOffsetLow  = (noff & 0xffffffff);
                            dwFileOffsetHigh = (noff >> 8);
                            /* you must create a view of the entire memory-mapped file or a part of it */
                            //m_items = static_cast<TValue*>(MapViewOfFile(hMap,FILE_MAP_ALL_ACCESS,0,0,0));
                            m_items = static_cast<TValue*>(MapViewOfFileEx(
                                hMap,                   // HANDLE hFileMappingObject,
                                FILE_MAP_ALL_ACCESS,    // DWORD dwDesiredAccess,
                                dwFileOffsetHigh,       // DWORD dwFileOffsetHigh,
                                dwFileOffsetLow,        // DWORD dwFileOffsetLow,
                                gran,                   // SIZE_T dwNumberOfBytesToMap,
                                NULL ));                // PVOID pvBaseAddress

                            if (m_items == NULL) {
                                showlasterr("MapViewOfFileEx FAILED!");
                                clear();
                                throw std::bad_alloc();
                            }
#else
                            if (ftruncate(m_fd, sizeof(TValue) * new_size) < 0) {
                                throw std::bad_alloc();
                            }
#endif 
                        }
#ifndef _MSC_VER
                        if (munmap(m_items, sizeof(TValue) * m_size) < 0) {
                            throw std::bad_alloc();
                        }
                        m_items = static_cast<TValue*>(mmap(NULL, sizeof(TValue) * new_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
                        if (m_items == MAP_FAILED) {
                            throw std::bad_alloc();
                        }
#endif
                        m_size = new_siz;
                    }
                    m_items[id] = value;
                }

                const TValue operator[](const uint64_t id) const {
                    return m_items[id];
                }

                uint64_t size() const {
                    return m_size;
                }

                uint64_t used_memory() const {
                    return m_size * sizeof(TValue);
                }

                void clear() {
#ifdef _MSC_VER
                    // TODO: unmap
                    if (m_items)
                        UnmapViewOfFile(m_items);
                    m_items = 0;
                    if (hMap)
                        CloseHandle(hMap);
                    hMap = 0;
                    if (m_fd && (m_fd != INVALID_HANDLE_VALUE))
                        CloseHandle(m_fd);
                    m_fd = 0;
                    if (is_temp)
                        unlink(file.c_str());
#else 
                    munmap(m_items, sizeof(TValue) * m_size);
#endif
                }

            private:

                uint64_t m_size;

                TValue* m_items;
#ifdef _MSC_VER
                std::string file;
                HANDLE hMap;
                HANDLE m_fd;
                DWORD gran;
                DWORD dwFileOffsetHigh,dwFileOffsetLow;
                SIZE_T ps;
                LONG distToMoveLo, distToMoveHi;
                bool is_temp;
#else
                int m_fd;
#endif
                /// Get file size in bytes.
                uint64_t get_file_size() const {
#ifdef _MSC_VER
                    LARGE_INTEGER li;
                    uint64_t sz;
                    //DWORD dwSizeLow, dwSizeHigh;
                    //struct __stat64 s; 
                    //if (_fstat64( (int)m_fd, &s ) != 0) {
                    //    throw std::bad_alloc();
                    //}
                    if (!GetFileSizeEx(m_fd, &li)) {
                        throw std::bad_alloc();
                    }
                    sz = li.QuadPart;
                    return sz;
#else
                    struct stat s;
                    if (fstat(m_fd, &s) < 0) {
                        throw std::bad_alloc();
                    }
                    return s.st_size;
#endif
                }

            }; // class MmapFile

        } // namespace ById

    } // namespace Storage

} // namespace Osmium


#endif // #ifndef _TEST_MMAP_HXX_
// eof - test-mmap.hxx
