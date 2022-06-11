// class BaseCommand;

using command_function = std::function<void(dpp::cluster&, const dpp::slashcommand_t&)>;

struct command_definition {
	std::string description;
	command_function function;
	std::vector<dpp::command_option> parameters = {};
};

dpp::message ephmsg(std::string content) {
    dpp::message m(content);
    m.set_flags(dpp::m_ephemeral);
    return m;
}

// class BaseCommand {
//   std::string name;
// public:
//   dpp::cluster* bot;
//   BaseCommand(dpp::cluster &bot, const std::string &name):
//     name(name), bot(&bot) {
//         cmds.insert({ name, this });
//     }
//   virtual void handle(const dpp::message &msg) const = 0; // pure virtual 
// };

// bool find_commands(std::string cmd, dpp::message msg) {
//   if (cmds.find(cmd) != cmds.end()) {
//       cmds.at(cmd)->handle(msg);
//       return true;
//   } else {
//       return false;
//   }
// }