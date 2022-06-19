#include "capbot/config.h"
#include "capbot/cooldown.h"

CooldownManager::CooldownManager():
    cache_map(new std::unordered_map<dpp::snowflake, std::unordered_map<std::string, time_t>>) {}

CooldownManager::~CooldownManager()  {
    std::unique_lock l(cooldown_mutex);
    delete cache_map;
}

void CooldownManager::trigger(dpp::snowflake id, std::string command_name) {
    time_t tm = time(NULL);
    std::unique_lock l(cooldown_mutex);
    std::unordered_map<dpp::snowflake, std::unordered_map<std::string, time_t>>::const_iterator existing = cache_map->find(id);
    if (existing == cache_map->end()) {
        (*cache_map)[id] = {
            {command_name, tm}
        };
    } else {
        (*cache_map)[id][command_name] = tm;
    }
}

void CooldownManager::reset_trigger(dpp::snowflake id, std::string command_name) {
    // // this cannot error unless someone pass in smth stupid
    // time_t tm = time(NULL) - cmds.at(command_name).cooldown * 2;
    std::unique_lock l(cooldown_mutex);
    std::unordered_map<dpp::snowflake, std::unordered_map<std::string, time_t>>::const_iterator existing = cache_map->find(id);
    if (existing != cache_map->end()) {
        // don't reset cooldown if it doesn't exist. This is very unlikely to be called.
        (*cache_map)[id].erase(command_name);
    }
}

bool CooldownManager::can_trigger(dpp::snowflake id, std::string command_name) {
    std::shared_lock l(cooldown_mutex);
    std::unordered_map<dpp::snowflake, std::unordered_map<std::string, time_t>>::const_iterator existing = cache_map->find(id);
    if (existing == cache_map->end()) {
        return true;
    }
    time_t tm = time(NULL);
    if (existing->second.find(command_name) == existing->second.end()) {
        return true;
    } else if (existing->second.at(command_name) + cmds.at(command_name).cooldown < tm) {
        return true;
    }
    return false;
}

int CooldownManager::seconds_to_wait(dpp::snowflake id, std::string command_name) {
    std::shared_lock l(cooldown_mutex);
    std::unordered_map<dpp::snowflake, std::unordered_map<std::string, time_t>>::const_iterator existing = cache_map->find(id);
    if (existing == cache_map->end()) {
        return 0;
    } else if (existing->second.find(command_name) == existing->second.end()) {
        return 0;
    }
    return cmds.at(command_name).cooldown - (int) std::difftime(time(NULL), existing->second.at(command_name));
}