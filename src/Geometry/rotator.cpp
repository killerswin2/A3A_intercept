#include "rotator.hpp"
#include <algorithm>
#include <cmath>
#include "../commands.hpp"


Rotator::Rotator()
{

    Commands& commands = Commands::get();
    commands.addCommand("clampAngle", "clamp angles a,b,c", userFunctionWrapper<clampAngle>, intercept::types::GameDataType::SCALAR, intercept::types::GameDataType::ARRAY);
    commands.addCommand("clampAxis", "clamp an axis between 0..360", userFunctionWrapper<clampAxis>, intercept::types::GameDataType::SCALAR, intercept::types::GameDataType::SCALAR);
    commands.addCommand("normalizeAxis", "normalize an axis between -180..180", userFunctionWrapper<normalizeAxis>, intercept::types::GameDataType::SCALAR, intercept::types::GameDataType::SCALAR);
}

Rotator::~Rotator()
{
}
// returns a value between 0..360
float Rotator::clampAxisInternal(float angle)
{
    angle = std::fmod(angle, 360);

    if (angle < 0)
    {
        angle = angle + 360.0f;
    }

    return angle;
}

// returns a value between -180..180
float Rotator::normalizeAxisInternal(float angle)
{
    // make sure angle is with in (0,360)
    angle = clampAxis(angle);

    if (angle > 180.0f)
    {
        angle = angle - 360.0f;     // in the range of -180,180 normalized
    }

    return angle;
}

// clamps an Angle between min angle and max angle.
// coterminal Angles are handled.
float Rotator::clampAngleInternal(float angle, float minAngle, float maxAngle)
{
    float deltaAngle = clampAxisInternal(maxAngle - minAngle);
    float centerAngle = clampAxisInternal(minAngle + deltaAngle);
    float centerDisplacement = normalizeAxisInternal(angle - centerAngle);

    // clamp values here
    if (centerDisplacement > deltaAngle)
    {
        return normalizeAxisInternal(centerAngle + deltaAngle);
    }
    else if (centerDisplacement < -deltaAngle)
    {
        return normalizeAxisInternal(centerAngle - deltaAngle);
    }

    return normalizeAxisInternal(angle);
}

game_value Rotator::clampAngle(game_value_parameter right_arg)
{
    return clampAngleInternal(right_arg[0], right_arg[1], right_arg[2]);
}

game_value Rotator::normalizeAxis(game_value_parameter right_arg)
{
    return Rotator::normalizeAxisInternal(right_arg);
}

game_value Rotator::clampAxis(game_value_parameter right_arg)
{
    return Rotator::clampAxisInternal(right_arg);
}