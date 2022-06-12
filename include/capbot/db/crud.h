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

class Database {
    std::string url;
    dpp::cluster* client;
    std::multimap<std::string, std::string> headers;
public:
    Database(std::string project_key, std::string project_id, std::string base, dpp::cluster &client): 
        url(fmt::format("https://database.deta.sh/v1/{}/{}/", project_id, base)),
        // url(fmt::format("http://localhost:8080/v1/{}/{}/", project_id, base)),
        client(&client) {
            headers.insert(std::pair<std::string, std::string>("X-API-Key", project_key));
        }
    
    void put(std::string items) {
        std::multimap<std::string, std::string> hdrs = headers;
        hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
        client->request(url+"items", dpp::http_method::m_put, [](const dpp::http_request_completion_t& e) {
            std::cout << e.status << e.body << '\n';
        }, items, "application/json", hdrs);
    }

    void get(std::string key) {
        client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_get, [](const dpp::http_request_completion_t& e) {
            std::cout << e.status << e.body << '\n';
        }, "", "", headers);
    }

    void del(std::string key) {
        client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_delete, [](const dpp::http_request_completion_t& e) {
            std::cout << e.status << e.body << '\n';
        }, "", "", headers);
    }

    void post(std::string item) {
        std::multimap<std::string, std::string> hdrs = headers;
        hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
        client->request(url+"items", dpp::http_method::m_post, [](const dpp::http_request_completion_t& e) {
            std::cout << e.status << e.body << '\n';
        }, item, "application/json", hdrs);
    }

    void patch(std::string key, std::string item) {
        std::multimap<std::string, std::string> hdrs = headers;
        hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
        client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_patch, [](const dpp::http_request_completion_t& e) {
            std::cout << e.status << e.body << '\n';
        }, item, "application/json", hdrs);
    }
    
    void query(std::string query) {
        std::multimap<std::string, std::string> hdrs = headers;
        hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
        client->request(url+"query", dpp::http_method::m_post, [](const dpp::http_request_completion_t& e) {
            std::cout << e.status << e.body << '\n';
        }, query, "application/json", hdrs);
    }
};
#endif