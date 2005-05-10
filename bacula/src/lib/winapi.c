/*
 * Windows APIs that are different for each system.
 *   We use pointers to the entry points so that a
 *   single binary will run on all Windows systems.
 *
 *     Kern Sibbald MMIII
 */
/*
   Copyright (C) 2000-2005 Kern Sibbald

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.

 */

#include "bacula.h"

#if defined(HAVE_CYGWIN) || defined(HAVE_WIN32)

#include "winapi.h"

// init with win9x, but maybe set to NT in InitWinAPI
DWORD  g_platform_id = VER_PLATFORM_WIN32_WINDOWS;


/* API Pointers */

t_OpenProcessToken      p_OpenProcessToken = NULL;
t_AdjustTokenPrivileges p_AdjustTokenPrivileges = NULL;
t_LookupPrivilegeValue  p_LookupPrivilegeValue = NULL;

t_SetProcessShutdownParameters p_SetProcessShutdownParameters = NULL;

t_CreateFileA   p_CreateFileA = NULL;
t_CreateFileW   p_CreateFileW = NULL;

t_wunlink p_wunlink = NULL;
t_wmkdir p_wmkdir = NULL;
t_wopen p_wopen = NULL;

t_cgetws p_cgetws = NULL;
t_cwprintf p_cwprintf = NULL;

t_GetFileAttributesA    p_GetFileAttributesA = NULL;
t_GetFileAttributesW    p_GetFileAttributesW = NULL;

t_GetFileAttributesExA  p_GetFileAttributesExA = NULL;
t_GetFileAttributesExW  p_GetFileAttributesExW = NULL;

t_SetFileAttributesA    p_SetFileAttributesA = NULL;
t_SetFileAttributesW    p_SetFileAttributesW = NULL;
t_BackupRead            p_BackupRead = NULL;
t_BackupWrite           p_BackupWrite = NULL;
t_WideCharToMultiByte p_WideCharToMultiByte = NULL;
t_MultiByteToWideChar p_MultiByteToWideChar = NULL;

t_FindFirstFileA p_FindFirstFileA = NULL;
t_FindFirstFileW p_FindFirstFileW = NULL;

t_FindNextFileA p_FindNextFileA = NULL;
t_FindNextFileW p_FindNextFileW = NULL;

t_SetCurrentDirectoryA p_SetCurrentDirectoryA = NULL;
t_SetCurrentDirectoryW p_SetCurrentDirectoryW = NULL;

t_GetCurrentDirectoryA p_GetCurrentDirectoryA = NULL;
t_GetCurrentDirectoryW p_GetCurrentDirectoryW = NULL;


void 
InitWinAPIWrapper() 
{
   HMODULE hLib = LoadLibraryA("KERNEL32.DLL");
   if (hLib) {
      /* create file calls */
      p_CreateFileA = (t_CreateFileA)
          GetProcAddress(hLib, "CreateFileA");
      p_CreateFileW = (t_CreateFileW)
          GetProcAddress(hLib, "CreateFileW");      

      /* attribute calls */
      p_GetFileAttributesA = (t_GetFileAttributesA)
          GetProcAddress(hLib, "GetFileAttributesA");
      p_GetFileAttributesW = (t_GetFileAttributesW)
          GetProcAddress(hLib, "GetFileAttributesW");
      p_GetFileAttributesExA = (t_GetFileAttributesExA)
          GetProcAddress(hLib, "GetFileAttributesExA");
      p_GetFileAttributesExW = (t_GetFileAttributesExW)
          GetProcAddress(hLib, "GetFileAttributesExW");
      p_SetFileAttributesA = (t_SetFileAttributesA)
          GetProcAddress(hLib, "SetFileAttributesA");
      p_SetFileAttributesW = (t_SetFileAttributesW)
          GetProcAddress(hLib, "SetFileAttributesW");
      /* process calls */
      p_SetProcessShutdownParameters = (t_SetProcessShutdownParameters)
          GetProcAddress(hLib, "SetProcessShutdownParameters");
      /* backup calls */
      p_BackupRead = (t_BackupRead)
          GetProcAddress(hLib, "BackupRead");
      p_BackupWrite = (t_BackupWrite)
          GetProcAddress(hLib, "BackupWrite");
      /* char conversion calls */
      p_WideCharToMultiByte = (t_WideCharToMultiByte)
          GetProcAddress(hLib, "WideCharToMultiByte");
      p_MultiByteToWideChar = (t_MultiByteToWideChar)
          GetProcAddress(hLib, "MultiByteToWideChar");

      /* find files */
      p_FindFirstFileA = (t_FindFirstFileA)
          GetProcAddress(hLib, "FindFirstFileA"); 
      p_FindFirstFileW = (t_FindFirstFileW)
          GetProcAddress(hLib, "FindFirstFileW");       
      p_FindNextFileA = (t_FindNextFileA)
          GetProcAddress(hLib, "FindNextFileA");
      p_FindNextFileW = (t_FindNextFileW)
          GetProcAddress(hLib, "FindNextFileW");
      /* set and get directory */
      p_SetCurrentDirectoryA = (t_SetCurrentDirectoryA)
          GetProcAddress(hLib, "SetCurrentDirectoryA");
      p_SetCurrentDirectoryW = (t_SetCurrentDirectoryW)
          GetProcAddress(hLib, "SetCurrentDirectoryW");       
      p_GetCurrentDirectoryA = (t_GetCurrentDirectoryA)
          GetProcAddress(hLib, "GetCurrentDirectoryA");
      p_GetCurrentDirectoryW = (t_GetCurrentDirectoryW)
          GetProcAddress(hLib, "GetCurrentDirectoryW");      
      FreeLibrary(hLib);
   }
   
   hLib = LoadLibraryA("MSVCRT.DLL");
   if (hLib) {
      /* unlink */
      p_wunlink = (t_wunlink)
      GetProcAddress(hLib, "_wunlink");
      /* wmkdir */
      p_wmkdir = (t_wmkdir)
      GetProcAddress(hLib, "_wmkdir");
      /* wopen */
      p_wopen = (t_wopen)
      GetProcAddress(hLib, "_wopen");
      
      /* cgetws */
      p_cgetws = (t_cgetws)
      GetProcAddress (hLib, "_cgetws");
      /* cwprintf */
      p_cwprintf = (t_cwprintf)
      GetProcAddress (hLib, "_cwprintf");
      
      FreeLibrary(hLib);
   }
   
   hLib = LoadLibraryA("ADVAPI32.DLL");
   if (hLib) {
      p_OpenProcessToken = (t_OpenProcessToken)
         GetProcAddress(hLib, "OpenProcessToken");
      p_AdjustTokenPrivileges = (t_AdjustTokenPrivileges)
         GetProcAddress(hLib, "AdjustTokenPrivileges");
      p_LookupPrivilegeValue = (t_LookupPrivilegeValue)
         GetProcAddress(hLib, "LookupPrivilegeValueA");
      FreeLibrary(hLib);
   }

   // do we run on win 9x ???
   OSVERSIONINFO osversioninfo;
   osversioninfo.dwOSVersionInfoSize = sizeof(osversioninfo);

   // Get the current OS version
   if (!GetVersionEx(&osversioninfo)) {
      g_platform_id = 0;
   } else {
      g_platform_id = osversioninfo.dwPlatformId;
   }

   if (g_platform_id == VER_PLATFORM_WIN32_WINDOWS) {
      p_BackupRead = NULL;
      p_BackupWrite = NULL;

      p_CreateFileW = NULL;          
      p_GetFileAttributesW = NULL;          
      p_GetFileAttributesExW = NULL;
          
      p_SetFileAttributesW = NULL;
                
      p_FindFirstFileW = NULL;
      p_FindNextFileW = NULL;
      p_SetCurrentDirectoryW = NULL;
      p_GetCurrentDirectoryW = NULL;

      p_wunlink = NULL;
      p_wmkdir = NULL;
      p_wopen = NULL;
   }   
}
#endif
