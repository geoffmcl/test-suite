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
#include <stdint.h>
#ifdef _MSC_VER
#include <Windows.h>
#include "byid.hpp"
#include "winErr.hpp"
#include "MMFIODef.h"
#else
#include <sys/mman.h>
#include <unistd.h>
#include "byid.hpp"
// #include <osmium/storage/byid.hpp>
#endif
#include "sprtf.hxx"
#include "utils.hxx"

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

#ifdef _MSC_VER
            /**
            * MmapFile stores data in files using the CreateFileMapping/MapViewOfFile system call.
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
                MmapFile(const std::string& infile="", bool remove=true) :
                    Base<TValue>(),
                    m_size(1),
                    is_temp(false) 
                {
                    file = infile;
                    if (file == "") {
                        file = gettempfilename();
                        is_temp = true;
                    }
	                bool bRet = mmf.Open(file.c_str(), OPN_READWRITE, true, 4096);
                	if( ! bRet ) {
                        std::string err;
                        mmf.GetMMFLastError(err);
                        fprintf(stderr, "Failed to open file '%s' - %s\n", file.c_str(), err.c_str());
                        mmf.Close();
                        throw std::bad_alloc();
                    }
                    do_delete = remove;
                }

                ~MmapFile() {
                    clear();
                }

                void set(const uint64_t id, const TValue value) {
                    quint sz = sizeof(TValue);
                    uint64_t offset =  id * sz;
                    if ( !mmf.Seek( offset, SP_BEGIN, true ) ) {
                        throw std::bad_alloc();
                    }
                    if( mmf.Write( (void *)&value, sz ) != sz ) {
                        throw std::bad_alloc();
                    }
                    if (id > m_size)
                        m_size = id;

                }

                //const TValue operator[](const uint64_t id) const {
                TValue operator[](uint64_t id) {
                    quint sz = sizeof(TValue);
                    uint64_t offset =  id * sz;
                    char buf[26];
                    char *cp = buf;
                    if ( !mmf.Seek( offset, SP_BEGIN ) ) {
                        throw std::bad_alloc();
                    }
                    if (mmf.Read((void *)cp,sz) != sz) {
                        throw std::bad_alloc();
                    }
                    TValue *ptv = (TValue *)cp;
                    return *ptv;
                }

                uint64_t size() const {
                    return m_size;
                }

                uint64_t used_memory() const {
                    return m_size * sizeof(TValue);
                }

                void clear() {
                    mmf.Close();
                    if (do_delete || is_temp) {
                        unlink(file.c_str());   // toss the files
                    }
                }

            private:

               	CWinMMFIO mmf;

                std::string file;
                bool is_temp;
                bool do_delete;
                uint64_t m_size;

                /// Get file size in bytes.
                uint64_t get_file_size() {
                    uint64_t len;
                    if ( !mmf.GetLength(len) ) {
                        mmf.Close();
                        throw std::bad_alloc();
                    }
                    return len;
                }

            }; // class MmapFile


#else // !_MSC_VER

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
                    m_size(1) {
                    if (filename == "") {
                        FILE* file = tmpfile();
                        if (!file) {
                            throw std::bad_alloc();
                        }
                        m_fd = fileno(file);
                    } else {
                        m_fd = open(filename.c_str(), O_RDWR | O_CREAT, 0600);
                    }

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

                    // make sure the file is at least as large as the initial size
                    if (get_file_size() < sizeof(TValue) * m_size) {
                        if (ftruncate(m_fd, sizeof(TValue) * m_size) < 0) {
                            throw std::bad_alloc();
                        }
                    }

                    m_items = static_cast<TValue*>(mmap(NULL, sizeof(TValue) * m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
                    if (m_items == MAP_FAILED) {
                        throw std::bad_alloc();
                    }
                }

                ~MmapFile() {
                    clear();
                }

                void set(const uint64_t id, const TValue value) {
                    if (id >= m_size) {
                        uint64_t new_size = id + size_increment;

                        // if the file backing this mmap is smaller than needed, increase its size
                        if (get_file_size() < sizeof(TValue) * new_size) {
                            if (ftruncate(m_fd, sizeof(TValue) * new_size) < 0) {
                                throw std::bad_alloc();
                            }
                        }

                        if (munmap(m_items, sizeof(TValue) * m_size) < 0) {
                            throw std::bad_alloc();
                        }
                        m_items = static_cast<TValue*>(mmap(NULL, sizeof(TValue) * new_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
                        if (m_items == MAP_FAILED) {
                            throw std::bad_alloc();
                        }
                        m_size = new_size;
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
                    munmap(m_items, sizeof(TValue) * m_size);
                }

            private:

                uint64_t m_size;

                TValue* m_items;

                int m_fd;

                /// Get file size in bytes.
                uint64_t get_file_size() const {
                    struct stat s;
                    if (fstat(m_fd, &s) < 0) {
                        throw std::bad_alloc();
                    }
                    return s.st_size;
                }

            }; // class MmapFile
#endif // _MSC_VER y/n

        } // namespace ById

    } // namespace Storage

} // namespace Osmium



#endif // #ifndef _TEST_MMAP_HXX_
// eof - test-mmap.hxx
