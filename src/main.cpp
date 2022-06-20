#include <dpp/dpp.h>
#include <dpp/fmt/format.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <map>
#include <utility>
#include <chrono>
#include "capbot/cmd.h"
#include "capbot/config.h"
#include "capbot/db/crud.h"
#include "capbot/cooldown.h"

// void ls_recursive(const std::filesystem::path& path) {
//     for(const auto& p: std::filesystem::recursive_directory_iterator(path)) {
//         if (!std::filesystem::is_directory(p)) {
//             std::cout << p.path().u8string().substr(6) << '\n';
//         }
//     }
// }

void join(const std::vector<std::string>& v, char c, std::string& s) {

    s.clear();

    for (std::vector<std::string>::const_iterator p = v.begin(); p != v.end(); ++p) {
        s += *p;
        if (p != v.end() - 1)
            s += c;
    }
}

int main() {
    init_all_items();

	if (!getenv("DISCORD_TOKEN") || !getenv("PROJECT_KEY") || !getenv("PROJECT_ID")) {
		std::cout << "Could not find the DISCORD_TOKEN or PROJECT_KEY or PROJECT_ID environment variable.\n";
		return 1;
	}

    dpp::cluster bot(getenv("DISCORD_TOKEN"), dpp::i_default_intents | dpp::i_message_content);

    // Database db(getenv("PROJECT_KEY"), getenv("RPOJECT_ID"), "gnc", bot);
    std::string prokey, projid;
    prokey = getenv("PROJECT_KEY");
    projid = getenv("PROJECT_ID");
    Db maindb(prokey, projid, "currency", bot);
    Db usersdb(prokey, projid, "bot_users", bot);
    // db_tests(maindb);

    CooldownManager cooldowns;

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot](const dpp::ready_t & event) {
        bot.set_presence(dpp::presence(dpp::ps_dnd, dpp::at_game, "c++"));
        if (dpp::run_once<struct bulk_register>()) {
			std::vector<dpp::slashcommand> slash_commands;
			for (auto & def : cmds) {
				dpp::slashcommand c;
				c.set_name(def.first).
				  set_description(def.second.description).
				  set_application_id(bot.me.id).
                  set_dm_permission(true);
				c.options = def.second.parameters;
				slash_commands.push_back(c);
			}
			bot.global_bulk_command_create(slash_commands);
		}
        std::cout << "Logged in as " << bot.me.username << "!\n";
    });

    bot.on_slashcommand([&bot, &maindb, &cooldowns, &usersdb](const dpp::slashcommand_t& event) {
        std::string name = event.command.get_command_name();
        if (cmds.find(name) != cmds.end()) {
            int wait_time = cooldowns.seconds_to_wait(event.command.usr.id, name);
            if (wait_time <= 0) {
                cooldowns.trigger(event.command.usr.id, name);
                usersdb.patch(std::to_string(event.command.usr.id), {
                    {"increment", {
                        {"cmds", 1}
                    }}
                }, [&usersdb, event](const dpp::http_request_completion_t &evt) {
                    if (evt.status == 404) {
                        usersdb.post({
                            {"key", std::to_string(event.command.usr.id)},
                            {"cmds", 1}
                        });
                    }
                });
                cmds.at(name).function(CmdCtx{bot, maindb, cooldowns}, event);
            } else {
                event.reply(ephmsg(fmt::format("Woah, slow down! Next execution is in {}", wait_time)));
            }
        }
    });

    bot.on_message_create([&bot](const dpp::message_create_t & event) {
        if (event.msg.author.is_bot()) {
            return;
        }
        if (event.msg.content == "test") {
            bot.message_create(dpp::message(event.msg.channel_id, "Test success"));
        }
        if (event.msg.content.rfind("s!", 0) == 0 && event.msg.content.length() > 2) {
            std::string content = event.msg.content.substr(2);
            std::stringstream ss(content);
            std::istream_iterator<std::string> begin(ss);
            std::istream_iterator<std::string> end;
            std::vector<std::string> vstrings(begin, end);
            std::string cmd = vstrings[0];
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c){ return std::tolower(c); });
            std::string s;
            vstrings.erase(vstrings.begin());
            join(vstrings, ',', s);
            
            // dbg print
            // std::copy(vstrings.begin(), vstrings.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
            // if (!find_commands(cmd, event.msg)) {
               bot.message_create(dpp::message(event.msg.channel_id, fmt::format("Prefix detected! Cmd: {} Args: (len = {}) {}", cmd, std::to_string(vstrings.size()), s)));
            // }
        }
    });
 
    std::cout << "starting..." << '\n';
    bot.start(false);
    return 0;
}