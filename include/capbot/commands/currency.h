#ifndef CURRENCY_H_
#define CURRENCY_H_
#include <dpp/dpp.h>
#include "../cmd.h"

void _register(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void unregister(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void balance(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void beg(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void deposit(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void withdraw(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void inventory(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void give_items(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void give_money(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void bitcoin(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void share(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void share_item(const CmdCtx ctx, const dpp::slashcommand_t &ev);

#endif