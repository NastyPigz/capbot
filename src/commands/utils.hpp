#ifndef UTILS_H_
#define UTILS_H_
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <map>

dpp::message getinv(const json usr_data, const std::string username, const std::string pronoun, const int page) {
    std::map<std::string, int64_t> inv; 
    for (auto const& [item, value]: usr_data["inv"].get<std::map<std::string, int64_t>>()) {
        if (value > 0) {
            inv[item] = value;
        }
    }
    if (inv.size() > 0) {
        int page_total = std::ceil(inv.size()/5.0);
        dpp::embed em;
        em.set_title(fmt::format("{}'s inventory", username));
        em.set_footer(dpp::embed_footer().set_text(fmt::format("page {} of {}", page, page_total)));
        if (page > page_total) {
            return ephmsg("Page number provided was over total amount of pages");
        }
        for (std::map<std::string, int64_t>::iterator it = inv.begin(); it != inv.end(); it++) {
            int items = std::distance(inv.begin(), it);
            if ((page-1)*5 > items) {
                continue;
            }
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
            if ((items+1) / page == 5) {
                break;
            }
        }
        return dpp::message()
            .add_embed(dpp::embed()
                .set_title(fmt::format("{}'s inventory", username))
                .set_description(usr_data["inv"].dump(4))
            )
            .add_embed(em);
    } else {
        return ephmsg(fmt::format("{}'re too poor to have anything", pronoun));
    }
}
#endif