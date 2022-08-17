#include "commands.hpp"
#include <algorithm>
#include <cmath>


Commands& commands = Commands::get();



// returns a value inbetween 0..360
float clampAxis(float angle)
{
    angle = std::fmod(angle, 360);

    if (angle < 0)
    {
        angle = angle + 360.0f;
    }

    return angle;
}

inline float normalizeAxis(float angle)
{
    // make sure angle is with in (0,360)
    angle = clampAxis(angle);

    if (angle > 180.0f)
    {
        angle = angle - 360.0f;     // in the range of -180,180 normalized
    }

    return angle;
}

game_value clampAngle(game_value_parameter right_arg)
{
   float angle = right_arg[0];
   float minAngle = right_arg[1];
   float maxAngle = right_arg[2];

   float deltaAngle = clampAxis(maxAngle - minAngle);
   float centerAngle = clampAxis(minAngle + deltaAngle);
   float centerDisplacement = normalizeAxis(angle - centerAngle);

   // clamp values here
   if (centerDisplacement > deltaAngle)
   {
       return normalizeAxis(centerAngle + deltaAngle);
   } 
   else if (centerDisplacement < -deltaAngle)
   {
       return normalizeAxis(centerAngle - deltaAngle);
   }

   return normalizeAxis(angle);
   
}

int intercept::api_version() { //This is required for the plugin to work.
    return INTERCEPT_SDK_API_VERSION;
}

void intercept::register_interfaces() {
    
}

void intercept::pre_start() {
    commands.addCommand("clampAngle", "clamp angles a,b,c", userFunctionWrapper<clampAngle>, intercept::types::GameDataType::SCALAR, intercept::types::GameDataType::ARRAY);
    
}

void intercept::pre_init() {
    intercept::sqf::system_chat("The Intercept template plugin is running!");
}

