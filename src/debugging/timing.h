#include <chrono>
#include <intercept.hpp>

namespace Debug
{
	class Timing
	{
	private:
		static void fileOperationsInternal();
	public:
		Timing();
		~Timing();
		static game_value setStartTime();
		static game_value setEndTime();
		static game_value logTimeDifference();
	};
}