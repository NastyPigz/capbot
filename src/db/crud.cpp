#include "capbot/db/crud.h"

std::string put_str(std::vector<json> items) {
    std::string result = "{\"items\":[";
    for (auto &i: items) {
        result += i.dump();
    }
    result += "]}";
    return result;
};

// class Database

Db::Db(std::string project_key, std::string project_id, std::string base, dpp::cluster &client): 
    url(fmt::format("https://database.deta.sh/v1/{}/{}/", project_id, base)),
    client(&client)
{
    headers.insert(std::pair<std::string, std::string>("X-API-Key", project_key));
}
    
void Db::put(std::string items) {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    client->request(url+"items", dpp::http_method::m_put, [](const dpp::http_request_completion_t& e) {
        std::cout << e.status << e.body << '\n';
    }, items, "application/json", hdrs);
}

void Db::get(std::string key) {
    client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_get, [](const dpp::http_request_completion_t& e) {
        std::cout << e.status << e.body << '\n';
    }, "", "", headers);
}

void Db::del(std::string key) {
    client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_delete, [](const dpp::http_request_completion_t& e) {
        std::cout << e.status << e.body << '\n';
    }, "", "", headers);
}

void Db::post(std::string item) {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    client->request(url+"items", dpp::http_method::m_post, [](const dpp::http_request_completion_t& e) {
        std::cout << e.status << e.body << '\n';
    }, item, "application/json", hdrs);
}

void Db::patch(std::string key, std::string item) {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_patch, [](const dpp::http_request_completion_t& e) {
        std::cout << e.status << e.body << '\n';
    }, item, "application/json", hdrs);
}

void Db::query(std::string query) {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    client->request(url+"query", dpp::http_method::m_post, [](const dpp::http_request_completion_t& e) {
        std::cout << e.status << e.body << '\n';
    }, query, "application/json", hdrs);
}