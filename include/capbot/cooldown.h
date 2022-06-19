#ifndef COOLDOWN_H
#define COOLDOWN_H
#include <iostream>
#include <map>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>

class CooldownManager {
	std::shared_mutex cooldown_mutex;
	std::unordered_map<dpp::snowflake, std::unordered_map<std::string, time_t>> *cache_map;
public:
	CooldownManager();

	~CooldownManager();

	void trigger(dpp::snowflake id, std::string command_name);

	bool can_trigger(dpp::snowflake id, std::string command_name);

	int seconds_to_wait(dpp::snowflake id, std::string command_name);

	void reset_trigger(dpp::snowflake id, std::string command_name);
};
#endif