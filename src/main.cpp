#include <windows.h>
#include <iostream>

#include "../include/processQuery.hpp"

int WINAPI WinMain(HINSTANCE instanceHandle, 
                   HINSTANCE prevInstanceHandle, 
                   char* commandLine, int commandShow)
{
    for(const DWORD& processID : getEnumeratedProcessIDs())
    {
        std::cout << processID << ',';
    }

    return 0;
}
