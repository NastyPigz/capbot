#ifndef UTILS_H_
#define UTILS_H_
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <map>

std::tuple<dpp::message, bool> getinv(json &usr_data, const std::string username, const std::string pronoun, const int page) {
    std::map<std::string, int64_t> inv; 
    for (auto const& [item, value]: usr_data["inv"].get<std::map<std::string, int64_t>>()) {
        if (value > 0) {
            inv[item] = value;
        }
    }
    if (inv.size() > 0) {
        int page_total = std::ceil(inv.size()/5.0);
        bool edit = false;
        dpp::embed em;
        em.set_title(fmt::format("{}'s inventory", username));
        em.set_footer(dpp::embed_footer().set_text(fmt::format("page {} of {}", page, page_total)));
        if (page > page_total) {
            return std::make_tuple(ephmsg("Page number provided was over total amount of pages"), false);
        }
        for (std::map<std::string, int64_t>::iterator it = inv.begin(); it != inv.end(); it++) {
            int items = std::distance(inv.begin(), it);
            if ((page-1)*5 > items) {
                continue;
            }
            if (shop_items.contains(it->first)) {
                std::string display_name = shop_items[it->first]["display"].get<std::string>();
                std::string names = "";
                if (shop_items[it->first]["name"].get<std::vector<std::string>>().size() > 2) {
                    names = fmt::format("`{}`, `{}`, `{}` ...", shop_items[it->first]["name"][0].get<std::string>(), shop_items[it->first]["name"][1].get<std::string>(), shop_items[it->first]["name"][2].get<std::string>());
                } else if (shop_items[it->first]["name"].get<std::vector<std::string>>().size() == 2) {
                    names = fmt::format("`{}`, `{}`", shop_items[it->first]["name"][0].get<std::string>(), shop_items[it->first]["name"][1].get<std::string>());
                } else {
                    names = fmt::format("`{}`", shop_items[it->first]["name"][0].get<std::string>());
                }
                em.add_field(
                    fmt::format("{} - {} owned", display_name, FormatWithCommas(it->second)),
                    fmt::format("*ID* {}", names)
                );
            } else {
                edit = true;
                usr_data["inv"].erase(it->first);
                em.add_field(
                    fmt::format("❓ {} - {} owned", it->first, FormatWithCommas(it->second)),
                    "*ID* `nullptr`"
                );
            }
            if ((items+1) / page == 5) {
                break;
            }
        }
        return std::make_tuple(dpp::message()
            .add_embed(dpp::embed()
                .set_title(fmt::format("{}'s inventory", username))
                .set_description(usr_data["inv"].dump(4))
            )
            .add_embed(em),
            edit
        );
    } else {
        return std::make_tuple(ephmsg(fmt::format("{}'re too poor to have anything", pronoun)), false);
    }
}

void chkinv(const Db &maindb, json inv, std::string id) {
    bool edit = false;
    for (auto &[key, _]: shop_items.items()) {
        if (!inv.contains(key)) {
            edit = true;
            inv[key] = 0;
        }
    }
    if (edit) {
        maindb.patch(id, {
            {"set", {
                {"inv", inv}
            }}
        });
    }
}

void getbal_then(const Db &maindb, std::string user_id, std::function<void(json)> callback, std::function<void()> err) {
    maindb.get(user_id, [callback, err](const dpp::http_request_completion_t &evt) {
        if (evt.status == 200) {
            callback(json::parse(evt.body));
        } else {
            std::cout << "getbal error: " << evt.body << '\n';
            err();
        }
    });
}
#endif