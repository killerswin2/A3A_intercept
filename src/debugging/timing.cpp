#include "timing.h"
#include "../commands.hpp"
#include <filesystem>
#include <future>
#include <iostream>
#include <fstream>
#include <future>

#ifdef _WIN32
#include <windows.h> 
#include <Shlobj.h>
#define INITKNOWNFOLDERS
#include <Knownfolders.h>
#endif

namespace Debug
{
    static std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;


    /*
     * @brief Timing constructor
     * @author Killerswin2
     * @detail adds timing is for a high resolution clock to test code in sqf
     */
    Timing::Timing()
    {
        Commands& commands = Commands::get();
        commands.addCommand("setStartTime", "start time", userFunctionWrapper<setStartTime>, intercept::types::GameDataType::BOOL);
        commands.addCommand("setEndTime", "end time", userFunctionWrapper<setEndTime>, intercept::types::GameDataType::BOOL);
        commands.addCommand("logTimeDifference", "end time - start time", userFunctionWrapper<logTimeDifference>, intercept::types::GameDataType::BOOL);
    }
    Timing::~Timing()
    {
    }

    /*
     * @brief assigns the start time
     * @author Killerswin2
     * @return BOOL returns true always
     */
    game_value Timing::setStartTime()
    {
        startTime = std::chrono::high_resolution_clock::now();
        return true;
    }

    /*
     * @brief assigns the end time
     * @author Killerswin2
     * @return BOOL returns true always
     */
    game_value Timing::setEndTime()
    {
        endTime = std::chrono::high_resolution_clock::now();
        return true;
    }

    /*
     * @brief calculates the difference of end time and start time
     * @detail this is done async it logs the data to a file, documents for windows, var/log for linux
     * @author Killerswin2
     * @return BOOL returns true always
     */
    void Timing::fileOperationsInternal()
    {
        std::chrono::duration<float> elapsedTimeSeconds = endTime - startTime;

        //@todo discuss a default log path, as users might not have a c: or d: drive also, its windows specific right now
        std::filesystem::path fsfilepath;
        std::ofstream MyFile;

        //@todo linux here
        //filepath here /var/log/yourapp.log

        // change the log file location
        #ifdef _WIN32
        //FOLDERID_Documents
        wchar_t* path;
        HRESULT handleResult = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path);
        if (SUCCEEDED(handleResult))
        {
            std::wstring new_path = path;
            new_path.append(L"\\log\\log.log");
            fsfilepath = new_path;
        }
        #endif

        // @Todo better file error handling here
        MyFile.open(fsfilepath, std::ios_base::app);
        if (MyFile.bad())
        {
            std::cout << "my is null" << std::endl;
        }
        MyFile << elapsedTimeSeconds.count() << "s\n";
        MyFile.close();

        #ifdef _WIN32
        CoTaskMemFree(path);
        #endif
    }

    /*
     * @brief calls fileOperationsInternal to calculate time difference
     * @author Killerswin2
     * @return BOOL returns true always
     */
    game_value Timing::logTimeDifference()
    {
        auto ret = std::async(fileOperationsInternal);
        return true;
    }
}

