#ifndef PROCESSQUERY_HPP
#define PROCESSQUERY_HPP

#include <windows.h>
#include <string>
#include <set>

//Assumptions
// - There are no repeated process IDs
//Input: N/A
//Returns: A std::set containing process IDs
std::set<DWORD> getEnumeratedProcessIDs();

#endif /* PROCESSQUERY_HPP */