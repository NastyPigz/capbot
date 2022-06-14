// configuration file... including all the commands
#include "capbot/cmd.h"
#include "capbot/config.h"

// only allow 100 commands
// 25 subcommands

void initAllItems() {
    for (auto& el : shop_items.items()) {
        shop_items_default[el.key()] = 0;
    }
    for (auto& el : badge_items.items()) {
        badge_items_default[el.key()] = false;
    }
}

void ping(CmdCtx ctx, const dpp::slashcommand_t &ev) {
    auto start = std::chrono::high_resolution_clock::now();
    ev.thinking(true, [start, ev](const dpp::confirmation_callback_t &v) {
        auto stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = stop - start;
        ev.edit_response(dpp::message(fmt::format("Pong!\n**API Calc: **{}\n**API Real: **{}\n**WS: **{}", elapsed_seconds.count(), v.bot->rest_ping, ev.from->websocket_ping)));
    });
}

void uptime(CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.reply(ephmsg(ctx.bot.uptime().to_string()));
}

void help(CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.reply(ephmsg("ok"));
}

void think(CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(true);
}

void _register(CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(true, [ctx, ev](const dpp::confirmation_callback_t &v) {  
        ctx.db.post({
            {"key", std::to_string(ev.command.member.user_id)},
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

void unregister(CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(true, [ctx, ev](const dpp::confirmation_callback_t &v) {
        ctx.db.del(std::to_string(ev.command.usr.id), [ev](auto _) {
            ev.edit_response(ephmsg("Unregistered."));
        });
    });
}

void balance(CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(true, [ctx, ev](const dpp::confirmation_callback_t &v) {
        ctx.db.get(std::to_string(ev.command.usr.id), [ev](const dpp::http_request_completion_t &evt) {
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
                    FormatWithCommas(usr_data["bank"].get<int64_t>()) + "/" + FormatWithCommas(usr_data["bank_max"].get<int64_t>()),
                    true
                )
            ));
        });
    });
}