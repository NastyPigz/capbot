#ifndef CONFIG_H_
#define CONFIG_H_
#define EPH_OR_NOT false

#include <dpp/dpp.h>
#include <fmt/format.h>
#include <map>
#include <iostream>
#include <iomanip>
#include <locale>
#include <random>
#include "cmd.h"
#include "items.h"
#include "commands/currency.h"
#include "commands/general.h"

template<class T>
std::string FormatWithCommas(T value) {
    std::stringstream ss;
    ss.imbue(std::locale("en_US.UTF-8"));
    ss << std::fixed << value;
    return ss.str();
}

void init_all_items();

// configuration file... including all the commands

// max commands is 100, 25 subcommands, 25 subcommand groups
inline const std::map<std::string, command_definition> cmds = {
    { "ping", { "A ping command", ping, 0 }},
    /* help is literally useless */
    // { "help", {
    //     "A help command", help, 0, { 
    //         { dpp::command_option(dpp::co_string, "term", "Help term", false) },
    //     }
    // }},
    { "uptime", { "An uptime command", uptime, 0 }},
    { "think", { "think forever (15 minutes)", think, 15 * 60 }},
    { "fail", { "This application did not respond", fail, 1 * 60 }},
    { "register", { "register an account for capbot", _register, 60 * 60 }},
    { "unregister", { "unregister your account for capbot", unregister, 2 * 60 * 60 }},
    { "balance", {
        "Check your balance", balance, 1, {
            { dpp::command_option(dpp::co_user, "user", "Target user's balance", false) },
        }
    }},
    {"bank", {
        "View banking profile, available banks, or sign up for a new bank system", bank, 1, {
            {
                dpp::command_option(dpp::co_sub_command, "profile", "View your banking profile").
                    add_option(dpp::command_option(dpp::co_user, "user", "Target user's banking profile", false))
            },
            {
                dpp::command_option(dpp::co_sub_command, "buy", "Buy a new bank system").
                    add_option(
                        dpp::command_option(dpp::co_integer, "type", "The bank you want to buy.", true)
                            .add_choice(dpp::command_option_choice("default", std::int64_t(0)))
                            .add_choice(dpp::command_option_choice("premium", std::int64_t(1)))
                            .add_choice(dpp::command_option_choice("royal", std::int64_t(2)))
                            .add_choice(dpp::command_option_choice("meme", std::int64_t(3)))
                            .add_choice(dpp::command_option_choice("ussr", std::int64_t(4))))
            },
            {
                dpp::command_option(dpp::co_sub_command, "view", "View the information about a certain bank").
                    add_option(
                        dpp::command_option(dpp::co_integer, "type", "The bank you want to buy.", false)
                            // you cannot switch back to default bank
                            .add_choice(dpp::command_option_choice("default", std::int64_t(0)))
                            .add_choice(dpp::command_option_choice("premium", std::int64_t(1)))
                            .add_choice(dpp::command_option_choice("royal", std::int64_t(2)))
                            .add_choice(dpp::command_option_choice("meme", std::int64_t(3)))
                            .add_choice(dpp::command_option_choice("ussr", std::int64_t(4))))
            }
        }
    }},
    // { "bank", {
    //     "Check your bank", bank, 1
    // }},
    // { "banks", {
    //     "View avaliable banks", banks, 1, {
    //         { dpp::command_option(dpp::co_integer, "type", "the bank you want to view.", false)
    //             .add_choice(dpp::command_option_choice("default", std::int64_t(0)))
    //             .add_choice(dpp::command_option_choice("premium", std::int64_t(1)))
    //             .add_choice(dpp::command_option_choice("royal", std::int64_t(2)))
    //             .add_choice(dpp::command_option_choice("meme", std::int64_t(3)))
    //             .add_choice(dpp::command_option_choice("ussr", std::int64_t(4)))
    //         }
    //     }
    // }},
    { "beg", { "pls give me money I'm begging you", beg, 15 }},
    { "mine", { "40% risk of dying and losing all your wallet money... but 60% big price", mine, 30}},
    { "deposit", {
        "place money into your bank", deposit, 5, {
            {
                dpp::command_option(dpp::co_integer, "amount", "Amount you want to deposit (-1 for max amount /all)", true)
            }
        }
    }},
    { "withdraw", {
        "withdraw money from your bank", withdraw, 5, {
            {
                dpp::command_option(dpp::co_integer, "amount", "Amount you want to withdraw (-1 for max amount /all)", true)
            }
        }
    }},
    { "inventory", {
        "views your current inventory", inventory, 1, {
            {
                dpp::command_option(dpp::co_integer, "page", "Page of inventory", false)
            },
            {
                dpp::command_option(dpp::co_user, "user", "Target user's inventory", false)
            }
        }
    }},
    {"gibitem", {
        "abuse an item to someone", give_items, 0, {
            {
                dpp::command_option(dpp::co_string, "item", "Name of item", true)
            },
            {
                dpp::command_option(dpp::co_integer, "amount", "amount of items", true)
            },
            {
                dpp::command_option(dpp::co_user, "user", "Target user's inventory", false)
            }
        }
    }},
    {"gib", {
        "abuse money", give_money, 0, {
            {
                dpp::command_option(dpp::co_integer, "amount", "amount of money", true)
            },
            {
                dpp::command_option(dpp::co_user, "user", "Target user's money", false)
            }
        }
    }},
    {"btc", {
        "bitcoins", bitcoin, 1, {
            {
                dpp::command_option(dpp::co_sub_command, "view", "See bitcoins").
                    add_option(dpp::command_option(dpp::co_user, "user", "Target user's bitcoins", false))
            },
            {
                dpp::command_option(dpp::co_sub_command, "buy", "Buy bitcoins").
                    add_option(dpp::command_option(dpp::co_integer, "amount", "amount of bitcoins to buy", true))
            },
            {
                dpp::command_option(dpp::co_sub_command, "sell", "Sell bitcoins").
                    add_option(dpp::command_option(dpp::co_integer, "amount", "amount of bitcoins to sell", true))
            },
            {
                dpp::command_option(dpp::co_sub_command, "rate", "Shows you bitcoin exchange rate")
            },
            {
                dpp::command_option(dpp::co_sub_command, "reset", "Resets bitcoin rate to 50,000")
            }
        }
    }},
    {"share", {
        "sharing is caring", share, 3, {
            {
                dpp::command_option(dpp::co_user, "user", "Which user to share to", true)
            },
            {
                dpp::command_option(dpp::co_integer, "amount", "How much to share", true)
            }
        }
    }},
    {"shareitem", {
        "gives an item to someone", share_item, 0, {
            {
                dpp::command_option(dpp::co_user, "user", "Target user's inventory", true)
            },
            {
                dpp::command_option(dpp::co_string, "item", "Name of item", true)
            },
            {
                dpp::command_option(dpp::co_integer, "amount", "amount of items", true)
            }
        }
    }},
    {"buy", {
        "Buys an item", buy_item, 1, {
            {
                dpp::command_option(dpp::co_string, "item", "Name of item", true)
            },
            {
                dpp::command_option(dpp::co_integer, "amount", "amount of items", true)
            }
        }
    }},
    {"sell", {
        "Sells an item (half price)", sell_item, 1, {
            {
                dpp::command_option(dpp::co_string, "item", "Name of item", true)
            },
            {
                dpp::command_option(dpp::co_integer, "amount", "amount of items", true)
            }
        }
    }},
    {"use", {
        "Use an item", use_item, 1, {
            {
                dpp::command_option(dpp::co_string, "item", "Name of item", true)
            },
            {
                dpp::command_option(dpp::co_integer, "amount", "amount of items", true)
            }
        }
    }},
    {"shop", {
        "Views shop", shop, 1, {
            {
                dpp::command_option(dpp::co_string, "item", "Name of item", false)
            },
            {
                dpp::command_option(dpp::co_integer, "page", "page number", false)
            }
        }
    }},
    { "shutdown", { "Shuts off the bot", shutdown, 0 }},
    { "roll", {
        "Gambling command, 15 coins minimum and 500 maximum bet", roll, 15, {
            {
                dpp::command_option(dpp::co_integer, "amount", "amount to bet, 25 - 500", true)
            }
        }
    }},
    { "roulette", {
        "Gambling command, 50 coins minimum and 5000 maximum bet", roulette, 15, {
            {
                dpp::command_option(dpp::co_integer, "amount", "amount to bet, 50 - 5000", true)
            },
            {
                dpp::command_option(dpp::co_integer, "bet", "number between 0 and 12 to bet on", true)
            }
        }
    }}
};

#endif