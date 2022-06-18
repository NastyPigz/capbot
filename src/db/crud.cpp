#include "capbot/db/crud.h"

// class Database

Db::Db(std::string project_key, std::string project_id, std::string base, dpp::cluster &client): 
    url(fmt::format("https://database.deta.sh/v1/{}/{}/", project_id, base)),
    client(&client),
    headers({
        {"X-API-Key", project_key}
    }) {}
    
void Db::put(json items, handle_function fn) const {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    client->request(url+"items", dpp::http_method::m_put, [fn](const dpp::http_request_completion_t& e) {
        fn(e);
        std::cout << "PUT/items " << e.status << ' ' << e.body << '\n';
    }, json({
        {"items", items}
    }).dump(), "application/json", hdrs);
}

void Db::get(std::string key, handle_function fn) const {
    client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_get, [fn, key](const dpp::http_request_completion_t& e) {
        fn(e);
        std::cout << fmt::format("GET/items/{} ", key) << e.status << ' ' << e.body << '\n';
    }, "", "", headers);
}

void Db::del(std::string key, handle_function fn) const {
    client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_delete, [fn, key](const dpp::http_request_completion_t& e) {
        fn(e);
        std::cout << fmt::format("DELETE/items/{} ", key) << e.status << ' ' << e.body << '\n';
    }, "", "", headers);
}

void Db::post(json item, handle_function fn) const {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    client->request(url+"items", dpp::http_method::m_post, [fn](const dpp::http_request_completion_t& e) {
        fn(e);
        std::cout << "POST/items " << e.status << ' ' << e.body << '\n';
    }, json({
        {"item", item}
    }).dump(), "application/json", hdrs);
}

void Db::patch(std::string key, json item, handle_function fn) const {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_patch, [fn, key](const dpp::http_request_completion_t& e) {
        fn(e);
        std::cout << fmt::format("PATCH/items/{} ", key) << e.status << ' ' << e.body << '\n';
    }, item.dump(), "application/json", hdrs);
}

void Db::query(json query, handle_function fn, unsigned int limit, std::string last) const {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    json pl = {
        {"query", query}
    };
    if (limit != 0) {
        pl["limit"] = limit;
    }
    if (last != "") {
        pl["last"] = last;
    }
    client->request(url+"query", dpp::http_method::m_post, [fn](const dpp::http_request_completion_t& e) {
        fn(e);
        std::cout << "POST/query " << e.status << ' ' << e.body << '\n';
    }, pl.dump(), "application/json", hdrs);
}