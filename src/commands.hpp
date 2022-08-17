#pragma once
#include <vector>
#include <intercept.hpp>

class Commands
{
private:
	std::vector<intercept::types::registered_sqf_function> m_addedCommands;

public:
	Commands();
	~Commands();
	Commands& operator=(const Commands&) = delete;
	Commands(const Commands&) = delete;

	void addCommand(std::string_view name, std::string_view description, WrapperFunctionNular function_, game_data_type return_arg_type);
	void addCommand(std::string_view name, std::string_view description, WrapperFunctionUnary function_, game_data_type return_arg_type, game_data_type right_arg_type);
	void addCommand(std::string_view name, std::string_view description, WrapperFunctionBinary function_, game_data_type return_arg_type, game_data_type left_arg_type, game_data_type right_arg_type);
};