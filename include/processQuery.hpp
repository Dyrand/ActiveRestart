#ifndef PROCESSQUERY_HPP
#define PROCESSQUERY_HPP

#include <windows.h>
#include <string>
#include <set>

//Switch between the correct tstring type depending on UNICODE support
namespace std {
    #ifdef _UNICODE
    typedef wstring tstring;
    #else
    typedef string tstring;
    #endif // _UNICODE
};

//Defines the base character type
typedef std::remove_pointer<LPTSTR>::type base_char;

//Returns: A std::set containing process IDs
std::set<DWORD> getProcessIDs();

//Return: A std::set cotaining process Handles
//Note: A handle may not be obtainable for all processes
std::set<HANDLE> getProcessHandles(std::set<DWORD> &processIDs);

//Return: A std::set containing process executable paths
std::set<std::tstring> getProcessExecutables(std::set<HANDLE> &processHandles);

//Return: A std::set containing process paths resolved to drive letters
std::set<std::tstring> getResolvedPaths(std::set<std::tstring> & processPaths);

#endif /* PROCESSQUERY_HPP */