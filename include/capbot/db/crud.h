#ifndef CRUD_H_
#define CRUD_H_
#include <iostream>
#include <vector>
#include <map>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <fmt/format.h>
// #define DEFAULT_FUNCTION [](auto e){}

// using handle_function = std::function<void(const dpp::http_request_completion_t&)>;

class Db {
    const std::string url;
    dpp::cluster* client;
    const std::multimap<std::string, std::string> headers;
public:
    Db(std::string project_key, std::string project_id, std::string base, dpp::cluster &client);
    
    dpp::task<dpp::http_request_completion_t> put(json items) const;

    dpp::task<dpp::http_request_completion_t> get(std::string key) const;

    dpp::task<dpp::http_request_completion_t> del(std::string key) const;

    dpp::task<dpp::http_request_completion_t> post(json item) const;

    dpp::task<dpp::http_request_completion_t> patch(std::string key, json item) const;
    
    dpp::task<dpp::http_request_completion_t> query(json query, unsigned int limit = 0, std::string last = "") const;
};
#endif