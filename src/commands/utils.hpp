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
            int items_count = std::distance(inv.begin(), it);
            if ((page-1)*5 > items_count) {
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
            if ((items_count+1) / page == 5) {
                break;
            }
        }
        return std::make_tuple(ephmsg("")
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

dpp::message getshop(const int page) {
    std::map<std::string, json> shop; 
    for (auto const& [item, value]: shop_items.get<std::map<std::string, json>>()) {
        if (value["showing"].get<bool>()) {
            shop[item] = value;
        }
    }
    int page_total = std::ceil(shop.size()/5.0);
    dpp::embed em;
    em.set_title("CTC shop");
    em.set_color(dpp::colors::red);
    em.set_footer(dpp::embed_footer().set_text(fmt::format("page {} of {}", page, page_total)));
    if (page > page_total) {
        return ephmsg("Page number provided was over total amount of pages");
    }
    for (std::map<std::string, json>::iterator it = shop.begin(); it != shop.end(); it++) {
        int items_count = std::distance(shop.begin(), it);
        if ((page-1)*5 > items_count) {
            continue;
        }
        int64_t price = it->second["price"].get<int64_t>();
        em.add_field(
            it->second["display"],
            fmt::format("{} | {}", (price == 0 ? "-2147483648" : FormatWithCommas(price)), it->second["description"].get<std::string>())
        );
        if ((items_count+1) / page == 5) {
            break;
        }
    }
    return ephmsg("").add_embed(em);
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
            err();
        }
    });
}

uint32_t get_bank_colour(int type) {
    switch (type) {
        case 0:
            return dpp::colors::black;
        case 1:
            return dpp::colors::red;
        case 2:
            return dpp::colors::vivid_violet;
        case 3:
            return dpp::colors::green;
        case 4:
            return dpp::colors::tahiti_gold;
    }
}

std::string get_bank_name(int type) {
    switch (type) {
        case 0:
            return "Default";
        case 1:
            return "Premium";
        case 2:
            return "Royal";
        case 3:
            return "Meme";
        case 4:
            return "USSR";
    }
}

void find_use(std::string id, std::string item) {
    if (item == "bank_space") {
        // have to implement bank colours first.
    } else if (item == "coin_bag") {

    } else if (item == "beef") {

    } else if (item == "cursed_beef") {

    } else if (item == "horrorse_celery") {

    } else {

    }
}
#endif