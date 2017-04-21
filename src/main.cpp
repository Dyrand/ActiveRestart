#include <windows.h>
#include <Shlwapi.h>
#include <iostream>
#include <cstring>
#include <fstream>

#include "processQuery.hpp"

int WINAPI WinMain(HINSTANCE instanceHandle, 
                   HINSTANCE prevInstanceHandle, 
                   char* commandLine, int commandShow)
{
    const char szUniqueNamedMutex[] = "active_restart_";

    //Create mutex to prevent program from running twice
    HANDLE hHandle = CreateMutex( NULL, TRUE, szUniqueNamedMutex);

    if( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        return 0;
    }

    std::tstring outputFile = "tracked_programs.txt";
    char* szCurrentDirectory = new char[MAX_PATH];
    char* szCurrentOutputFile = new char[MAX_PATH];
    szCurrentOutputFile[0] = 0;

    GetCurrentDirectory(MAX_PATH,szCurrentDirectory);
    strcat(szCurrentOutputFile, szCurrentDirectory);
    strcat(szCurrentOutputFile, "\\");
    strcat(szCurrentOutputFile, outputFile.c_str());

    std::set<DWORD> processIDs;
    std::set<HANDLE> processHandles;
    std::set<std::tstring> processPaths;

    //If the output file exists, then open all programs
    if(PathFileExists(szCurrentOutputFile))
    {
        std::cout << "checking processes" << std::endl;

        //Get list of programs already running and remove duplictes
        processIDs = getProcessIDs();
    
        processHandles = getProcessHandles(processIDs);

        processPaths = getProcessExecutables(processHandles);

        processPaths = getResolvedPaths(processPaths);

        std::ifstream file_in(outputFile);
        
        std::string executable_path;
        getline(file_in,executable_path);

        DWORD wait_for_process = 2500; //Time in milliseconds

        while(file_in.good())
        {
            std::cout << "checking: " << executable_path << std::endl;
            if(PathFileExists(executable_path.c_str()) && 
               processPaths.count(executable_path) == 0)
            {
                std::cout << "starting: " << executable_path << std::endl;
                STARTUPINFO startup;
                PROCESS_INFORMATION processInfo;

                memset(&startup, 0, sizeof(startup));
                memset(&processInfo, 0, sizeof(processInfo)); 
                startup.cb = sizeof(startup);
                
                CreateProcess(
                    executable_path.c_str(), //lpApplicationName
                    NULL, //lpCommandLine
                    NULL, //lpProcessAttributes
                    NULL, //lpThreadAttributes
                    FALSE, //bInheritHandles
                    DETACHED_PROCESS, //dwCreationFlags
                    NULL, //lpEnvironment
                    NULL, //lpCurrentDirectory
                    &startup, //lpStartupInfo
                    &processInfo //lpProcessInformation
                    );

                WaitForSingleObject( processInfo.hProcess, wait_for_process );
                CloseHandle( processInfo.hProcess );
                CloseHandle( processInfo.hThread );        
            }

            getline(file_in,executable_path);
        }

        std::cout << "done checking processes" << std::endl;
    }

    DWORD sleep_interval = 15000; //In milliseconds

    while(true)
    {
        std::ofstream file_out(outputFile, std::ios_base::trunc);

        processIDs = getProcessIDs();
    
        processHandles = getProcessHandles(processIDs);

        processPaths = getProcessExecutables(processHandles);

        processPaths = getResolvedPaths(processPaths);

        for(const std::tstring& path : processPaths)
        {
            file_out << path << "\n";
        }

        Sleep(sleep_interval);
    }

    ReleaseMutex( hHandle );
    CloseHandle( hHandle );

    return 0;
}
