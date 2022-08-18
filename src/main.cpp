#include <intercept.hpp>
#include "commands.hpp"
#include "Geometry/rotator.hpp"


Commands& commands = Commands::get();



int intercept::api_version() { //This is required for the plugin to work.
    return INTERCEPT_SDK_API_VERSION;
}

void intercept::register_interfaces() {
    
}

void intercept::pre_start() {
    Rotator rotator;
    
}

void intercept::pre_init() {
    intercept::sqf::system_chat("The Intercept template plugin is running!");
}

