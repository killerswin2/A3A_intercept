#include "../vendors/nlohmann/json.hpp"
#include "jsonHashMap.hpp"
#include <intercept.hpp>
#include "../commands.hpp"
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <iostream>
#include <vector>


static sqf_script_type JsonGameDataHashMap_type;

class JsonGameDataHashMap : public game_data
{
public:
	JsonGameDataHashMap() {}

	void lastRefDeleted() const override { delete this; }
	const sqf_script_type& type() const override { return JsonGameDataHashMap_type; }
	~JsonGameDataHashMap() override {};

	// copied from Dedman
	bool get_as_bool() const override { return true; }
	float get_as_number() const override { return 0.f; }
	const r_string& get_as_string() const override { static r_string nm("jsonHashmap"sv); return nm; }
	game_data* copy() const override { return new JsonGameDataHashMap(*this); } //#TODO make sure this works
	r_string to_string() const override { return r_string("jsonHashMap"sv); }
	//virtual bool equals(const game_data*) const override; //#TODO isEqualTo on hashMaps would be quite nice I guess?
	const char* type_as_string() const override { return "jsonHashMap"; }
	bool is_nil() const override { return false; }
	bool can_serialize() override { return true; }//Setting this to false causes a fail in scheduled and global vars

	serialization_return serialize(param_archive& ar) override {
		game_data::serialize(ar);
		size_t entryCount;
		if (ar._isExporting) entryCount = map.size();
		return serialization_return::no_error;
	}
	std::unordered_map<std::string, nlohmann::json> map;
};

game_data* createJsonGameDataHashMap(param_archive* arch)
{
	auto data = new JsonGameDataHashMap();
	if (arch)
	{
		data->serialize(*arch);
	}
	return data;
}

game_value createJsonHashMap()
{
	return game_value(new JsonGameDataHashMap());
}

game_value jsonHashSetFromFile(game_value_parameter hashmap, game_value_parameter file)
{

	std::string stringPath = file;
	//@A3A-intercept-plugin/json/web_colors.json
	std::filesystem::path jsonFilePath(stringPath);

	//file error handling.
	if (auto status = std::filesystem::status(jsonFilePath);
		status.type() == std::filesystem::file_type::not_found || status.type() != std::filesystem::file_type::regular)
	{
		return {};
	}

	// open the json file.
	std::ifstream jsonFile(jsonFilePath);
	nlohmann::json jsonStuff = nlohmann::json::parse(jsonFile);

	std::ofstream MyFile;
	// logger stuff.... debug 
	MyFile.open("@A3A-intercept-plugin/json/log.txt", std::ios_base::app);
	MyFile << "called correctly\n";

	auto hashMapPointer = static_cast<JsonGameDataHashMap*>(hashmap.data.get());
	hashMapPointer->map = std::move(jsonStuff.get<std::unordered_map<std::string, nlohmann::json>>());

	//MyFile << hashMapPointer->map["colors"];

	return {};
}

game_value jsonHashSet(game_value_parameter hashmap, game_value_parameter jsonData)
{
	if (hashmap.is_nil()) { return {}; }
	std::string jsonString = jsonData;
	nlohmann::json value = nlohmann::json::parse(jsonString);
	std::unordered_map<std::string, nlohmann::json> jsonMapToMerge = value.get<std::unordered_map<std::string, nlohmann::json>>();

	static_cast<JsonGameDataHashMap*>(hashmap.data.get())->map.merge(jsonMapToMerge);

	return {};
}

game_value jsonHashCount(game_value_parameter hashmap)
{
	if (hashmap.is_nil()) return 0;
	return static_cast<JsonGameDataHashMap*>(hashmap.data.get())->map.size();
}

game_value jsonHashGet(game_value_parameter hashmap, game_value_parameter key)
{
	if (hashmap.is_nil()) return {};
	game_value retData;
	auto hashMapPointer = static_cast<JsonGameDataHashMap*>(hashmap.data.get());
	auto value = hashMapPointer->map.find(key);
	if (value != hashMapPointer->map.end())
	{

		switch (value->second.type())
		{
		case nlohmann::json::value_t::null:
			return {};
			break;
		case nlohmann::json::value_t::boolean:
			return value->second.get<bool>();
			break;
		case nlohmann::json::value_t::number_float:
			return value->second.get<float>();
			break;
		case nlohmann::json::value_t::number_integer:
			return value->second.get<float>();
			break;
		case nlohmann::json::value_t::number_unsigned:
			return value->second.get<float>();
			break;
		case nlohmann::json::value_t::object:
			return {};		// objects not supported right now
			break;
		case nlohmann::json::value_t::string:
			return value->second.get<std::string>();
			break;
		case nlohmann::json::value_t::array:
			// check what we have. boolean, float, or strings
			switch (value->second.at(0).type())
			{
			case nlohmann::json::value_t::boolean:
				return value->second.get<std::vector<bool>>();
				break;
			case nlohmann::json::value_t::number_float:
				return value->second.get<std::vector<float>>();
				break;
			case nlohmann::json::value_t::number_integer:
				return value->second.get<std::vector<float>>();
				break;
			case nlohmann::json::value_t::number_unsigned:
				return value->second.get<std::vector<float>>();
				break;
			case nlohmann::json::value_t::string:
				return value->second.get<std::vector<std::string>>();
				break;
			default:
				return {};	//ok WTF are you doing with Arma style json???
			}
			break;
		default:
			return {};
		}


	}

	return {};
}


void hashMap::JsonHashMap::preStart()
{
	auto codeType = intercept::client::host::register_sqf_type("JSONHASHMAP"sv, "jsonHashMap"sv, "hashmap for json stuff", "jsonHashMap"sv, createJsonGameDataHashMap);
	JsonGameDataHashMap_type = codeType.second;

	Commands& commands = Commands::get();
	//commands.addCommand("+", "", userFunctionWrapper<>);
	//commands.addCommand("apply", "", userFunctionWrapper<>);
	commands.addCommand("count", "", userFunctionWrapper<jsonHashCount>, game_data_type::SCALAR, codeType.first);
	commands.addCommand("createJsonHashMap", "", userFunctionWrapper<createJsonHashMap>, codeType.first);
	//commands.addCommand("deleteAt", "", userFunctionWrapper<>);
	//commands.addCommand("forEach", "", userFunctionWrapper<>);
	commands.addCommand("get", "returns data from key", userFunctionWrapper<jsonHashGet>, game_data_type::ANY, codeType.first, game_data_type::STRING);
	//commands.addCommand("getNested", "", userFunctionWrapper<jsonHashGet>, game_data_type::ANY, codeType.first, game_data_type::ARRAY);
	//commands.addCommand("getOrDefault", "", userFunctionWrapper<>, game_data_type::ANY, codeType.first, game_data_type::ARRAY);
	//commands.addCommand("hashValue", "", userFunctionWrapper<>);
	//commands.addCommand("in", "", userFunctionWrapper<>);
	//commands.addCommand("insert", "", userFunctionWrapper<>);
	//commands.addCommand("keys", "", userFunctionWrapper<>);
	//commands.addCommand("merge", "", userFunctionWrapper<>);
	commands.addCommand("set", "set key value pair in hashmap", userFunctionWrapper<jsonHashSet>, game_data_type::NOTHING, codeType.first, game_data_type::STRING);
	commands.addCommand("setFromFile", "sets a hashMap from a json file", userFunctionWrapper<jsonHashSetFromFile>, game_data_type::NOTHING, codeType.first, game_data_type::STRING);
	//commands.addCommand("toArray", "", userFunctionWrapper<>);
	//commands.addCommand("values", "", userFunctionWrapper<>);
}