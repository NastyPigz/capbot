#ifndef CMD_H_
#define CMD_H_

#include <dpp/dpp.h>
#include <fmt/format.h>
#include <iostream>
#include "db/crud.h"
#include "cooldown.h"


#if EPH_OR_NOT == false
#define ephmsg(content) dpp::message(content)
#else
#define ephmsg(content) dpp::message(content).set_flags(dpp::m_ephemeral)
#endif

// btc timer
inline int minute_left = 1;

struct CmdCtx {
    dpp::cluster &bot;
    const Db &maindb;
	CooldownManager &cooldowns;
};

using command_function = const std::function<dpp::task<void>(const CmdCtx, const dpp::slashcommand_t ev)>;

struct command_definition {
	const std::string description;
	const command_function function;
	const int cooldown;
	const std::vector<dpp::command_option> parameters = {};
};

// dpp::message ephmsg(const std::string content);

#endif