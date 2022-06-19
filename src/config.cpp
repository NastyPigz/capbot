// configuration file... including all the commands
#include "capbot/cmd.h"
#include "capbot/config.h"

// only allow 100 commands
// 25 subcommands

void init_all_items() {
    for (auto& el : shop_items.items()) {
        shop_items_default[el.key()] = 0;
    }
    for (auto& el : badge_items.items()) {
        badge_items_default[el.key()] = false;
    }
}

void ping(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    auto start = std::chrono::high_resolution_clock::now();
    ev.thinking(EPH_OR_NOT, [start, ev](const dpp::confirmation_callback_t &v) {
        auto stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = stop - start;
        ev.edit_response(dpp::message(fmt::format("Pong!\n**API Calc: **{}\n**API Real: **{}\n**WS: **{}", elapsed_seconds.count(), v.bot->rest_ping, ev.from->websocket_ping)));
    });
}

void uptime(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.reply(ephmsg(ctx.bot.uptime().to_string()));
}

void help(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.reply(ephmsg("ok"));
}

void think(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(EPH_OR_NOT);
}

void _register(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {  
        ctx.maindb.post({
            {"key", std::to_string(ev.command.usr.id)},
            {"bal", 0},
            {"bank", 0},
            {"bank_max", 5000},
            {"btc", 0},
            {"exp", 0},
            {"multi", 0},
            {"bank_type", 0},
            {"job", ""},
            {"inv", shop_items_default},
            {"badges", badge_items_default}
        }, [ev](const dpp::http_request_completion_t &evt) {
            if (evt.status == 409) {
                ev.edit_response(ephmsg("You already registered!"));
            } else {
                ev.edit_response(ephmsg("Registered."));
            }
        });
    });
}

void unregister(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
        ctx.maindb.del(std::to_string(ev.command.usr.id), [ev](auto _) {
            ev.edit_response(ephmsg("Unregistered."));
        });
    });
}

void balance(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
        if (ev.command.get_command_interaction().options.size() == 0) {
            ctx.maindb.get(std::to_string(ev.command.usr.id), [ev](const dpp::http_request_completion_t &evt) {
                if (evt.status == 200) {
                    json usr_data = json::parse(evt.body);
                    ev.edit_response(dpp::message().add_embed(dpp::embed()
                        .set_title(fmt::format("{}'s balance", ev.command.usr.username))
                        .add_field(
                            "Value",
                            usr_data.dump(4)
                        )
                        .add_field(
                            "Wallet",
                            FormatWithCommas(usr_data["bal"].get<int64_t>()),
                            true
                        )
                        .add_field(
                            "Bank",
                            FormatWithCommas(usr_data["bank"].get<int64_t>()) + " / " + FormatWithCommas(usr_data["bank_max"].get<int64_t>()),
                            true
                        )
                    ));
                } else {
                    ev.edit_response(ephmsg("You are not registered yet!"));
                }
            });
        } else {
            dpp::snowflake user_id = std::get<dpp::snowflake>(ev.get_parameter("user"));
            ctx.bot.user_get(user_id, [ctx, ev](const dpp::confirmation_callback_t &callback) {
                dpp::user_identified user = std::get<dpp::user_identified>(callback.value);
                std::string username = user.username;
                ctx.maindb.get(std::to_string(user.id), [ev, username](const dpp::http_request_completion_t &evt) {
                    if (evt.status == 200) {
                        json usr_data = json::parse(evt.body);
                        ev.edit_response(dpp::message().add_embed(dpp::embed()
                            .set_title(fmt::format("{}'s balance", username))
                            .add_field(
                                "Value",
                                usr_data.dump(4)
                            )
                            .add_field(
                                "Wallet",
                                FormatWithCommas(usr_data["bal"].get<int64_t>()),
                                true
                            )
                            .add_field(
                                "Bank",
                                FormatWithCommas(usr_data["bank"].get<int64_t>()) + " / " + FormatWithCommas(usr_data["bank_max"].get<int64_t>()),
                                true
                            )
                        ));
                    } else {
                        ev.edit_response(ephmsg(fmt::format("{} has not registered yet!", username)));
                    }
                });
            });
        } 
    });
}

void beg(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
        std::random_device rd;
        std::mt19937 gen(rd());
        // add multiplier later
        std::uniform_int_distribution<> distr(1, 10);
        int amt = distr(gen);
        ctx.maindb.patch(std::to_string(ev.command.usr.id), {
            {"increment", {
                {"bal", amt}
            }}
        }, [ev, amt](const dpp::http_request_completion_t &evt) {
            if (evt.status == 200) {
                ev.edit_response(ephmsg("").add_embed(dpp::embed()
                    .set_title("CapitalismBot walked out of a shop and gave you a few changes!")
                    .set_description(fmt::format("You received: {}", FormatWithCommas(amt)))
                ));
            } else {
                ev.edit_response(ephmsg("You are not registered yet!"));
            }
        });
    });
}

void deposit(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    // TODO: 
    // implement bank_color(s)
    ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
        int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
        if (amount <= 0 && amount != -1) {
            ev.edit_response(ephmsg("You cannot deposit \"no\" coins."));
            return;
        }
        ctx.maindb.get(std::to_string(ev.command.usr.id), [ctx, ev, amount](const dpp::http_request_completion_t &evt) {
            if (evt.status == 200) {
                json usr_data = json::parse(evt.body);
                int64_t damt = amount;
                int64_t cur_bal = usr_data["bal"].get<int64_t>();
                int64_t bank_left = usr_data["bank_max"].get<int64_t>() - usr_data["bank"].get<int64_t>();
                if (amount == -1) {
                    damt = cur_bal > bank_left ? bank_left : cur_bal;
                }
                if (cur_bal < damt) {
                    ev.edit_response(ephmsg("You do not have that much money in your wallet"));
                } else if (bank_left < damt) {
                    ev.edit_response(ephmsg("You cannot hold that much money"));
                } else if (cur_bal == 0) {
                    // prevent them from typing -1 and making a request
                    ev.edit_response(ephmsg("You are broke my guy"));
                } else {
                    ctx.maindb.patch(std::to_string(ev.command.usr.id), {
                        {"increment", {
                            {"bal", -damt},
                            {"bank", damt}
                        }}
                    }, [ev, damt](const dpp::http_request_completion_t &evt) {
                        if (evt.status == 200) {
                            ev.edit_response(ephmsg(fmt::format("{} deposited.", FormatWithCommas(damt))));
                        } else {
                            ev.edit_response(ephmsg("Something went wrong while trying to deposit your money."));
                        }
                    });
                }
            } else {
                ev.edit_response(ephmsg("You are not registered yet!"));
            }
        });
    });
}

void withdraw(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    // TODO:
    // implement bank color(s)
    ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
        int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
        if (amount <= 0 && amount != -1) {
            ctx.cooldowns.reset_trigger(ev.command.usr.id, "withdraw");
            ev.edit_response(ephmsg("You cannot withdraw \"no\" coins."));
            return;
        }
        ctx.maindb.get(std::to_string(ev.command.usr.id), [ctx, ev, amount](const dpp::http_request_completion_t &evt) {
            if (evt.status == 200) {
                json usr_data = json::parse(evt.body);
                int64_t wamt = amount;
                int64_t cur_bank = usr_data["bank"].get<int64_t>();
                if (amount == -1) {
                    wamt = cur_bank;
                }
                if (cur_bank < wamt) {
                    ev.edit_response(ephmsg("You do not have that much money in your bank"));
                } else if (cur_bank == 0) {
                    // prevent them from typing -1 and making a request
                    ev.edit_response(ephmsg("You bank is empty!"));
                } else {
                    ctx.maindb.patch(std::to_string(ev.command.usr.id), {
                        {"increment", {
                            {"bal", wamt},
                            {"bank", -wamt}
                        }}
                    }, [ev, wamt](const dpp::http_request_completion_t &evt) {
                        if (evt.status == 200) {
                            ev.edit_response(ephmsg(fmt::format("{} withdrawed.", FormatWithCommas(wamt))));
                        } else {
                            ev.edit_response(ephmsg("Something went wrong while trying to withdraw your money."));
                        }
                    });
                }
            } else {
                ev.edit_response(ephmsg("You are not registered yet!"));
            }
        });
    });
}

void fail(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
   // hehehehaw
}