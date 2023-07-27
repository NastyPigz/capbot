#include "capbot/db/crud.h"

// class Database

Db::Db(std::string project_key, std::string project_id, std::string base, dpp::cluster &client): 
    url(fmt::format("https://database.deta.sh/v1/{}/{}/", project_id, base)),
    client(&client),
    headers({
        {"X-API-Key", project_key}
    }) {}
    
dpp::task<dpp::http_request_completion_t> Db::put(json items) const {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    auto e = co_await client->co_request(url+"items", dpp::http_method::m_put, json({
        {"items", items}
    }).dump(), "application/json", hdrs);
    std::cout << "PUT/items " << e.status << ' ' << e.body << '\n';
    co_return e;
}

dpp::task<dpp::http_request_completion_t> Db::get(std::string key) const {
    auto e = co_await client->co_request(url+"items/"+key, dpp::http_method::m_get, "", "", headers);
    std::cout << fmt::format("GET/items/{} ", key) << e.status << ' ' << e.body << '\n';
    co_return e;
}

dpp::task<dpp::http_request_completion_t> Db::del(std::string key) const {
    auto e = co_await client->co_request(url+"items/{}"+key, dpp::http_method::m_delete, "", "", headers);
    std::cout << fmt::format("DELETE/items/{} ", key) << e.status << ' ' << e.body << '\n';
    co_return e;
}

dpp::task<dpp::http_request_completion_t> Db::post(json item) const {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    auto e = co_await client->co_request(url+"items", dpp::http_method::m_post, json({
        {"item", item}
    }).dump(), "application/json", hdrs);
    std::cout << "POST/items " << e.status << ' ' << e.body << '\n';
    co_return e;
}

dpp::task<dpp::http_request_completion_t> Db::patch(std::string key, json item) const {
    std::multimap<std::string, std::string> hdrs = headers;
    hdrs.insert(std::pair<std::string, std::string>("Content-Type", "application/json"));
    auto e = co_await client->co_request(url+"items/"+key, dpp::http_method::m_patch, item.dump(), "application/json", hdrs);
    std::cout << fmt::format("PATCH/items/{} ", key) << e.status << ' ' << e.body << '\n';
    co_return e;
}

dpp::task<dpp::http_request_completion_t> Db::query(json query, unsigned int limit, std::string last) const {
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
    auto e = co_await client->co_request(url+"query", dpp::http_method::m_post, pl.dump(), "application/json", hdrs);
    std::cout << "POST/query " << e.status << ' ' << e.body << '\n';   
    co_return e;
}