#ifndef GENERAL_H_
#define GENERAL_H_
#include <dpp/dpp.h>
#include "../cmd.h"

void ping(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void uptime(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void help(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void think(const CmdCtx ctx, const dpp::slashcommand_t &ev);

void fail(const CmdCtx ctx, const dpp::slashcommand_t &ev);

// void button(const CmdCtx ctx, const dpp::slashcommand_t &ev);

#endif