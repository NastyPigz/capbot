#ifndef GENERAL_H_
#define GENERAL_H_
#include <dpp/dpp.h>
#include "../cmd.h"

dpp::task<void> ping(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> uptime(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> help(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> think(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> fail(const CmdCtx ctx, const dpp::slashcommand_t ev);

dpp::task<void> shutdown(const CmdCtx ctx, const dpp::slashcommand_t ev);

// dpp::task<void> button(const CmdCtx ctx, const dpp::slashcommand_t ev);

#endif