#include <windows.h>
#include <Psapi.h>
#include <iostream>
#include <algorithm>
#include <exception>
#include <string>
#include <array>
#include <set>

#include "../include/processQuery.hpp"

//Core Api Call: EnumProcesses
std::set<DWORD> getEnumeratedProcessIDs()
{
    static std::size_t array_capacity = 1000;
    static DWORD* processIDarray = new DWORD[array_capacity];
    DWORD numberOfBytes = 0;
    bool success;

    success = EnumProcesses(processIDarray, sizeof(DWORD)*array_capacity, &numberOfBytes);
    if(!success)
    {
        int last_error = GetLastError();
    }

    //If array was filled, try calling again with larger capacity
    // to possibly get more processIDs
    while(sizeof(DWORD)*array_capacity == numberOfBytes)
    {
        try
        {
            delete[] processIDarray;
            array_capacity *= 2;
            processIDarray = new DWORD[array_capacity];
        }
        catch(const std::exception& e)
        {
            break;
        }
        
        success = EnumProcesses(processIDarray, sizeof(DWORD)*array_capacity, &numberOfBytes);
        
        if(!success)
        {
            int last_error = GetLastError();
        }
    }

    std::set<DWORD> outputIDs;

    for(int index = 0; numberOfBytes >  0; numberOfBytes -= sizeof(DWORD), ++index)
    {
        outputIDs.insert(processIDarray[index]);
    }
    
    return outputIDs;
}