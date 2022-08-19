#include <intercept.hpp>

class Rotator
{
public:
	Rotator();
	~Rotator();

	static float clampAxisInternal(float angle);
	static float normalizeAxisInternal(float angle);
	static float clampAngleInternal(float angle, float minAngle, float maxAngle);
	static vector3 clampInternal(float, float, float);
	static game_value clampAxis(game_value_parameter right_arg);
	static game_value normalizeAxis(game_value_parameter right_arg);
	static game_value clampAngle(game_value_parameter right_arg);
	static game_value clamp(game_value_parameter right_arg);

private:

};
