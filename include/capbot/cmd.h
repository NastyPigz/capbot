#ifndef CMD_H
#define CMD_H

#include <dpp/dpp.h>
#include <dpp/fmt/format.h>
#include <iostream>

using command_function = std::function<void(dpp::cluster&, const dpp::slashcommand_t&)>;

struct command_definition {
	std::string description;
	command_function function;
	std::vector<dpp::command_option> parameters = {};
};

dpp::message ephmsg(std::string content);
#endif