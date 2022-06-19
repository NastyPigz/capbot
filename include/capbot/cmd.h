#ifndef CMD_H
#define CMD_H

#include <dpp/dpp.h>
#include <dpp/fmt/format.h>
#include <iostream>
#include "db/crud.h"
#include "cooldown.h"

struct CmdCtx {
    dpp::cluster &bot;
    const Db &maindb;
	CooldownManager &cooldowns;
};

using command_function = const std::function<void(const CmdCtx, const dpp::slashcommand_t &ev)>;

struct command_definition {
	const std::string description;
	const command_function function;
	const int cooldown;
	const std::vector<dpp::command_option> parameters = {};
};

dpp::message ephmsg(const std::string content);

#endif