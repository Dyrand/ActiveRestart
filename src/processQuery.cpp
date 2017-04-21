#include <windows.h>
#include <FileAPI.h>
#include <Psapi.h>
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <exception>
#include <array>
#include <set>
#include <map>
#include <utility>
#include <tchar.h>

#include "processQuery.hpp"

//Core API Call: EnumProcesses
std::set<DWORD> getProcessIDs()
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

//Core API call OpenProcess
std::set<HANDLE> getProcessHandles(std::set<DWORD> &processIDs)
{
    std::set<HANDLE> processHandles;
    HANDLE currentHANDLE;

    for(const DWORD &processID : processIDs)
    {
        currentHANDLE = OpenProcess( 
                            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                            FALSE,
                            processID
                        );
        
        if(currentHANDLE != NULL)
        {
            processHandles.insert(currentHANDLE);
        }
    }

    return processHandles;
}

//Internal Function
//Returns the executable path for a single function
std::tstring getProcessExecutable(const HANDLE& processHandle)
{
    std::vector<base_char> processPath;
    processPath.reserve(512);
    auto max_size = processPath.max_size();
    std::vector<base_char>::size_type length_copied;
    bool full = true;

    while(full)
    {
        length_copied = GetProcessImageFileName(processHandle, processPath.data(), processPath.capacity());
        if(length_copied == processPath.capacity())
        {
            if(processPath.capacity() == max_size)
            {
                break;
            }
            else if(processPath.capacity()*2 > max_size)
            {
                processPath.reserve(max_size);
            }
            else
            {
                processPath.reserve(processPath.capacity()*2);
            }
        }
        else
        {
            full = false;
        }
    }

    std::tstring path(processPath.begin(), processPath.begin()+length_copied);

    return path;
}

//Core API call GetProcessImageFileName
std::set<std::tstring> getProcessExecutables(std::set<HANDLE> &processHandles)
{
    std::set<std::tstring> processPaths;

    for(const HANDLE &processHandle: processHandles)
    {
        processPaths.insert(getProcessExecutable(processHandle));
    }

    //Remove empty string from set if it exists
    if(processPaths.count(std::tstring()) == 1)
    {
        processPaths.erase(std::tstring());
    }

    return processPaths;
}

//Internal Function
//Map device names to drive letters
std::map<std::tstring,std::tstring> InitializeDeviceMap()
{
    std::map<std::tstring,std::tstring> hardDiskCollection;
    TCHAR tszLinkName[MAX_PATH] = { 0 };
    TCHAR tszDevName[MAX_PATH] = { 0 };
    TCHAR tcDrive = 0;

    strcpy( tszLinkName, "a:" );
    for ( tcDrive = 'a'; tcDrive < 'z'; ++tcDrive )
    {
        tszLinkName[0] = tcDrive;
        if ( QueryDosDevice( tszLinkName, tszDevName, MAX_PATH ) )
        {
            hardDiskCollection.insert( std::pair<std::tstring, std::tstring>( tszDevName , tszLinkName) );
        }
    }

    return hardDiskCollection;
}

//Core API call QueryDosDevice
std::set<std::tstring> getResolvedPaths(std::set<std::tstring> & processPaths)
{
    //Maps devices names to disk letters
    static std::map<std::tstring,std::tstring> device_map = InitializeDeviceMap();
    std::set<std::tstring> resolvedPaths;

    for(std::tstring path : processPaths)
    {
        std::size_t end_index = path.find("HarddiskVolume");
        end_index = path.find("\\" , end_index+1);
        std::tstring device_name = path.substr(0,end_index);

        resolvedPaths.insert(path.replace(0,end_index,device_map.at(device_name)));
    }
    
    return resolvedPaths;
}
