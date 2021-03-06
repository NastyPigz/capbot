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