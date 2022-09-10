#include <intercept.hpp>
#include <chrono>
#include <string>
#include "commands.hpp"
#include "Geometry/rotator.hpp"
#include "debugging/timing.h"
#include "hashing/jsonHashMap.hpp"


Commands& commands = Commands::get();



int intercept::api_version() { //This is required for the plugin to work.
    return INTERCEPT_SDK_API_VERSION;
}

void intercept::register_interfaces() {
    
}

void intercept::pre_start() {
    Rotator rotator;
    Debug::Timing timer;
    hashMap::JsonHashMap::preStart();
    
}

void intercept::pre_init() {
    // you will be missed system_chat("The Intercept template plugin is running!")
}

