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
    commands.addCommand("clamp", "clamp vec3 components", userFunctionWrapper<clamp>, intercept::types::GameDataType::ARRAY, intercept::types::GameDataType::ARRAY);
}

Rotator::~Rotator()
{

}



/*
 * @brief returns a value between 0 and 360 degrees
 * @author Killerswin2
 * @param angle float angle in degrees to be AxisClamped
 * @return float clampedAxis
 * @see Rotator::clampAxis for sqf cmd
 */
float Rotator::clampAxisInternal(float angle)
{
    angle = std::fmod(angle, 360);

    if (angle < 0)
    {
        angle = angle + 360.0f;
    }

    return angle;
}

/*
 * @brief returns a value between -180 and 180 degrees
 * @author Killerswin2
 * @param angle float angle in degrees to be normalized
 * @return float normalized angle
 * @see Rotator::normalizeAxis for sqf cmd
 */
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


/*
 * @brief clamps an angle in degrees between min angle and max angle.
 * @author Killerswin2
 * @param angle float current angle measurement
 * @param minAngle float min allowed angle to clamp to
 * @param maxAngle float max allowed angle to clamp to
 * @return float normalized angle
 * @see Rotator::clampAngle for sqf cmd
 * @warning coterminal angles are handled, 400 -> 40
 */
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


/*
* @brief clamps all components of a vec3 between 0 and 360
* @author Killerswin2
* @param x float x component
* @param y float y component
* @param z float z component
* @return vec3 clamped vec3
* @see Rotator::clamp for sqf cmd
*/
vector3 Rotator::clampInternal(float x, float y, float z)
{
    return vector3{clampAxis(x), clampAxis(y), clampAxis(z)};
}

/*
* @brief sqf wrapper for clamp
* @author Killerswin2
* @return VEC3
* @see Rotator::clampInternal for c++ implementation
*/
game_value Rotator::clamp(game_value_parameter right_arg)
{
    return clampInternal(right_arg[0], right_arg[1], right_arg[2]);
}

/*
 * @brief sqf wrapper for clampAngle
 * @author Killerswin2
 * @return SCALAR
 * @see Rotator::clampAngleInternal for c++ implementation
 */
game_value Rotator::clampAngle(game_value_parameter right_arg)
{
    return clampAngleInternal(right_arg[0], right_arg[1], right_arg[2]);
}

/*
 * @brief sqf wrapper for normalizeAxis
 * @author Killerswin2
 * @return SCALAR
 * @see Rotator::normalizeAxisInternal for c++ implementation
 */
game_value Rotator::normalizeAxis(game_value_parameter right_arg)
{
    return Rotator::normalizeAxisInternal(right_arg);
}

/*
 * @brief sqf wrapper for clampAxis
 * @author Killerswin2
 * @return SCALAR
 * @see Rotator::clampAxisInternal for c++ implementation
 */
game_value Rotator::clampAxis(game_value_parameter right_arg)
{
    return Rotator::clampAxisInternal(right_arg);
}