/*\
 * test-deldir.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
// other includes
#include "test-deldir.hxx"

static const char *module = "test-deldir";

// implementation
// from : http://stackoverflow.com/questions/734717/how-to-delete-a-folder-in-c
#ifdef WIN32
#include <string>
#include <iostream>

#include <windows.h>
#include <conio.h>

int DeleteDirectory(const std::string &refcstrRootDirectory,
                    bool              bDeleteSubdirectories = true);

int DeleteDirectory(const std::string &refcstrRootDirectory,
                    bool              bDeleteSubdirectories)
{
    int             iRC;
    bool            bSubdirectory = false;       // Flag, indicating whether
                                                 // subdirectories have been found
    HANDLE          hFile;                       // Handle to directory
    std::string     strFilePath;                 // Filepath
    std::string     strPattern;                  // Pattern
    WIN32_FIND_DATA FileInformation;             // File information

    strPattern = refcstrRootDirectory + "\\*.*";
    hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
    if( hFile == INVALID_HANDLE_VALUE ) {
      return 1;
    }

    do
    {
        if ( strcmp(FileInformation.cFileName,".") && strcmp(FileInformation.cFileName,"..") ) {
            // it is NOT '.' or '..'
            strFilePath.erase();
            strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

            if( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                if( bDeleteSubdirectories ) {
                    // Delete subdirectory
                    iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
                    if(iRC) {
                        ::FindClose(hFile); // Close handle
                        return iRC;
                    }
                }
                else
                    bSubdirectory = true;
            } else {
                // Set file attributes
                if(::SetFileAttributes(strFilePath.c_str(),FILE_ATTRIBUTE_NORMAL) == FALSE) {
                    iRC = ::GetLastError();
                    ::FindClose(hFile); // Close handle
                    return iRC;
                }

                // Delete file
                if(::DeleteFile(strFilePath.c_str()) == FALSE) {
                    iRC = ::GetLastError();
                    ::FindClose(hFile); // Close handle
                    return iRC;
                }
            }
        }
    } while(::FindNextFile(hFile, &FileInformation) == TRUE);

    DWORD dwError = ::GetLastError();
    // Close handle
    ::FindClose(hFile);
    if(dwError != ERROR_NO_MORE_FILES)
      return dwError;
    else {
        if(!bSubdirectory) {
            // Set directory attributes
            if(::SetFileAttributes(refcstrRootDirectory.c_str(),FILE_ATTRIBUTE_NORMAL) == FALSE) {
                return ::GetLastError();
            }

            // Delete directory
            if(::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE) {
                return ::GetLastError();
            }
        }
    }

    return 0;
}


int test_deldir()
{
  int         iRC                  = 0;
  std::string strDirectoryToDelete = "c:\\mydir";


  // Delete 'c:\mydir' without deleting the subdirectories
  iRC = DeleteDirectory(strDirectoryToDelete, false);
  if(iRC)
  {
    std::cout << "Error " << iRC << std::endl;
    return -1;
  }

  // Delete 'c:\mydir' and its subdirectories
  iRC = DeleteDirectory(strDirectoryToDelete);
  if(iRC)
  {
    std::cout << "Error " << iRC << std::endl;
    return -1;
  }

  // Wait for keystroke
  _getch();

  return 0;
}

///////////////////////////////////////////////////////////////////
// MSDN samples
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

DWORD DisplayErrorBox(LPTSTR lpszFunction);

int test_tmain(int argc, TCHAR *argv[])
{
   WIN32_FIND_DATA ffd;
   LARGE_INTEGER filesize;
   TCHAR szDir[MAX_PATH];
   size_t length_of_arg;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   DWORD dwError=0;
   
   // If the directory is not specified as a command-line argument,
   // print usage.

   if(argc != 2)
   {
      _tprintf(TEXT("\nUsage: %s <directory name>\n"), argv[0]);
      return (-1);
   }

   // Check that the input path plus 3 is not longer than MAX_PATH.
   // Three characters are for the "\*" plus NULL appended below.

   StringCchLength(argv[1], MAX_PATH, &length_of_arg);

   if (length_of_arg > (MAX_PATH - 3))
   {
      _tprintf(TEXT("\nDirectory path is too long.\n"));
      return (-1);
   }

   _tprintf(TEXT("\nTarget directory is %s\n\n"), argv[1]);

   // Prepare string for use with FindFile functions.  First, copy the
   // string to a buffer, then append '\*' to the directory name.

   StringCchCopy(szDir, MAX_PATH, argv[1]);
   StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

   // Find the first file in the directory.

   hFind = FindFirstFile(szDir, &ffd);

   if (INVALID_HANDLE_VALUE == hFind) 
   {
      DisplayErrorBox(TEXT("FindFirstFile"));
      return dwError;
   } 
   
   // List all the files in the directory with some info about them.

   do
   {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         _tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
      }
      else
      {
         filesize.LowPart = ffd.nFileSizeLow;
         filesize.HighPart = ffd.nFileSizeHigh;
         _tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
      }
   }
   while (FindNextFile(hFind, &ffd) != 0);
 
   dwError = GetLastError();
   if (dwError != ERROR_NO_MORE_FILES) 
   {
      DisplayErrorBox(TEXT("FindFirstFile"));
   }

   FindClose(hFind);
   return dwError;
}


DWORD DisplayErrorBox(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and clean up

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    return dw;
}




//////////////////////////////////////////////////////////////////
#endif // WIN32

#ifdef HAVE_DIRENT_H
void remove_dir(char *path)
{
        struct dirent *entry = NULL;
        DIR *dir = NULL;
        dir = opendir(path);
        while(entry = readdir(dir))
        {   
                DIR *sub_dir = NULL;
                FILE *file = NULL;
                char abs_path[264];
                abd_path[0] = 0;
                if(strcmp(entry->d_name,".") && strcmp(entry->d_name,".."))
                {   
                        sprintf(abs_path, "%s/%s", path, entry->d_name);
                        if(sub_dir = opendir(abs_path))
                        {   
                                closedir(sub_dir);
                                remove_dir(abs_path);
                        }   
                        else 
                        {   
                                if(file = fopen(abs_path, "r"))
                                {   
                                        fclose(file);
                                        remove(abs_path);
                                }   
                        }   
                }   
        }   
        remove(path);
}


// from : http://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c
int remove_directory(const char *path)
{
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d)
   {
      struct dirent *p;

      r = 0;

      while (!r && (p=readdir(d)))
      {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
          {
             continue;
          }

          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);

          if (buf)
          {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);

             if (!stat(buf, &statbuf))
             {
                if (S_ISDIR(statbuf.st_mode))
                {
                   r2 = remove_directory(buf);
                }
                else
                {
                   r2 = unlink(buf);
                }
             }

             free(buf);
          }

          r = r2;
      }

      closedir(d);
   }

   if (!r)
   {
      r = rmdir(path);
   }

   return r;
}

#endif // HAVE_DIRENT_h

#ifdef UNIX_LIKE_SYSTEM
int recursive_delete(const char *dir)
{
    int ret = 0;
    FTS *ftsp = NULL;
    FTSENT *curr;

    // Cast needed (in C) because fts_open() takes a "char * const *", instead
    // of a "const char * const *", which is only allowed in C++. fts_open()
    // does not modify the argument.
    char *files[] = { (char *) dir, NULL };

    // FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
    //                in multithreaded programs
    // FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
    //                of the specified directory
    // FTS_XDEV     - Don't cross filesystem boundaries
    ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);
    if (!ftsp) {
        fprintf(stderr, "%s: fts_open failed: %s\n", dir, strerror(errno));
        ret = -1;
        goto finish;
    }

    while ((curr = fts_read(ftsp))) {
        switch (curr->fts_info) {
        case FTS_NS:
        case FTS_DNR:
        case FTS_ERR:
            fprintf(stderr, "%s: fts_read error: %s\n",
                    curr->fts_accpath, strerror(curr->fts_errno));
            break;

        case FTS_DC:
        case FTS_DOT:
        case FTS_NSOK:
            // Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were
            // passed to fts_open()
            break;

        case FTS_D:
            // Do nothing. Need depth-first search, so directories are deleted
            // in FTS_DP
            break;

        case FTS_DP:
        case FTS_F:
        case FTS_SL:
        case FTS_SLNONE:
        case FTS_DEFAULT:
            if (remove(curr->fts_accpath) < 0) {
                fprintf(stderr, "%s: Failed to remove: %s\n",
                        curr->fts_path, strerror(errno));
                ret = -1;
            }
            break;
        }
    }

finish:
    if (ftsp) {
        fts_close(ftsp);
    }

    return ret;
}


#endif // UNIX_LIKE_SYSTEM


// eof = test-deldir.cxx
