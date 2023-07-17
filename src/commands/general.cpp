#include "capbot/commands/general.h"
#include "capbot/cmd.h"
#include "capbot/config.h"

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

void shutdown(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(EPH_OR_NOT, [ev, ctx](const dpp::confirmation_callback_t &v) {
        if (ev.command.usr.id == dpp::snowflake(763854419484999722)) {
            ctx.bot.shutdown();
            // delete ctx.bot;
            // ctx.bot = nullptr;
            ev.edit_response(ephmsg("I hate women"));
        } else {
            ev.edit_response(ephmsg(":nerd:"));
        }
    });
}

// void button(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
//     ev.reply(
//         dpp::message("this text has buttons").add_component(
//             dpp::component().add_component(
//                 dpp::component().set_label("Click me!").
//                 set_type(dpp::cot_button).
//                 set_emoji(u8"😄").
//                 set_style(dpp::cos_danger).
//                 set_id("myid")
//             )
//         )
//     );
// }