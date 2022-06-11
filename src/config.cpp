// configuration file... including all the 
#include "capbot/cmd.h"
#include "capbot/config.h"

void ping(dpp::cluster &bot, const dpp::slashcommand_t &ev) {
    auto start = std::chrono::high_resolution_clock::now();
    ev.thinking(true, [&bot, start, ev] (auto v) {
        auto stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = stop - start;
        ev.edit_response(dpp::message(fmt::format("Pong!\n**Calc:**{}\n**Real:**{}", elapsed_seconds.count(), bot.rest_ping)));
    });
}

void uptime(dpp::cluster &bot, const dpp::slashcommand_t &ev) {
    ev.reply(ephmsg(bot.uptime().to_string()));
}

void help(dpp::cluster &bot, const dpp::slashcommand_t &ev) {
    ev.reply(ephmsg("ok"));
}

void think(dpp::cluster &bot, const dpp::slashcommand_t &ev) {
    ev.thinking(true);
}