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

game_value jsonHashConvertToGameType(nlohmann::json& object)
{
	switch (object.type())
	{
	case nlohmann::json::value_t::null:
		return {};
		break;
	case nlohmann::json::value_t::boolean:
		return object.get<bool>();
		break;
	case nlohmann::json::value_t::number_float:
		return object.get<float>();
		break;
	case nlohmann::json::value_t::number_integer:
		return object.get<float>();
		break;
	case nlohmann::json::value_t::number_unsigned:
		return object.get<float>();
		break;
	case nlohmann::json::value_t::object:
		return {};		// objects not supported right now
		break;
	case nlohmann::json::value_t::string:
		return object.get<std::string>();
		break;
	case nlohmann::json::value_t::array:
		// check what we have. boolean, float, or strings
		switch (object.at(0).type())
		{
		case nlohmann::json::value_t::boolean:
			return object.get<std::vector<bool>>();
			break;
		case nlohmann::json::value_t::number_float:
			return object.get<std::vector<float>>();
			break;
		case nlohmann::json::value_t::number_integer:
			return object.get<std::vector<float>>();
			break;
		case nlohmann::json::value_t::number_unsigned:
			return object.get<std::vector<float>>();
			break;
		case nlohmann::json::value_t::string:
			return object.get<std::vector<std::string>>();
			break;
		default:
			return {};	//ok WTF are you doing with Arma style json???
		}
		break;
	default:
		return {};
	}
}


game_value jsonHashGet(game_value_parameter hashmap, game_value_parameter key)
{
	// hashmap can't be nil or key can't be anything other than string.
	if (hashmap.is_nil() || key.type_enum() != game_data_type::STRING) return {};
	game_value retData;
	auto hashMapPointer = static_cast<JsonGameDataHashMap*>(hashmap.data.get());
	auto value = hashMapPointer->map.find(key);
	if (value != hashMapPointer->map.end())
	{
		return jsonHashConvertToGameType(value->second);
	}

	return {};
}


game_value jsonHashGetNested(game_value_parameter hashmap, game_value_parameter arrayData)
{
	//return if the hashmap is nil, or the array arg size is not bigger than 1
	if (hashmap.is_nil() || arrayData.size() < 1) { return {}; }
	if (arrayData.size() == 1) { return jsonHashGet(hashmap, arrayData[0]); }
	auto hashMapPointer = static_cast<JsonGameDataHashMap*>(hashmap.data.get());
	auto value = hashMapPointer->map.find(arrayData[0]);
	nlohmann::json jsonObject = value->second;
	nlohmann::json object = jsonObject; // need copy to do dumb nested get

	int i;
	for (i = 1; i < arrayData.size(); i++)
	{
		object = object[arrayData[i]];
	}

	std::ofstream MyFile;
	// logger stuff.... debug 
	MyFile.open("@A3A-intercept-plugin/json/log.txt", std::ios_base::app);
	MyFile << object << "\n";
	MyFile << jsonObject << "\n";

	return jsonHashConvertToGameType(object);
	
}

template<class UnaryFunction>
void recursive_iteration(const nlohmann::json& object, UnaryFunction func)
{
	for (auto& item : object)
	{
		if (item.is_structured())
		{
			if (item.is_array()) {
				recursive_iteration(item, func);
			}
			else if (item.is_object())
			{
				func(item);
			}
		}
		else
		{
			func(item);
		}
	}
}

game_value jsonHashIsIn(game_value_parameter key, game_value_parameter hashmap)
{
	if (hashmap.is_nil()) { return {}; }
	auto hashMapPointer = static_cast<JsonGameDataHashMap*>(hashmap.data.get());
	auto contain = hashMapPointer->map.find(key);
	if (contain != hashMapPointer->map.end()) { return true; }
	return false;
}

game_value jsonHashIsInNested(game_value_parameter key, game_value_parameter hashmap)
{
	if (hashmap.is_nil()) { return {}; }
	auto hashMapPointer = static_cast<JsonGameDataHashMap*>(hashmap.data.get());
	auto contain = hashMapPointer->map.find(key);
	if (contain != hashMapPointer->map.end())
	{
		return true; 
	}
	else
	{
		bool hashContains = false;
		for (auto& hashMapPairs : hashMapPointer->map)
		{
			if (hashMapPairs.second.contains(key))
			{
				return true;	//found in in the first level, no need to recurse
			}
			recursive_iteration(hashMapPairs.second, [&hashContains, &key](const nlohmann::json& object)
				{
					if (object.contains(key)) { hashContains = true; return; }
					
				});
		}
		return hashContains;
	}
}

game_value jsonHashKeyList(game_value_parameter hashmap)
{
	if (hashmap.is_nil()) { return 0; };
	auto hashMapPointer = static_cast<JsonGameDataHashMap*>(hashmap.data.get());
	std::vector<std::string> keyList;
	for (auto& hashMapPairs : hashMapPointer->map) {
		keyList.push_back(hashMapPairs.first);
	}

	return std::move(keyList);
}

game_value jsonHashMerge(game_value_parameter modifiedHashmap, game_value_parameter adderHashmap)
{
	if (modifiedHashmap.is_nil() || adderHashmap.is_nil()) { return {}; }
	auto modifiedhashMapPointer = static_cast<JsonGameDataHashMap*>(modifiedHashmap.data.get());
	auto adderhashMapPointer = static_cast<JsonGameDataHashMap*>(adderHashmap.data.get());
	modifiedhashMapPointer->map.merge(adderhashMapPointer->map);

	return {};
}

game_value jsonHashDelete(game_value_parameter hashmap, game_value_parameter key)
{
	if (hashmap.is_nil()) { return {}; }
	auto hashMapPointer = static_cast<JsonGameDataHashMap*>(hashmap.data.get());
	auto foundKey = hashMapPointer->map.find(key);
	if( foundKey != hashMapPointer->map.end()) { hashMapPointer->map.erase(foundKey); }

	return {};
}


game_value jsonHashGetOrDefault(game_value_parameter hashmap, game_value_parameter arrayData)
{
	if (hashmap.is_nil()) { return {}; }
	game_value data = jsonHashGet(hashmap, arrayData[0]);
	if (data.size() == 0)
	{
		return arrayData[1];
	}
	return data;
}

game_value jsonHashGetOrDefaultNested(game_value_parameter hashmap, game_value_parameter arrayData)
{
	if (hashmap.is_nil()) { return {}; }
	game_value data = jsonHashGetNested(hashmap, arrayData[0]);
	if (data.size() == 0)
	{
		return arrayData[1];
	}
	return data;
}

void hashMap::JsonHashMap::preStart()
{
	auto codeType = intercept::client::host::register_sqf_type("JSONHASHMAP"sv, "jsonHashMap"sv, "hashmap for json stuff", "jsonHashMap"sv, createJsonGameDataHashMap);
	JsonGameDataHashMap_type = codeType.second;

	Commands& commands = Commands::get();
	//commands.addCommand("+", "", userFunctionWrapper<>);
	//commands.addCommand("apply", "", userFunctionWrapper<>);
	commands.addCommand("count", "count key amount", userFunctionWrapper<jsonHashCount>, game_data_type::SCALAR, codeType.first);
	commands.addCommand("createJsonHashMap", "create a hashmap for json support", userFunctionWrapper<createJsonHashMap>, codeType.first);
	commands.addCommand("deleteAt", "", userFunctionWrapper<jsonHashDelete>, game_data_type::NOTHING, codeType.first, game_data_type::STRING);
	//commands.addCommand("forEach", "", userFunctionWrapper<>);
	commands.addCommand("get", "returns data from key", userFunctionWrapper<jsonHashGet>, game_data_type::ANY, codeType.first, game_data_type::STRING);
	commands.addCommand("getNested", "returns data from a nested object key", userFunctionWrapper<jsonHashGetNested>, game_data_type::ANY, codeType.first, game_data_type::ARRAY);
	commands.addCommand("getOrDefault", "returns data or default if not found", userFunctionWrapper<jsonHashGetOrDefault>, game_data_type::ANY, codeType.first, game_data_type::ARRAY);
	commands.addCommand("getOrDefaultNested", "returns nested data or default if not found", userFunctionWrapper<jsonHashGetOrDefaultNested>, game_data_type::ANY, codeType.first, game_data_type::ARRAY);
	commands.addCommand("in", "finds if key is in hashmap", userFunctionWrapper<jsonHashIsIn>, game_data_type::BOOL, game_data_type::STRING, codeType.first);
	commands.addCommand("inNested", "finds if key is in a nested hashmap", userFunctionWrapper<jsonHashIsInNested>, game_data_type::BOOL, game_data_type::STRING, codeType.first);
	//commands.addCommand("insert", "", userFunctionWrapper<>);
	commands.addCommand("keys", "list of json hashmap keys", userFunctionWrapper<jsonHashKeyList>, game_data_type::ARRAY, codeType.first);
	commands.addCommand("merge", "merges two json hashmaps", userFunctionWrapper<jsonHashMerge>, game_data_type::NOTHING, codeType.first, codeType.first);
	commands.addCommand("set", "set key value pair in hashmap", userFunctionWrapper<jsonHashSet>, game_data_type::NOTHING, codeType.first, game_data_type::STRING);
	commands.addCommand("setFromFile", "sets a hashMap from a json file", userFunctionWrapper<jsonHashSetFromFile>, game_data_type::NOTHING, codeType.first, game_data_type::STRING);
	//commands.addCommand("str", "", userFunctionWrapper<>);
	//commands.addCommand("toArray", "", userFunctionWrapper<>);
	//commands.addCommand("values", "", userFunctionWrapper<>);
}