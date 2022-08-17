#include "commands.hpp"

Commands::~Commands()
{
}

void Commands::addCommand(std::string_view name, std::string_view description, intercept::types::nular_function function_, game_data_type return_arg_type)
{
	m_addedCommands.push_back(intercept::client::host::register_sqf_command(name, description, function_, return_arg_type));
}

void Commands::addCommand(std::string_view name, std::string_view description, intercept::types::unary_function function_, game_data_type return_arg_type, game_data_type right_arg_type)
{
	m_addedCommands.push_back(intercept::client::host::register_sqf_command(name, description, function_, return_arg_type, right_arg_type));
}

void Commands::addCommand(std::string_view name, std::string_view description, intercept::types::binary_function function_, game_data_type return_arg_type, game_data_type left_arg_type, game_data_type right_arg_type)
{
	m_addedCommands.push_back(intercept::client::host::register_sqf_command(name, description, function_, return_arg_type, left_arg_type, right_arg_type));
}

Commands Commands::s_Instance;
