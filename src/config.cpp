// configuration file... including all the commands
#include "capbot/cmd.h"
#include "capbot/config.h"

// only allow 100 commands
// 25 subcommands

void init_all_items() {
    for (auto& el : shop_items.items()) {
        shop_items_default[el.key()] = 0;
    }
    for (auto& el : badge_items.items()) {
        badge_items_default[el.key()] = false;
    }
}

void fail(const CmdCtx ctx, const dpp::slashcommand_t &ev) {
   // hehehehaw
}