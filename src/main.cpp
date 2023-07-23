// capbot
#include <dpp/dpp.h>
#include <dpp/etf.h>
#include <fmt/format.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "capbot/cmd.h"
#include "capbot/config.h"
#include "capbot/cooldown.h"
#include "capbot/db/crud.h"
#include "capbot/essentials.h"
// webserver
#include "webserver/server.h"
// others
#include <thread>

// void ls_recursive(const std::filesystem::path& path) {
//     for(const auto& p: std::filesystem::recursive_directory_iterator(path))
//     {
//         if (!std::filesystem::is_directory(p)) {
//             std::cout << p.path().u8string().substr(6) << '\n';
//         }
//     }
// }

int main() {
	std::cout << "starting program..." << '\n';
	std::thread thread_obj([]() {
		try {
			auto const address = net::ip::make_address("0.0.0.0");
			unsigned short port = 5050;

			net::io_context ioc{1};

			tcp::acceptor acceptor{ioc, {address, port}};
			tcp::socket socket{ioc};
			http_server(acceptor, socket);
			std::cout << "starting server..." << '\n';
			ioc.run();
		} catch (std::exception const& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
	});

	dpp::snowflake testers[] = {763854419484999722, 719249148624502824, 705462078613356625, 1004621611225325688, 761451726997422110, 532798408340144148};

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

	int sec_left = 1;
	bot.start_timer(
		[&sec_left](dpp::timer timer) {
			// every 30 minutes
			if (sec_left == 60 * 30) {
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> rng(9500, 10500);
				exchange_rate = rng(gen);
				// if (rng(gen) == 0) {
				// 	std::uniform_int_distribution<> distr(10000, 20000);
				// 	exchange_rate = distr(gen);
				// } else {
				// 	std::uniform_int_distribution<> distr(40000, 60000);
				// 	exchange_rate = distr(gen);
				// }
				sec_left = 1;
			} else {
				sec_left = sec_left + 1;
			}
		},
		1);

	CooldownManager cooldowns;

	bot.on_log(dpp::utility::cout_logger());

	bot.on_ready([&bot](const dpp::ready_t& event) {
		bot.set_presence(dpp::presence(dpp::ps_dnd, dpp::at_game, "C++"));
		if (dpp::run_once<struct bulk_register>()) {
			std::vector<dpp::slashcommand> slash_commands;
			for (auto& def : cmds) {
				dpp::slashcommand c;
				c.set_name(def.first)
					.set_description(def.second.description)
					.set_application_id(bot.me.id)
					.set_dm_permission(true);
				for (const dpp::command_option& param : def.second.parameters) {
					if (param.name == "item") {
						dpp::command_option opt = param;
						for (auto& [key, _] : shop_items_default.get<std::map<std::string, int>>()) {
							// dpp::command_option_choice choice;
							std::string key_name = key;
							replace_all(key_name, "_", " ");
							// choice.name = key_name;
							// choice.value = key;
							opt.add_choice(dpp::command_option_choice(key_name, key));
						}
						c.add_option(opt);
					} else {
						c.add_option(param);
					}
				}
				slash_commands.push_back(c);
			}
			bot.global_bulk_command_create(slash_commands);
		}
		std::cout << "Logged in as " << bot.me.username << "!\n";
	});

	bot.on_slashcommand([&bot, &maindb, &cooldowns, &usersdb, &sec_left, &testers](const dpp::slashcommand_t& event) {
		// std::cout << sec_left << '\n';
		std::string name = event.command.get_command_name();
		if (cmds.find(name) != cmds.end()) {
			int wait_time = cooldowns.seconds_to_wait(event.command.usr.id, name);
			if (wait_time <= 0) {
				if (std::find(std::begin(testers), std::end(testers), event.command.usr.id) == std::end(testers)) {
					cooldowns.trigger(event.command.usr.id, name);
				}
				usersdb.patch(std::to_string(event.command.usr.id),
							  {{"increment", {{"cmds", 1}}}},
							  [&usersdb, event](
								  const dpp::http_request_completion_t& evt) {
								  if (evt.status == 404) {
									  usersdb.post({{"key", std::to_string(event.command.usr.id)}, {"cmds", 1}});
								  }
							  });
				maindb.patch(std::to_string(event.command.usr.id),
					{{"increment", {{"exp", 1}}}}
				);
				cmds.at(name).function(CmdCtx{bot, maindb, cooldowns, sec_left}, event);
			} else {
				event.reply(ephmsg(fmt::format("Woah, slow down! Next execution is in {}", wait_time)));
			}
		}
	});

	bot.on_message_create([&bot](const dpp::message_create_t& event) {
		if (event.msg.author.is_bot()) {
			return;
		}
		if (event.msg.content == "test") {
			bot.message_create(dpp::message(event.msg.channel_id, "Test success"));
		}
		if (event.msg.content.rfind("s!", 0) == 0 &&
			event.msg.content.length() > 2) {
			std::string content = event.msg.content.substr(2);
			std::stringstream ss(content);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> vstrings(begin, end);
			std::string cmd = vstrings[0];
			std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return std::tolower(c); });
			std::string s;
			vstrings.erase(vstrings.begin());
			join(vstrings, ',', s);

			// dbg print
			// std::copy(vstrings.begin(), vstrings.end(),
			// std::ostream_iterator<std::string>(std::cout, "\n")); if
			// (!find_commands(cmd, event.msg)) {
			bot.message_create(dpp::message(event.msg.channel_id, fmt::format("Prefix detected! Cmd: {} Args: (len = {}) {}", cmd, std::to_string(vstrings.size()), s)));
			// }
		}
	});

	bot.on_button_click([](const dpp::button_click_t & event) {
		// event.edit_original_response(ephmsg("Test123"));
        event.reply("You clicked: " + event.custom_id);
    });

	bot.on_select_click([&maindb, &bot](const dpp::select_click_t & event) {
        /* Select clicks are still interactions, and must be replied to in some form to
         * prevent the "this interaction has failed" message from Discord to the user.
         */
		if (event.custom_id == "curse_1") {
			return maindb.patch(std::to_string(event.command.usr.id),
					{{"increment",
						{
							{"inv.cursed_beef", -1},
						}
					}},
					[event, &maindb, &bot](const dpp::http_request_completion_t &evt) {
						if (evt.status == 200) {
							maindb.patch(event.values[0],
								{{"increment",
									{
										{"multi", -5}
									}
								}},
								[event, &maindb, &bot](const dpp::http_request_completion_t &evt) {
									if (evt.status == 200) {
										std::string user_id = event.values[0];
										dpp::message msg("");
										msg.content = fmt::format("Fuck you <@{}>! You are cursed.", user_id);
										msg.channel_id = event.command.channel_id;
										bot.message_create(msg);
										event.reply(dpp::ir_update_message, "Cursed");
										bot.start_timer([user_id, &maindb, &bot](dpp::timer h) {
											maindb.patch(
												user_id,
												{{"increment",
													{
														{"multi", 5}
													}
												}},
												[](const dpp::http_request_completion_t &evt) {
													// unlucky bastard if this request failed
												});
											bot.stop_timer(h);
										}, 30 * 60);
									} else {
										event.reply("Looks like god has decided to not curse this person!");
									}
								}
							);
						} else {
							event.reply("Sorry buddy but we failed to charge you!");
						}
					}
				);
		}
        event.reply("You clicked " + event.custom_id + " and chose: " + event.values[0]);
    });

	std::cout << "starting with version " << dpp::utility::version() << '\n';

	bot.start(false);
	return 0;
}