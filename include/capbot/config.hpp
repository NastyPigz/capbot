#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <dpp/dpp.h>
#include <dpp/fmt/format.h>
#include <map>
#include <iostream>
#include "cmd.hpp"
// configuration file... including all the commands

void ping(dpp::cluster &bot, const dpp::slashcommand_t &ev);

void uptime(dpp::cluster &bot, const dpp::slashcommand_t &ev);

void help(dpp::cluster &bot, const dpp::slashcommand_t &ev);

void think(dpp::cluster &bot, const dpp::slashcommand_t &ev);

// max commands is 100, 25 subcommands, 25 subcommand groups
inline std::map<std::string, command_definition> cmds = {
    { "ping", { "A ping command", ping }},
    { "help", {
        "A help command", help , { 
            { dpp::command_option(dpp::co_string, "term", "Help term", false) },
        }
    }},
    { "uptime", { "An uptime command", uptime }},
    { "think", { "think forever (15 minutes)", think }},
};

#endif