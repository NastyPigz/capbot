#ifndef CRUD_H_
#define CRUD_H_
#include <iostream>
#include <vector>
#include <map>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <dpp/fmt/format.h>
#define DEFAULT_FUNCTION [](auto e){}

using handle_function = std::function<void(const dpp::http_request_completion_t&)>;

class Db {
    const std::string url;
    dpp::cluster* client;
    const std::multimap<std::string, std::string> headers;
public:
    Db(std::string project_key, std::string project_id, std::string base, dpp::cluster &client);
    
    void put(json items, handle_function fn = DEFAULT_FUNCTION) const;

    void get(std::string key, handle_function fn = DEFAULT_FUNCTION) const;

    void del(std::string key, handle_function fn = DEFAULT_FUNCTION) const;

    void post(json item, handle_function fn = DEFAULT_FUNCTION) const;

    void patch(std::string key, json item, handle_function fn = DEFAULT_FUNCTION) const;
    
    void query(json query, handle_function fn = DEFAULT_FUNCTION, unsigned int limit = 0, std::string last = "") const;
};
#endif