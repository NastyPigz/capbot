#include "capbot/db/crud.h"

std::string put_str(std::vector<json> items) {
    std::string result = "{\"items\":[";
    for (auto &i: items) {
        result += i.dump();
    }
    result += "]}";
    return result;
};

// class Database {
//     std::string url;
//     dpp::cluster* client;
//     std::multimap<std::string, std::string> headers;
// public:
//     Database(std::string project_key, std::string project_id, std::string base, dpp::cluster &client): 
//         url(fmt::format("https://database.deta.sh/v1/{}/{}/", project_id, base)),
//         client(&client) {
//             headers.insert(std::pair<std::string, std::string>("X-API-Key", project_key));
//         }
    
//     void put(std::string items) {
//         client->request(url+"items", dpp::http_method::m_put, [](const dpp::http_request_completion_t& e) {
//             std::cout << e.status << e.body << '\n';
//         }, items, "application/json", headers);
//     }

//     void get(std::string key) {
//         client->request(fmt::format(url+"items/{}", key), dpp::http_method::m_get, [](const dpp::http_request_completion_t& e) {
//             std::cout << e.status << e.body << '\n';
//         }, "", "", headers);
//     }
// };