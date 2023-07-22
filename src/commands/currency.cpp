#include "capbot/commands/currency.h"

#include "capbot/cmd.h"
#include "capbot/config.h"
#include "capbot/essentials.h"
#include "utils.hpp"

void _register(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		ctx.maindb.post(
			{{"key", std::to_string(ev.command.usr.id)},
			 {"bal", 0},
			 {"bank", 0},
			 {"bank_max", 5000},
			 {"btc", 0},
			 {"exp", 0},
			 {"multi", 0},
			 {"bank_type", 0},
			 {"job", ""},
			 {"inv", shop_items_default},
			 {"badges", badge_items_default}},
			[ev](const dpp::http_request_completion_t &evt) {
				if (evt.status == 409) {
					ev.edit_response(ephmsg("You already registered!"));
				} else {
					ev.edit_response(ephmsg("Registered."));
				}
			});
	});
}

void unregister(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		ctx.maindb.del(std::to_string(ev.command.usr.id), [ev](auto _) {
			ev.edit_response(ephmsg("Unregistered."));
		});
	});
}

void balance(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		if (ev.command.get_command_interaction().options.size() == 0) {
			getbal_then(
				ctx.maindb, std::to_string(ev.command.usr.id),
				[ev](json usr_data) {
					ev.edit_response(dpp::message().add_embed(
						dpp::embed()
							.set_title(fmt::format("{}'s balance", ev.command.usr.username))
							// .add_field("Value", usr_data.dump(4))
							.add_field("Wallet", FormatWithCommas(usr_data["bal"].get<int64_t>()), true)
							.add_field(
								"Bank",
								FormatWithCommas(usr_data["bank"].get<int64_t>()) + " / " + FormatWithCommas(usr_data["bank_max"].get<int64_t>()),
								true
                            )));
				},
				[ctx, ev]() {
					ctx.cooldowns.reset_trigger(ev.command.usr.id, "balance");
					ev.edit_response(ephmsg("You are not registered yet!"));
				});
		} else {
			dpp::snowflake user_id = std::get<dpp::snowflake>(ev.get_parameter("user"));
			ctx.bot.user_get(user_id, [ctx, ev](const dpp::confirmation_callback_t &callback) {
				dpp::user_identified user = std::get<dpp::user_identified>(callback.value);
				std::string username = user.username;
				getbal_then(
					ctx.maindb, std::to_string(user.id),
					[ev, username](json usr_data) {
						ev.edit_response(dpp::message().add_embed(
							dpp::embed()
								.set_title(fmt::format("{}'s balance", username))
								// .add_field("Value", usr_data.dump(4))
								.add_field("Wallet", FormatWithCommas(usr_data["bal"].get<int64_t>()), true)
								.add_field(
									"Bank",
									FormatWithCommas(usr_data["bank"].get<int64_t>()) + " / " + FormatWithCommas(usr_data["bank_max"].get<int64_t>()),
									true
                                )));
					},
					[ctx, ev, username]() {
						ctx.cooldowns.reset_trigger(ev.command.usr.id, "balance");
						ev.edit_response(ephmsg(fmt::format("{} has not registered yet!", username)));
					});
			});
		}
	});
}

void beg(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ev, ctx](json pl) {
				std::string user_id = std::to_string(ev.command.usr.id);
				int level = pl["exp"].get<int64_t>() / 100;
				// maximum 1000 levels worth of multiplier, which is 20%
				level = std::min(level, 1000);
				int multi = pl["multi"].get<int64_t>() + level / 50;
				// maximum 100% multiplier
				multi = std::min(multi, 100);
				if (pl["multi"].get<int64_t>() < 0) {
					// negative multiplier glitch bec of cursed beef
					ctx.maindb.patch(user_id, {
						{"set", {
							{"multi", 0}
						}}
					});
					multi = 0;
				}
				multi += pl["inv"]["horrorse_celery"].get<int64_t>() * 100;
				std::random_device rd;
				std::mt19937 gen(rd());
				// add multiplier later
				std::uniform_int_distribution<> distr(1, 10);
				int amt = distr(gen) * (1 + multi / 100);
				ctx.maindb.patch(
					std::to_string(ev.command.usr.id), {{"increment", {{"bal", amt}}}},
					[ctx, ev, amt](const dpp::http_request_completion_t &evt) {
						if (evt.status == 200) {
							ev.edit_response(ephmsg("").add_embed(
								dpp::embed()
									.set_title("CapitalismBot walked out of a shop and gave you a few changes!")
									.set_description(fmt::format("You received: {}", FormatWithCommas(amt)))));
						} else {
							ctx.cooldowns.reset_trigger(ev.command.usr.id, "beg");
							ev.edit_response(ephmsg("You are not registered yet!"));
						}
					});
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "bank");
				ev.edit_response(ephmsg("You has not registered yet!"));
			});
	});
}

void mine(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ev, ctx](json pl) {
				std::string user_id = std::to_string(ev.command.usr.id);
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distr(1, 5);
				// 20% dying but we're gonna say 40% to increase tension
				if (distr(gen) < 2 && user_id != "705462078613356625") {
					if (pl["inv"]["puppy_pass"].get<int64_t>() > 0) {
						return ev.edit_response("You almost died... But puppy pass saved you!");
					}
					if (pl["inv"]["capitalism_blessing"].get<int64_t>() > 0) {
						return ctx.maindb.patch(
							std::to_string(ev.command.usr.id), {
								{"increment", {
									{"inv.capitalism_blessing", -1}
								}}
							},
							[ctx, ev](const dpp::http_request_completion_t &evt) {
								if (evt.status == 200) {
									ev.edit_response(ephmsg("You almost died... But 1x capitalism blessing saved your life!"));
								} else {
									ctx.cooldowns.reset_trigger(ev.command.usr.id, "mine");
									ev.edit_response(ephmsg("You are not registered yet!"));
								}
						});
					}
					return ctx.maindb.patch(
						std::to_string(ev.command.usr.id), {{"set", {{"bal", 0}}}},
						[ctx, ev](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg("").add_embed(
									dpp::embed()
										.set_title("You died!")
										.set_description("You lost all your money.")
										.set_color(dpp::colors::red)));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "mine");
								ev.edit_response(ephmsg("You are not registered yet!"));
							}
						});
				}
				int level = pl["exp"].get<int64_t>() / 100;
				// maximum 1000 levels worth of multiplier, which is 20%
				level = std::min(level, 1000);
				int multi = pl["multi"].get<int64_t>() + level / 50;
				// maximum 100% multiplier
				multi = std::min(multi, 100);
				if (pl["multi"].get<int64_t>() < 0) {
					// negative multiplier glitch bec of cursed beef
					ctx.maindb.patch(user_id, {
						{"set", {
							{"multi", 0}
						}}
					});
					multi = 0;
				}
				multi += pl["inv"]["horrorse_celery"].get<int64_t>() * 100;
				// add multiplier later
				std::uniform_int_distribution<> rng(100, 200);
				int amt = rng(gen) * (1 + multi / 100);
				ctx.maindb.patch(
					std::to_string(ev.command.usr.id), {{"increment", {{"bal", amt}}}},
					[ctx, ev, amt](const dpp::http_request_completion_t &evt) {
						if (evt.status == 200) {
							ev.edit_response(ephmsg("").add_embed(
								dpp::embed()
									.set_title("CapitalismBot walked out of a shop and gave you a few changes!")
									.set_description(fmt::format("You received: {}", FormatWithCommas(amt)))));
						} else {
							ctx.cooldowns.reset_trigger(ev.command.usr.id, "beg");
							ev.edit_response(ephmsg("You are not registered yet!"));
						}
					});
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "bank");
				ev.edit_response(ephmsg("You has not registered yet!"));
			});
	});
}

void bank(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ev](json usr_data) {
				ev.edit_response(dpp::message().add_embed(
					dpp::embed()
						.set_title(fmt::format("{}'s bank data", ev.command.usr.username))
						.set_description(fmt::format("{} | {}\nBank Company: {}",
							FormatWithCommas(usr_data["bank"].get<int64_t>()),
							FormatWithCommas(usr_data["bank_max"].get<int64_t>()),
							get_bank_name(usr_data["bank_type"].get<int64_t>())
						))
						.set_color(get_bank_colour(usr_data["bank_type"].get<int64_t>()))));
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "bank");
				ev.edit_response(ephmsg("You has not registered yet!"));
			});
	});
}

void banks(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ev](const dpp::confirmation_callback_t &v) {
		int64_t type = -1;
		if (std::holds_alternative<int64_t>(ev.get_parameter("type"))) {
			type = std::get<int64_t>(ev.get_parameter("type"));
		}
		switch (type) {
			case -1:
				ev.edit_response(ephmsg("")
					.add_embed(
						dpp::embed()
							.set_title("Banks")
							.set_description("Available banks are: `default`, `premium`, `royal`, `meme`, `USSR`")
					));
				break;
			case 0:
				ev.edit_response(ephmsg("")
					.add_embed(
						dpp::embed()
							.set_title("Default")
							.set_description("Free of charge but you cannot get more space.")
					));
				break;
			case 1:
				ev.edit_response(ephmsg("")
					.add_embed(
						dpp::embed()
							.set_title("Premium")
							.set_description("Pay 5,000 to unlock. Every deposit must be lower than 200.")
					));
				break;
			case 2:
				ev.edit_response(ephmsg("")
					.add_embed(
						dpp::embed()
							.set_title("Royal")
							.set_description("Pay 25,000 to unlock. Every deposiit and withdraw must be higher than 2500.")
					));
				break;
			case 3:
				ev.edit_response(ephmsg("")
					.add_embed(
						dpp::embed()
							.set_title("Meme")
							.set_description("Pay 20,000 to unlock. Every deposit there's a 5% chance you will get trolled and lose the amount.")
					));
				break;
			case 4:
				ev.edit_response(ephmsg("")
					.add_embed(
						dpp::embed()
							.set_title("USSR")
							.set_description("Free or charge, but you share half of your money each deposit.")
					));
				break;
		}
	});
}

void deposit(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	// TODO:
	// implement bank_color(s)
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
		if (amount <= 0 && amount != -1) {
			ctx.cooldowns.reset_trigger(ev.command.usr.id, "deposit");
			ev.edit_response(ephmsg("You cannot deposit \"no\" coins."));
			return;
		}
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ctx, ev, amount](json usr_data) {
				int64_t damt = amount;
				int64_t cur_bal = usr_data["bal"].get<int64_t>();
				int64_t bank_left = usr_data["bank_max"].get<int64_t>() - usr_data["bank"].get<int64_t>();
				if (amount == -1) {
					damt = cur_bal > bank_left ? bank_left : cur_bal;
				}
				// cooldown will not be reset because it's their fault for invalid input
				if (cur_bal < damt) {
					// ctx.cooldowns.reset_trigger(ev.command.usr.id, "deposit");
					ev.edit_response(ephmsg("You do not have that much money in your wallet"));
				} else if (bank_left < damt) {
					// ctx.cooldowns.reset_trigger(ev.command.usr.id, "deposit");
					ev.edit_response(ephmsg("You cannot hold that much money"));
				} else if (cur_bal == 0) {
					// prevent them from typing -1 and making a request
					// ctx.cooldowns.reset_trigger(ev.command.usr.id, "deposit");
					ev.edit_response(ephmsg("You are broke my guy"));
				} else {
					ctx.maindb.patch(
						std::to_string(ev.command.usr.id),
						{{"increment", {{"bal", -damt}, {"bank", damt}}}},
						[ctx, ev,
						 damt](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg(fmt::format("{} deposited.", FormatWithCommas(damt))));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "deposit");
								ev.edit_response(ephmsg("Something went wrong while trying to deposit your money."));
							}
						});
				}
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "deposit");
				ev.edit_response(ephmsg("You are not registered yet!"));
			});
	});
}

void withdraw(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	// TODO:
	// implement bank color(s)
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
		if (amount <= 0 && amount != -1) {
			ctx.cooldowns.reset_trigger(ev.command.usr.id, "withdraw");
			ev.edit_response(ephmsg("You cannot withdraw \"no\" coins."));
			return;
		}
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ctx, ev, amount](json usr_data) {
				int64_t wamt = amount;
				int64_t cur_bank = usr_data["bank"].get<int64_t>();
				if (amount == -1) {
					wamt = cur_bank;
				}
				if (cur_bank < wamt) {
					ev.edit_response(ephmsg("You do not have that much money in your bank"));
				} else if (cur_bank == 0) {
					// prevent them from typing -1 and making a request
					ev.edit_response(ephmsg("You bank is empty!"));
				} else {
					ctx.maindb.patch(
						std::to_string(ev.command.usr.id),
						{{"increment", {{"bal", wamt}, {"bank", -wamt}}}},
						[ctx, ev,
						 wamt](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg(fmt::format("{} withdrawed.", FormatWithCommas(wamt))));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "withdraw");
								ev.edit_response(ephmsg("Something went wrong while trying to withdraw your money."));
							}
						});
				}
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "withdraw");
				ev.edit_response(ephmsg("You are not registered yet!"));
			});
	});
}

void inventory(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		if (std::holds_alternative<int64_t>(ev.get_parameter("page"))) {
			if (std::get<int64_t>(ev.get_parameter("page")) < 1) {
				ev.edit_response(ephmsg("Page number must be above 0"));
				return;
			}
		}
		if (std::holds_alternative<dpp::snowflake>(ev.get_parameter("user"))) {
			dpp::snowflake user_id = std::get<dpp::snowflake>(ev.get_parameter("user"));
			ctx.bot.user_get(
				user_id, [ctx, ev, user_id](const dpp::confirmation_callback_t &callback) {
					dpp::user_identified user = std::get<dpp::user_identified>(callback.value);
					std::string username = user.username;
					getbal_then(
						ctx.maindb, std::to_string(user.id),
						[ctx, ev, username, user_id](json usr_data) {
							int page = 1;
							if (std::holds_alternative<int64_t>(ev.get_parameter("page"))) {
								page = std::get<int64_t>(ev.get_parameter("page"));
							}
							auto [msg, b] = getinv(usr_data, username, "they", page);
							ev.edit_response(msg);
							if (b) {
								ctx.maindb.patch(std::to_string(user_id), {{"set", {{"inv", usr_data["inv"]}}}});
							}
							chkinv(ctx.maindb, usr_data["inv"], std::to_string(user_id));
						},
						[ctx, ev, username]() {
							ctx.cooldowns.reset_trigger(ev.command.usr.id,"inventory");
							ev.edit_response(ephmsg(fmt::format("{} has not registered yet!", username)));
						});
				});
		} else {
			std::string username = ev.command.usr.username;
			getbal_then(
				ctx.maindb, std::to_string(ev.command.usr.id),
				[ctx, ev, username](json usr_data) {
					int page = 1;
					if (std::holds_alternative<int64_t>(ev.get_parameter("page"))) {
						page = std::get<int64_t>(ev.get_parameter("page"));
					}
					auto [msg, b] = getinv(usr_data, username, "you", page);
					ev.edit_response(msg);
					if (b) {
						ctx.maindb.patch(std::to_string(ev.command.usr.id), {{"set", {{"inv", usr_data["inv"]}}}});
					}
					chkinv(ctx.maindb, usr_data["inv"], std::to_string(ev.command.usr.id));
				},
				[ctx, ev]() {
					ctx.cooldowns.reset_trigger(ev.command.usr.id, "inventory");
					ev.edit_response(ephmsg("You have not registered yet!"));
				});
		}
	});
}

void give_items(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		dpp::snowflake author_id = ev.command.usr.id;
		if (author_id != dpp::snowflake(763854419484999722)) {
			return ev.edit_response(":nerd:");
		}
		if (std::holds_alternative<dpp::snowflake>(ev.get_parameter("user"))) {
			dpp::snowflake user_id = std::get<dpp::snowflake>(ev.get_parameter("user"));
			ctx.bot.user_get(
				user_id,
				[ctx, ev](const dpp::confirmation_callback_t &callback) {
					dpp::user_identified user =std::get<dpp::user_identified>(callback.value);
					std::string username = user.username;
					ctx.maindb.patch(
						std::to_string(user.id),
						{{"increment",
						  {{fmt::format("inv.{}", std::get<std::string>(ev.get_parameter("item"))), std::get<int64_t>(ev.get_parameter("amount"))}}}},
						[ctx, ev, username](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg("ok done"));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "inventory");
								ev.edit_response(ephmsg(fmt::format("{} has not registered yet!", username)));
							}
						});
				});
		} else {
			ctx.maindb.patch(
				std::to_string(ev.command.usr.id),
				{{"increment",
				  {{fmt::format("inv.{}", std::get<std::string>(ev.get_parameter("item"))), std::get<int64_t>(ev.get_parameter("amount"))}}}},
				[ctx, ev](const dpp::http_request_completion_t &evt) {
					if (evt.status == 200) {
						ev.edit_response(ephmsg("ok done"));
					} else {
						ctx.cooldowns.reset_trigger(ev.command.usr.id,"inventory");
						ev.edit_response(ephmsg("You have not registered yet!"));
					}
				});
		}
	});
}

void give_money(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		dpp::snowflake author_id = ev.command.usr.id;
		if (author_id != dpp::snowflake(763854419484999722)) {
			return ev.edit_response(":nerd:");
		}
		if (std::holds_alternative<dpp::snowflake>(ev.get_parameter("user"))) {
			dpp::snowflake user_id = std::get<dpp::snowflake>(ev.get_parameter("user"));
			ctx.bot.user_get(
				user_id,
				[ctx, ev](const dpp::confirmation_callback_t &callback) {
					dpp::user_identified user = std::get<dpp::user_identified>(callback.value);
					std::string username = user.username;
					ctx.maindb.patch(
						std::to_string(user.id),
						{{"increment",
						  {{"bal",
							std::get<int64_t>(ev.get_parameter("amount"))}}}},
						[ctx, ev, username](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg("ok done"));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "inventory");
								ev.edit_response(ephmsg(fmt::format("{} has not registered yet!", username)));
							}
						});
				});
		} else {
			ctx.maindb.patch(
				std::to_string(ev.command.usr.id),
				{{"increment",
				  {{"bal", std::get<int64_t>(ev.get_parameter("amount"))}}}},
				[ctx, ev](const dpp::http_request_completion_t &evt) {
					if (evt.status == 200) {
						ev.edit_response(ephmsg("ok done"));
					} else {
						ctx.cooldowns.reset_trigger(ev.command.usr.id, "inventory");
						ev.edit_response(ephmsg("You have not registered yet!"));
					}
				});
		}
	});
}

void bitcoin(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		dpp::command_interaction cmd_data = ev.command.get_command_interaction();
		if (cmd_data.options[0].name == "buy") {
			if (std::get<int64_t>(cmd_data.options[0].options[0].value) <= 0) {
				return ev.edit_response(ephmsg("").add_embed(dpp::embed().set_title("Stop trying to buy/sell nothing!")));
			}
			getbal_then(
				ctx.maindb, std::to_string(ev.command.usr.id),
				[ctx, ev, cmd_data](json pl) {
					int64_t balance = pl["bal"].get<int64_t>();
					int64_t amount = std::get<int64_t>(cmd_data.options[0].options[0].value);
					if (pl["btc"].get<int64_t>() > 1000000000 || pl["btc"].get<int64_t>() + amount > 1000000000) {
						return ev.edit_response(ephmsg("You can have 1 billion bitcoins at max."));
					}
					if (balance < exchange_rate * amount) {
						return ev.edit_response(ephmsg(fmt::format("You're broke mg it costs {}", FormatWithCommas(amount * exchange_rate))));
					}
					ctx.maindb.patch(
						std::to_string(ev.command.usr.id),
						{{"increment",
						  {{"bal", -(amount * exchange_rate)},
						   {"btc", amount}}}},
						[ev, amount](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg(fmt::format(
									"Successfully bought {} btc for {}.",
									FormatWithCommas(amount),
									FormatWithCommas(amount * exchange_rate))));
							} else {
								ev.edit_response(ephmsg("Something went wrong sorry man"));
							}
						});
				},
				[ctx, ev]() {
					ctx.cooldowns.reset_trigger(ev.command.usr.id, "btc");
					ev.edit_response(ephmsg("You have not registered yet!"));
				});
		} else if (cmd_data.options[0].name == "sell") {
			if (std::get<int64_t>(cmd_data.options[0].options[0].value) <= 0) {
				return ev.edit_response(
					ephmsg("").add_embed(dpp::embed().set_title("Stop trying to buy/sell nothing!")));
			}
			getbal_then(
				ctx.maindb, std::to_string(ev.command.usr.id),
				[ctx, ev, cmd_data](json pl) {
					int64_t balance = pl["btc"].get<int64_t>();
					int64_t amount = std::get<int64_t>(cmd_data.options[0].options[0].value);
					if (balance < amount) {
						return ev.edit_response(ephmsg("You don't have enough bitcoins."));
					}
					ctx.maindb.patch(
						std::to_string(ev.command.usr.id),
						{{"increment",
						  {{"bal", amount * exchange_rate}, {"btc", -amount}}}},
						[ev, amount](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg(fmt::format("Successfully sold {} btc for {}.", FormatWithCommas(amount), FormatWithCommas(amount * exchange_rate))));
							} else {
								ev.edit_response(ephmsg("Something went wrong sorry man"));
							}
						});
				},
				[ctx, ev]() {
					ctx.cooldowns.reset_trigger(ev.command.usr.id, "btc");
					ev.edit_response(ephmsg("You have not registered yet!"));
				});
		} else if (cmd_data.options[0].name == "view") {
			if (cmd_data.options[0].options.size() > 0) {
				dpp::snowflake user_id = std::get<dpp::snowflake>(cmd_data.options[0].options[0].value);
				ctx.bot.user_get(
					user_id, [ctx, ev, user_id](const dpp::confirmation_callback_t &callback) {
						std::string username = std::get<dpp::user_identified>(callback.value).username;
						getbal_then(
							ctx.maindb, std::to_string(user_id),
							[ev, username](json pl) {
								int64_t balance = pl["btc"].get<int64_t>();
								ev.edit_response(ephmsg("").add_embed(
									dpp::embed()
										.set_title(fmt::format("{}'s Bitcoin Status", username))
										.set_color(dpp::colors::moon_yellow)
										.add_field("BTC amount", fmt::format("{} has {} bitcoins, stonks!", username, FormatWithCommas(balance)))
                                ));
							},
							[ctx, ev, username]() {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "btc");
								ev.edit_response(ephmsg(fmt::format("{} has not registered yet!", username)));
							});
					});
			} else {
				std::string username = ev.command.usr.username;
				getbal_then(
					ctx.maindb, std::to_string(ev.command.usr.id),
					[ev, username](json pl) {
						int64_t balance = pl["btc"].get<int64_t>();
						ev.edit_response(ephmsg("").add_embed(
							dpp::embed()
								.set_title(fmt::format("{}'s Bitcoin Status", username))
								.set_color(dpp::colors::moon_yellow)
								.add_field(
									"BTC amount",
									fmt::format("You have {} bitcoins, stonks!", FormatWithCommas(balance)))));
					},
					[ctx, ev]() {
						ctx.cooldowns.reset_trigger(ev.command.usr.id, "btc");
						ev.edit_response(ephmsg("You have not registered yet!"));
					});
			}
		} else if (cmd_data.options[0].name == "rate") {
			ev.edit_response(ephmsg(fmt::format(
				"{}\nNext reset in {} seconds.",
				std::to_string(exchange_rate),
				std::to_string(60 - ctx.btc_timer))));
		} else if (cmd_data.options[0].name == "reset") {
			exchange_rate = 10000;
			ev.edit_response(ephmsg("ok"));
		}
	});
}

void share(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	// tax hehehehaw
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		if (std::get<int64_t>(ev.get_parameter("amount")) <= 0 && std::get<int64_t>(ev.get_parameter("amount")) != -1) {
			return ev.edit_response(ephmsg("Share at least 1 coin bro"));
		}
		dpp::snowflake user_id = std::get<dpp::snowflake>(ev.get_parameter("user"));
		if (user_id == ev.command.usr.id) {
			return ev.edit_response(ephmsg("You can't share money with yourself."));
		}
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ctx, ev, user_id](json pl) {
				int64_t balance = pl["bal"].get<int64_t>();
				bool pog = pl["badges"]["Pog"].get<bool>();
				int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
				if (amount == -1) {
                    if (balance == 0) {
                        return ev.edit_response(ephmsg("You have no money in your wallet xd"));
                    }
                    amount = balance;
                } else if (balance < amount) {
					return ev.edit_response(ephmsg("You're broke mg"));
				}
				amount *= (pog ? 1 : 0.85);
				ctx.maindb.patch(
					std::to_string(user_id), {{"increment", {{"bal", amount}}}},
					[ctx, ev, amount, pog](const dpp::http_request_completion_t &evt) {
						if (evt.status == 200) {
							ctx.maindb.patch(
								std::to_string(ev.command.usr.id),
								{{"increment", {{"bal", -amount}}}},
								[ev, amount, pog](auto _) {
									ev.edit_response(ephmsg(fmt::format("{} coin shared{}.", amount, (pog ? ", and you're pog so you get NO TAX" : " after a 15% tax rate"))));
								});
						} else {
							ctx.cooldowns.reset_trigger(ev.command.usr.id, "share");
							ev.edit_response(ephmsg("They have not registered yet!"));
						}
					});
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "share");
				ev.edit_response(ephmsg("You have not registered yet!"));
			});
	});
}

void share_item(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	// tax hehehehaw
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		if (!shop_items.contains(std::get<std::string>(ev.get_parameter("item")))) {
			return ev.edit_response(ephmsg("That item doesn't exist"));
		}
		if (std::get<int64_t>(ev.get_parameter("amount")) <= 0 && std::get<int64_t>(ev.get_parameter("amount")) != -1) {
			return ev.edit_response(ephmsg("Share at least 1 item bro"));
		}
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ctx, ev](json pl) {
				int64_t balance = 0;
				if (!pl["inv"].contains(std::get<std::string>(ev.get_parameter("item")))) {
					chkinv(ctx.maindb, pl["inv"], std::to_string(ev.command.usr.id));
				} else {
					balance = pl["inv"][std::get<std::string>(ev.get_parameter("item"))];
				}
				int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
                if (amount == -1) {
                    if (balance == 0) {
                        return ev.edit_response(ephmsg("You have none of that lol"));
                    }
                    amount = balance;
                } else if (balance < amount) {
					return ev.edit_response(ephmsg("You're broke mg"));
				}
				dpp::snowflake user_id = std::get<dpp::snowflake>(ev.get_parameter("user"));
				if (user_id == ev.command.usr.id) {
					return ev.edit_response(ephmsg("You can't share items with yourself."));
				}
				ctx.maindb.patch(
					std::to_string(user_id),
					{{"increment",
					  {{"inv." + std::get<std::string>(ev.get_parameter("item")), amount}}}},
					[ctx, ev, amount](const dpp::http_request_completion_t &evt) {
						if (evt.status == 200) {
							ctx.maindb.patch(
								std::to_string(ev.command.usr.id),
								{{"increment",
								  {{"inv." + std::get<std::string>(ev.get_parameter("item")), -amount}}}},
								[ev, amount](auto _) {
									ev.edit_response(ephmsg(fmt::format("{} of {} shared.", amount, std::get<std::string>(ev.get_parameter("item")))));
								});
						} else {
							ctx.cooldowns.reset_trigger(ev.command.usr.id, "shareitem");
							ev.edit_response(ephmsg("They have not registered yet!"));
						}
					});
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "shareitem");
				ev.edit_response(ephmsg("You have not registered yet!"));
			});
	});
}

void use_item(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(std::get<std::string>(ev.get_parameter("item")) == "cursed_beef" ? true : EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		std::string usable[] = {"bank_space", "coin_bag", "beef", "cursed_beef"};
		std::string item = std::get<std::string>(ev.get_parameter("item"));
		std::string user_id = std::to_string(ev.command.usr.id);
		// not possible
		// if (!shop_items.contains(item)) {
		// 	ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
		// 	return ev.edit_response(ephmsg("That item doesn't exist"));
		// }
		if (std::find(std::begin(usable), std::end(usable), item) == std::end(usable)) {
			ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
			return ev.edit_response(ephmsg("That item is not usable"));
		}
		if (std::get<int64_t>(ev.get_parameter("amount")) <= 0 && std::get<int64_t>(ev.get_parameter("amount")) != -1) {
			// ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
			return ev.edit_response(ephmsg("Use at least 1 item bro"));
		}
		getbal_then(
			ctx.maindb, user_id,
			[ctx, ev, item, user_id](json pl) {
				int64_t balance = 0;
				if (!pl["inv"].contains(item)) {
					chkinv(ctx.maindb, pl["inv"], user_id);
				} else {
					balance = pl["inv"][item];
				}
				int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
                if (amount == -1) {
                    if (balance == 0) {
                        return ev.edit_response(ephmsg("You have none of that lol"));
                    }
                    amount = balance;
                } else if (balance < amount) {
					return ev.edit_response(ephmsg("You don't have enough."));
				}
				find_use(ctx, ev, amount, user_id, item, pl);
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "use");
				ev.edit_response(ephmsg("You have not registered yet!"));
			});
	});
}

void buy_item(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		// not possible
        // if (!shop_items.contains(std::get<std::string>(ev.get_parameter("item")))) {
		// 	return ev.edit_response(ephmsg("That item doesn't exist"));
		// }
		if (!shop_items[std::get<std::string>(ev.get_parameter("item"))]["purchasable"].get<bool>()) {
			ctx.cooldowns.reset_trigger(ev.command.usr.id, "buy");
			return ev.edit_response(ephmsg("This item is not for sale."));
		}
		if (std::get<int64_t>(ev.get_parameter("amount")) <= 0 && std::get<int64_t>(ev.get_parameter("amount")) != -1) {
			return ev.edit_response(ephmsg("Buy at least 1 item bro"));
		}
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ctx, ev](json pl) {
				int64_t balance = pl["bal"].get<int64_t>();
				int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
                int64_t price = shop_items[std::get<std::string>(ev.get_parameter("item"))]["price"].get<int64_t>();
                if (amount == -1) {
                    // automatic floor lets gooo
                    amount = balance / price;
                    if (amount == 0) {
                        return ev.edit_response(ephmsg("You can't afford any of that."));
                    }
                } else if (balance < price * amount) {
					return ev.edit_response(ephmsg("You're broke mg"));
				}
                ctx.maindb.patch(
					std::to_string(ev.command.usr.id),
					{{"increment",
					    {
                            {"inv." + std::get<std::string>(ev.get_parameter("item")), amount},
                            {"bal", -amount * price}
                        }
                    }},
					[ctx, ev, amount, price](const dpp::http_request_completion_t &evt) {
						if (evt.status == 200) {
							ev.edit_response(ephmsg(fmt::format("Bought {} of {} for {}.", amount, shop_items[std::get<std::string>(ev.get_parameter("item"))]["display"].get<std::string>(), amount * price)));
						} else {
							ctx.cooldowns.reset_trigger(ev.command.usr.id, "buy");
							ev.edit_response(ephmsg("Something went wrong while processing your transaction, try again later."));
						}
					});
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "buy");
				ev.edit_response(ephmsg("You have not registered yet!"));
			});
    });
}

void sell_item(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
    ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		// not possible;
        // if (!shop_items.contains(std::get<std::string>(ev.get_parameter("item")))) {
		// 	ctx.cooldowns.reset_trigger(ev.command.usr.id, "sell");
		// 	return ev.edit_response(ephmsg("That item doesn't exist"));
		// }
		if (!shop_items[std::get<std::string>(ev.get_parameter("item"))]["sellable"].get<bool>()) {
			ctx.cooldowns.reset_trigger(ev.command.usr.id, "sell");
			return ev.edit_response(ephmsg("You can't sell this buddy!"));
		}
		if (std::get<int64_t>(ev.get_parameter("amount")) <= 0 && std::get<int64_t>(ev.get_parameter("amount")) != -1) {
			return ev.edit_response(ephmsg("Sell at least 1 item bro"));
		}
		getbal_then(
			ctx.maindb, std::to_string(ev.command.usr.id),
			[ctx, ev](json pl) {
				int64_t balance = 0;
				if (!pl["inv"].contains(std::get<std::string>(ev.get_parameter("item")))) {
					chkinv(ctx.maindb, pl["inv"], std::to_string(ev.command.usr.id));
				} else {
					balance = pl["inv"][std::get<std::string>(ev.get_parameter("item"))];
				}
				int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
                if (amount == -1) {
                    if (balance == 0) {
                        return ev.edit_response(ephmsg("You have none of that lol"));
                    }
                    amount = balance;
                } else if (balance < amount) {
					return ev.edit_response(ephmsg("You're broke mg"));
				}
                int64_t price = shop_items[std::get<std::string>(ev.get_parameter("item"))]["price"].get<int64_t>() * 0.5;
                ctx.maindb.patch(
					std::to_string(ev.command.usr.id),
					{{"increment",
					    {
                            {"inv." + std::get<std::string>(ev.get_parameter("item")), -amount},
                            {"bal", amount * price}
                        }
                    }},
					[ctx, ev, amount, price](const dpp::http_request_completion_t &evt) {
						if (evt.status == 200) {
							ev.edit_response(ephmsg(fmt::format("Sold {} of {} for {}.", amount, shop_items[std::get<std::string>(ev.get_parameter("item"))]["display"].get<std::string>(), amount * price)));
						} else {
							ctx.cooldowns.reset_trigger(ev.command.usr.id, "sell");
							ev.edit_response(ephmsg("Something went wrong while processing your transaction, try again later."));
						}
					});
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "sell");
				ev.edit_response(ephmsg("You have not registered yet!"));
			});
    });
}

void shop(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ev](const dpp::confirmation_callback_t &v) {
		if (std::holds_alternative<std::string>(ev.get_parameter("item"))) {
			std::string item = std::get<std::string>(ev.get_parameter("item"));
			json it = shop_items[item];
			ev.edit_response(ephmsg("").add_embed(dpp::embed()
				.set_title(it["display"].get<std::string>())
				.set_description(it["description"].get<std::string>())
				.set_color(dpp::colors::red)
				.set_thumbnail(it["thumbnail"].get<std::string>())
				.add_field(
					"Price",
					std::to_string(it["price"].get<int64_t>()),
					true
				)
				.add_field(
					"Showing in shop",
					it["showing"].get<bool>() ? "true" : "false",
					true
				)
				.add_field(
					"Purchasable",
					it["purchasable"].get<bool>() ? "true" : "false",
					false
				)
				.add_field(
					"Sellable",
					fmt::format("{} \n__Note:__ sell price is half", it["sellable"].get<bool>() ? "true" : "false"),
					false
				)
				.add_field(
					"Usable",
					it["usable"].get<bool>() ? "true" : "false",
					false
				)
				.add_field(
					"usable IDs (for message commands)",
					fmt::format("`{}`", join_vec(it["name"].get<std::vector<std::string>>(), "`, `")),
					false
				)
			));
		} else {
			int64_t page = 1;
			if (std::holds_alternative<int64_t>(ev.get_parameter("page"))) {
				page = std::get<int64_t>(ev.get_parameter("page"));
			}
			ev.edit_response(getshop(page));
		}
	});
}

void roll(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		std::string user_id = std::to_string(ev.command.usr.id);
		int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
		if (amount < 15 && amount != -1) {
			return ev.edit_response(ephmsg("You need to bet at least 15 coins"));
		}
		getbal_then(
			ctx.maindb, user_id,
			[ctx, ev, user_id, amount](json pl) {
				int64_t balance = pl["bal"].get<int64_t>();
				int amount = amount;
				if (amount == -1) {
					if (balance < 15) {
						return ev.edit_response(ephmsg("You don't even have 15 coins to start gambling."));
					}
                    amount = balance;
                } else if (balance < amount) {
					return ev.edit_response(ephmsg("You don't have enough."));
				}
				amount = std::min(500, amount);
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distr(1, 13);
				int rand1 = distr(gen);
				int rand2 = distr(gen);
				if (rand1 > rand2) {
					int level = pl["exp"].get<int64_t>() / 100;
					// maximum 1000 levels worth of multiplier, which is 20%
					level = std::min(level, 1000);
					int multi = pl["multi"].get<int64_t>() + level / 50;
					// maximum 100% multiplier
					multi = std::min(multi, 100);
					if (pl["multi"].get<int64_t>() < 0) {
						// negative multiplier glitch bec of cursed beef
						ctx.maindb.patch(user_id, {
							{"set", {
								{"multi", 0}
							}}
						});
						multi = 0;
					}
					multi += pl["inv"]["horrorse_celery"].get<int64_t>() * 100;
					std::uniform_int_distribution<> rng(amount / 2, amount);
					int win_amt = (int)(rng(gen) * (double)(1 + (double)(multi / 100.0)));
					// minimum earn 0.5 * (1 + multi/100)
					// maximum earn 1 * (1 + multi/100)
					ctx.maindb.patch(
						user_id,
						{{"increment",
							{
								{"bal", win_amt}
							}
						}},
						[ctx, ev, win_amt, rand1, rand2](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg("").add_embed(
									dpp::embed()
										.set_title("You won.")
										.set_description("Profit: " + FormatWithCommas(win_amt))
										.set_color(dpp::colors::green)
										.add_field("You", "Rolled `" + std::to_string(rand1) + "`")
										.add_field("Capitalism", "Rolled `" + std::to_string(rand2) + "`")
										));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "roll");
								ev.edit_response(ephmsg("Something went wrong while processing your transaction, try again later."));
							}
					});
				} else if (rand1 < rand2) {
					ctx.maindb.patch(
						user_id,
						{{"increment",
							{
								{"bal", -amount}
							}
						}},
						[ctx, ev, amount, rand1, rand2](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg("").add_embed(
									dpp::embed()
										.set_title("You lost.")
										.set_description("Loss: " + std::to_string(amount))
										.set_color(dpp::colors::red)
										.add_field("You", "Rolled `" + std::to_string(rand1) + "`")
										.add_field("Capitalism", "Rolled `" + std::to_string(rand2) + "`")
										));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "roll");
								ev.edit_response(ephmsg("Something went wrong while processing your transaction, try again later."));
							}
					});
				} else {
					ev.edit_response(ephmsg("").add_embed(
						dpp::embed()
							.set_title("You tied!")
							.set_description("No money was withdrawn from your account.")
							.add_field("You", "Rolled `" + std::to_string(rand1) + "`")
							.add_field("Capitalism", "Rolled `" + std::to_string(rand2) + "`")
							));
				}
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "roll");
				ev.edit_response(ephmsg("You have not registered yet!"));
			});
	});
}

void roulette(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
	ev.thinking(EPH_OR_NOT, [ctx, ev](const dpp::confirmation_callback_t &v) {
		std::string user_id = std::to_string(ev.command.usr.id);
		int64_t amount = std::get<int64_t>(ev.get_parameter("amount"));
		if (amount < 50 && amount != -1) {
			return ev.edit_response(ephmsg("You need to bet at least 50 coins"));
		}
		int64_t bet = std::get<int64_t>(ev.get_parameter("bet"));
		if (bet > 12 || bet < 0) {
			return ev.edit_response(ephmsg("To play roulette you must pick a bet number between 0 and 12."));
		}
		getbal_then(
			ctx.maindb, user_id,
			[ctx, ev, user_id, amount, bet](json pl) {
				int64_t balance = pl["bal"].get<int64_t>();
				int amount = amount;
                if (amount == -1) {
					if (balance < 50) {
						return ev.edit_response("You don't even have 50 coins to start betting...");
					}
                    amount = balance;
                } else if (balance < amount) {
					return ev.edit_response(ephmsg("You don't have enough."));
				}
				amount = std::min(5000, amount);
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distr(0, 12);
				int rand1 = distr(gen);
				if (rand1 == bet) {
					int level = pl["exp"].get<int64_t>() / 100;
					// maximum 1000 levels worth of multiplier, which is 20%
					level = std::min(level, 1000);
					int multi = pl["multi"].get<int64_t>() + level / 50;
					// maximum 100% multiplier
					multi = std::min(multi, 100);
					if (pl["multi"].get<int64_t>() < 0) {
						// negative multiplier glitch bec of cursed beef
						ctx.maindb.patch(user_id, {
							{"set", {
								{"multi", 0}
							}}
						});
						multi = 0;
					}
					multi += pl["inv"]["horrorse_celery"].get<int64_t>() * 100;
					std::uniform_int_distribution<> rng((bet == 0) ? (amount) : (amount / 2), (bet == 0) ? (amount*2) : (amount));
					int win_amt = (int)(rng(gen) * (double)(1 + (double)(multi / 100.0)));
					// minimum earn 0.5 * (1 + multi/100)
					// maximum earn 1 * (1 + multi/100)
					ctx.maindb.patch(
						user_id,
						{{"increment",
							{
								{"bal", win_amt}
							}
						}},
						[ctx, ev, win_amt, rand1, bet](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg("").add_embed(
									dpp::embed()
										.set_title("You won!")
										.set_description("Profit: " + FormatWithCommas(win_amt))
										.set_color(dpp::colors::green)
										.add_field("Results:", "Win number: `" + std::to_string(bet) + "`.")
										.set_footer(dpp::embed_footer().set_text("Roulette will pick a number between 0 and 12"))
										));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "roulette");
								ev.edit_response(ephmsg("Something went wrong while processing your transaction, try again later."));
							}
					});
				} else if ((rand1 % 2) == (bet % 2)) {
					int level = pl["exp"].get<int64_t>() / 100;
					// maximum 1000 levels worth of multiplier, which is 20%
					level = std::min(level, 1000);
					int multi = pl["multi"].get<int64_t>() + level / 50;
					// maximum 100% multiplier
					multi = std::min(multi, 100);
					if (pl["multi"].get<int64_t>() < 0) {
						// negative multiplier glitch bec of cursed beef
						ctx.maindb.patch(user_id, {
							{"set", {
								{"multi", 0}
							}}
						});
						multi = 0;
					}
					multi += pl["inv"]["horrorse_celery"].get<int64_t>() * 100;
					std::uniform_int_distribution<> rng(amount / 8, amount / 4);
					int win_amt = (int)(rng(gen) * (double)(1 + (double)(multi / 100.0)));
					// minimum earn 0.5 * (1 + multi/100)
					// maximum earn 1 * (1 + multi/100)
					ctx.maindb.patch(
						user_id,
						{{"increment",
							{
								{"bal", win_amt}
							}
						}},
						[ctx, ev, win_amt, rand1, bet](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg("").add_embed(
									dpp::embed()
										.set_title("You won! Same colour.")
										.set_description("Profit: " + std::to_string(win_amt))
										.set_color(dpp::colors::green)
										.add_field("Results:", "Your bet: `" + std::to_string(bet) + "`. Win number: `" + std::to_string(rand1) + "`.")
										.set_footer(dpp::embed_footer().set_text("Roullete will pick a number between 0 and 12"))
										));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "roulette");
								ev.edit_response(ephmsg("Something went wrong while processing your transaction, try again later."));
							}
					});
				} else {
					ctx.maindb.patch(
						user_id,
						{{"increment",
							{
								{"bal", -amount}
							}
						}},
						[ctx, ev, amount, rand1, bet](const dpp::http_request_completion_t &evt) {
							if (evt.status == 200) {
								ev.edit_response(ephmsg("").add_embed(
									dpp::embed()
										.set_title("You lost.")
										.set_description("Loss: " + std::to_string(amount))
										.set_color(dpp::colors::red)
										.add_field("Results:", "Your bet: `" + std::to_string(bet) + "`. Win number: `" + std::to_string(rand1) + "`.")
										.set_footer(dpp::embed_footer().set_text("Roullete will pick a number between 0 and 12"))
										));
							} else {
								ctx.cooldowns.reset_trigger(ev.command.usr.id, "roulette");
								ev.edit_response(ephmsg("Something went wrong while processing your transaction, try again later."));
							}
					});
				}
			},
			[ctx, ev]() {
				ctx.cooldowns.reset_trigger(ev.command.usr.id, "roulette");
				ev.edit_response(ephmsg("You have not registered yet!"));
			});
	});
}