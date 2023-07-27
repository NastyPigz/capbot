#ifndef CURRENCY_H_
#define CURRENCY_H_
#include <dpp/dpp.h>
#include "../cmd.h"

inline int exchange_rate = 10000;

dpp::task<void> _register(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> unregister(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> balance(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> bank(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> beg(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> mine(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> deposit(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> withdraw(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> inventory(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> give_items(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> give_money(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> bitcoin(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> share(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> share_item(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> buy_item(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> sell_item(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> use_item(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> shop(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> banks(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> roll(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> roulette(const CmdCtx ctx, const dpp::slashcommand_t ev);

#endif