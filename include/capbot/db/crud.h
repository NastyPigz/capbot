#ifndef CRUD_H
#define CRUD_H
#include <iostream>
#include <vector>
#include <map>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <dpp/fmt/format.h>

using handle_function = std::function<void(const dpp::http_request_completion_t&)>;

// void dummy_handle_function(const dpp::http_request_completion_t &ev) {
//     return;
// }

class Db {
    std::string url;
    dpp::cluster* client;
    std::multimap<std::string, std::string> headers;
public:
    Db(std::string project_key, std::string project_id, std::string base, dpp::cluster &client);
    
    void put(json items, handle_function fn = [](auto e){});

    void get(std::string key, handle_function fn = [](auto e){});

    void del(std::string key, handle_function fn = [](auto e){});

    void post(json item, handle_function fn = [](auto e){});

    void patch(std::string key, json item, handle_function fn = [](auto e){});
    
    void query(json query, handle_function fn = [](auto e){}, unsigned int limit = 0, std::string last = "");
};
#endif