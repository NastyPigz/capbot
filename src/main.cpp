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
    // comment this out for faster recompile
    // ls_recursive("./src/commands");
    initAllItems();

	if (!getenv("DISCORD_TOKEN") || !getenv("PROJECT_KEY") || !getenv("PROJECT_ID")) {
		std::cout << "Could not find the DISCORD_TOKEN or PROJECT_KEY or PROJECT_ID environment variable.\n";
		return 1;
	}

    dpp::cluster bot(getenv("DISCORD_TOKEN"), dpp::i_default_intents | dpp::i_message_content);

    // Database db(getenv("PROJECT_KEY"), getenv("RPOJECT_ID"), "gnc", bot);
    std::string prokey, projid;
    prokey = getenv("PROJECT_KEY");
    projid = getenv("PROJECT_ID");
    Db db(prokey, projid, "gnc", bot);
    // DB Tests
    // db.put({
    //     {
    //         {"key", "bastard1"},
    //         {"field1", "value1"}
    //     },
    //     {
    //         {"key", "bastard2"},
    //         {"field1", "value2"}
    //     }
    // });
    // db.post({
    //     {"key", "bastard3"},
    //     {"field1", "value3"}
    // });
    // db.patch("bastard2", {
    //     {"set", {
    //         {"field1", "value4"}
    //     }}
    // });
    // db.get("bastard1");
    // db.get("bastard2");
    // db.query(R"(
    //     [
    //         {
    //             "field1": "value3"
    //         }
    //     ]
    // )"_json);
    // db.del("bastard1");
    // db.del("bastard2");
    // db.del("bastard3");

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

    bot.on_slashcommand([&bot, &db](const dpp::slashcommand_t& event) {
        std::string name = event.command.get_command_name();
        if (cmds.find(name) != cmds.end()) {
            cmds.at(name).function(CmdCtx{bot, db}, event);
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