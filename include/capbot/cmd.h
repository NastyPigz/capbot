#ifndef CMD_H
#define CMD_H

#include <dpp/dpp.h>
#include <dpp/fmt/format.h>
#include <iostream>
#include "db/crud.h"

struct CmdCtx {
    dpp::cluster &bot;
    Db &maindb;
};

using command_function = std::function<void(CmdCtx, const dpp::slashcommand_t &ev)>;

struct command_definition {
	std::string description;
	command_function function;
	int cooldown;
	std::vector<dpp::command_option> parameters = {};
};

dpp::message ephmsg(std::string content);

#endif