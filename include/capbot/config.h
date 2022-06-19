#ifndef CONFIG_H
#define CONFIG_H
#define EPH_OR_NOT false

#include <dpp/dpp.h>
#include <dpp/fmt/format.h>
#include <map>
#include <iostream>
#include <iomanip>
#include <locale>
#include <random>
#include "cmd.h"

template<class T>
std::string FormatWithCommas(T value) {
    std::stringstream ss;
    ss.imbue(std::locale("en_US.UTF-8"));
    ss << std::fixed << value;
    return ss.str();
}

void init_all_items();

// configuration file... including all the commands

void ping(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void uptime(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void help(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void think(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void _register(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void unregister(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void balance(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void beg(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void deposit(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void withdraw(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void fail(const CmdCtx ctx, const dpp::slashcommand_t &ev);

// max commands is 100, 25 subcommands, 25 subcommand groups
inline const std::map<std::string, command_definition> cmds = {
    { "ping", { "A ping command", ping, 0 }},
    { "help", {
        "A help command", help, 0, { 
            { dpp::command_option(dpp::co_string, "term", "Help term", false) },
        }
    }},
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
    { "beg", { "pls give me money I'm begging you", beg, 15 }},
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
};

inline const json shop_items = R"({
    "horrorse_celery":{
        "name": ["horrorse", "celery", "hc","horse"],
        "display": "<:HorrorsesCelery:859085174650437682> Horrorse's Celery",
        "thumbnail": "https://cdn.discordapp.com/emojis/859085174650437682.png?v=1",
        "price": 1000000,
        "description": "healthy plant for healthy brain",
        "showing": true,
        "purchasable": true,
        "sellable": false,
        "usable": true
    },
    "penguin_token": {
        "name": ["penguin_token", "pt", "penguin", "token"],
        "display": "<:penguintoken:831569919937347594> Penguin Token",
        "thumbnail": "https://cdn.discordapp.com/emojis/831569919937347594.png?v=1",
	    "price": 100000,
		"description": "A collectible from Alpha Version",
        "showing": false,
		"purchasable": false,
        "sellable": true,
        "usable": false
	},
    "capitalism_blessing":{
        "name": ["capitalism_blessing", "capitalism", "blessing", "cab"],
        "display": "<:capitalism:823936558989049896> Capitalism Blessing",
        "thumbnail": "https://lh3.googleusercontent.com/proxy/F4ho3iWRCG0JDyEWsq9h0H-Wtc5TlYKvTqizdmV9vZb0kzxH5JLKSVq81I27A0-ETcB6lWrByN7Og_BIdKr6efhAOZZexIGCUbktQvjn5vbnj8ogTGA3E9gtXT5Mdw",
        "price": 5000,
        "description": "the capitalism insurance, saves you from injured once.",
        "showing": true,
        "purchasable": true,
        "sellable": true,
        "usable": false
    },
    "puppy_true_gaymer_pass":{
        "name": ["puppy_true_gaymer_pass", "puppy", "gaymer", "pass", "ptpp", "puppytruegaymerpass"],
        "display": "<:puppytruegaymerpass:831923372249841664> PuppyTrueGaymerPass",
        "thumbnail": "https://cdn.discordapp.com/emojis/831923372249841664.png?v=1",
        "price": 50000,
        "description": "with this special pass, you will never pay for hospital bills.",
        "showing": true,
        "purchasable": true,
        "sellable": true,
        "usable": false
    },
    "bank_space":{
        "name": ["bank_space","bank", "space", "banks"],
        "display": "<:bankspace:831975502675574794> Bank Space",
        "thumbnail": "https://cdn.discordapp.com/emojis/831975502675574794.png?v=1",
        "price": 2000,
        "description": "Turn this in for more bank space!",
        "showing": true,
        "purchasable": true,
        "sellable": false,
        "usable": true
    },
    "coin_bag":{
        "name": ["coin_bag", "bag", "cob"],
        "display": "<:coinbag:831582197432582184> Coin Bag",
        "thumbnail": "https://cdn.discordapp.com/emojis/831582197432582184.png?v=1",
        "price": 10,
        "description": "Gives a random amount of coins if used.",
        "showing": false,
        "purchasable": false,
        "sellable": true,
        "usable": true
    },
    "alphatester_trophy":{
        "name": ["alpha", "alphatester", "att", "tester", "alphatester_trophy", "alpha_tester_trophy"],
        "display": ":trophy: AlphaTester Trophy",
        "thumbnail": "https://lh3.googleusercontent.com/proxy/F4ho3iWRCG0JDyEWsq9h0H-Wtc5TlYKvTqizdmV9vZb0kzxH5JLKSVq81I27A0-ETcB6lWrByN7Og_BIdKr6efhAOZZexIGCUbktQvjn5vbnj8ogTGA3E9gtXT5Mdw",
        "price": 0,
        "description": "Special Trophy for Alpha Testers",
        "showing": true,
        "purchasable": false,
        "sellable": false,
        "usable": false
    },
    "kirk_juice":{
        "name": ["kirk", "juice", "kj", "kirk_juice"],
        "display": "<:kirkjuice:831964183657578567> Kirk Juice",
        "thumbnail": "https://cdn.discordapp.com/emojis/831964183657578567.png?v=1",
        "price": 10,
        "description": "A collectible from BETA version of the bot",
        "showing": true,
        "purchasable": true,
        "sellable": false,
        "usable": false
    },
    "beef":{
        "name": ["beef", "b"],
        "display": "<:beef:832048317734846464> beef",
        "thumbnail": "https://cdn.discordapp.com/emojis/832048317734846464.png?v=1",
        "price": 50,
        "description": "Grants you 1 percent multiplier for 1 minute! Beef keeps you strong.",
        "showing": true,
        "purchasable": true,
        "sellable": true,
        "usable": true
    },
    "cursed_beef":{
        "name": ["cursed", "cb", "cursed_beef"],
        "display": "<:cursedbeef:832048360751497266> Cursed Beef",
        "thumbnail": "https://cdn.discordapp.com/emojis/832048360751497266.png?v=1",
        "price": 1000,
        "description": "Gives a -5% multi to the user mentioned, have 40% chance of backfiring. Lasts 30 minutes!",
        "showing": true,
        "purchasable": true,
        "sellable": true,
        "usable": true
    },
    "noob_trophy":{
        "name": ["noob", "trophy", "nt", "noob_trophy"],
        "display": "<:noob_trophy:852234074964623360> Noob Trophy",
        "thumbnail": "https://cdn.discordapp.com/emojis/852234074964623360.png?v=1",
        "price": 100000,
        "description": "Trophies that are bought is not meaningless",
        "showing": true,
        "purchasable": true,
        "sellable": false,
        "usable": false
    }
})"_json;

inline json shop_items_default;

inline const json badge_items = R"({
    "BETA_tester": {
        "thumbnail": "<:betatester:834935585952497685>",
        "description": "A badge for BETA testers"
    },
    "ALPHA_tester": {
        "thumbnail": "<:alphatester:834935540843020308>",
        "description": "A badge for ALPHA testers"
    },
    "RICH": {
        "thumbnail": "<:richbadge:834935629061816331>",
        "description": "A badge for people that are considered rich"
    },
    "Pog": {
        "thumbnail": "<:pogbadge:834935609118162944>",
        "description": "these people refuse to pay taxes"
    },
    "test": {
        "thumbnail": "",
        "description": "test badge"
    },
    "admin": {
        "thumbnail": "",
        "description": "admin of the bot"
    }
})"_json;

inline json badge_items_default;

#endif