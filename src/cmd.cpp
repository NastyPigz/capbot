#include "capbot/cmd.h"
#include "capbot/db/crud.h"

dpp::message ephmsg(std::string content) {
    dpp::message m(content);
    m.set_flags(dpp::m_ephemeral);
    return m;
}