#include "timing.h"
#include "../commands.hpp"
#include <future>
#include <iostream>
#include <fstream>
#include <future>

namespace Debug
{
    static std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
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

    game_value Timing::setStartTime()
    {
        startTime = std::chrono::high_resolution_clock::now();
        return true;
    }

    game_value Timing::setEndTime()
    {
        endTime = std::chrono::high_resolution_clock::now();
        return true;
    }

    void Timing::fileOperations()
    {
        std::chrono::duration<float> elapsedTimeSeconds = endTime - startTime;
        std::ofstream MyFile;
        MyFile.open("D:\\log\\log.txt", std::ios_base::app);
        MyFile << elapsedTimeSeconds.count() << "s\n";
        MyFile.close();
    }

    game_value Timing::logTimeDifference()
    {
        auto ret = std::async(fileOperations);
        return true;
    }
}

