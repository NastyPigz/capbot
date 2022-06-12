#ifndef CRUD_H
#define CRUD_H
// deta stuff here
// https://database.deta.sh/v1/{project_id}/{base_name}
// header: 'X-API-Key: a0abcyxz_aSecretValue'
// 'Content-Type: application/json'

// bot.request("https://database.deta.sh/v1/{project_id}/{base_name}", dpp::http_method::m_get, [](){}, "application/json", headers)
#include <iostream>
#include <vector>
#include <map>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <dpp/fmt/format.h>

std::string put_str(std::vector<json> items);

class Db {
    std::string url;
    dpp::cluster* client;
    std::multimap<std::string, std::string> headers;
public:
    Db(std::string project_key, std::string project_id, std::string base, dpp::cluster &client);
    
    void put(std::string items);

    void get(std::string key);

    void del(std::string key);

    void post(std::string item);

    void patch(std::string key, std::string item);
    
    void query(std::string query);
};
#endif