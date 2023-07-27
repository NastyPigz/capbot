#ifndef UTILS_H_
#define UTILS_H_

#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <map>
#include <fmt/format.h>

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
            // .add_embed(dpp::embed()
            //     .set_title(fmt::format("{}'s inventory", username))
            //     .set_description(usr_data["inv"].dump(4))
            // )
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

dpp::task<std::optional<json>> getbal(const Db &maindb, std::string user_id) {
    auto evt = co_await maindb.get(user_id);
    if (evt.status == 200) {
        co_return json::parse(evt.body);
    } else {
        co_return std::nullopt;
    }
}

uint32_t get_bank_colour(int type) {
    uint32_t colour = 0;
    switch (type) {
        case 0:
            colour =  dpp::colors::black;
            break;
        case 1:
            colour =  dpp::colors::red;
            break;
        case 2:
            colour = dpp::colors::vivid_violet;
            break;
        case 3:
            colour = dpp::colors::green;
            break;
        case 4:
            colour = dpp::colors::tahiti_gold;
            break;
    }
    return colour;
}

std::string get_bank_name(int type) {
    std::string name = "";
    switch (type) {
        case 0:
            name = "Default";
            break;
        case 1:
            name =  "Premium";
            break;
        case 2:
            name =  "Royal";
            break;
        case 3:
            name =  "Meme";
            break;
        case 4:
            name = "USSR";
            break;
    }
    return name;
}

dpp::task<void> find_use(const CmdCtx ctx, const dpp::slashcommand_t ev, int64_t amount, std::string user_id, std::string item, nlohmann::json pl) {
    if (item == "bank_space") {
        // have to implement bank colours first.
        if (pl["bank_type"].get<int64_t>() == 0) {
            co_return ev.edit_response(ephmsg("You cannot get more bankspace with default bank!"));
        }
        std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(25*amount, 50*amount);
		int amt = distr(gen);
        auto evt = co_await ctx.maindb.patch(
            user_id,
            {{"increment",
                {
                    {"inv." + item, -amount},
                    {"bank_max", amt}
                }
            }});
        if (evt.status == 200) {
            ev.edit_response(ephmsg(fmt::format("Used {} of {}, and got {} extra bank space.", amount, shop_items[item]["display"].get<std::string>(), amt)));
        } else {
            ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
            ev.edit_response(ephmsg("Something went wrong while using your item, try again later."));
        }
    } else if (item == "coin_bag") {
        std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(100*amount, 150*amount);
        int amt = distr(gen);
        auto evt = co_await ctx.maindb.patch(
            user_id,
            {{"increment",
                {
                    {"inv." + item, -amount},
                    {"bal", amt}
                }
            }});
         if (evt.status == 200) {
            ev.edit_response(ephmsg(fmt::format("Used {} of {}, and got {} coins from the bag.", amount, shop_items[item]["display"].get<std::string>(), amt)));
        } else {
            ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
            ev.edit_response(ephmsg("Something went wrong while using your item, try again later."));
        }
    } else if (item == "beef") {
        auto evt = co_await ctx.maindb.patch(
            user_id,
            {{"increment",
                {
                    {"inv." + item, -amount},
                    {"multi", amount}
                }
            }});
        if (evt.status == 200) {
            ev.edit_response(ephmsg(fmt::format("Used {} of {}, and got {}% multiplier for 1 minute.", amount, shop_items[item]["display"].get<std::string>(), amount)));
            ctx.bot.start_timer([ctx, user_id, item, amount](dpp::timer h) {
                ctx.maindb.patch(
                    user_id,
                    {{"increment",
                        {
                            {"multi", -amount}
                        }
                    }});
                // lucky bastard if this request failed
                ctx.bot.stop_timer(h);
            }, 60);
        } else {
            ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
            ev.edit_response(ephmsg("Something went wrong while using your item, try again later."));
        }
    } else if (item == "cursed_beef") {
        std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(1, 10);
        // it's actually 30% backfiring, we are going to say 40% to make it more intense
        if (distr(gen) < 4) {
            auto evt = co_await ctx.maindb.patch(
                user_id,
                {{"increment",
                    {
                        {"inv." + item, -1},
                    }
                }});
             if (evt.status == 200) {
                ev.edit_response("Ughhh!! Your balls are too itchy and you dropped your beef! Good luck next time.");
            } else {
                ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
                ev.edit_response(ephmsg("Something went wrong while using your item, try again later."));
            }
        }
        co_return ev.edit_response(
            dpp::message("Who would you like to curse?").set_flags(dpp::m_ephemeral).add_component(
                dpp::component().add_component(
                    dpp::component().set_placeholder("Select who to curse").
                        set_type(dpp::cot_user_selectmenu).
                        set_id("curse_1")
                )
            )
        );
    // horrose_celery is no longer usable starting from July 21st 2023
    // } else if (item == "horrorse_celery") {
    //     return ctx.maindb.patch(
    //         user_id,
    //         {{"increment",
    //             {
    //                 {"inv." + item, -amount},
    //                 {"multi", amount*100}
    //             }
    //         }},
    //         [ctx, ev, amount, item, user_id](const dpp::http_request_completion_t &evt) {
    //             if (evt.status == 200) {
    //                 ev.edit_response(ephmsg(fmt::format("Used {} of {}, and got {}% multiplier PERMANENTLY.", amount*100, shop_items[item]["display"].get<std::string>(), amount)));
    //             } else {
    //                 ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
    //                 ev.edit_response(ephmsg("Something went wrong while using your item, try again later."));
    //             }
    //         });
    } else {

    }
    ev.edit_response(ephmsg("ok"));
}
#endif