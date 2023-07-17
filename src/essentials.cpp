#include "capbot/essentials.h"

void replace_all(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty()) return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();  // In case 'to' contains 'from', like
								   // replacing 'x' with 'yx'
	}
}

void join(const std::vector<std::string>& v, char c, std::string& s) {
	s.clear();

	for (std::vector<std::string>::const_iterator p = v.begin(); p != v.end();
		 ++p) {
		s += *p;
		if (p != v.end() - 1) s += c;
	}
}

std::string join_vec(const std::vector<std::string>& v, std::string c) {
    std::string s = "";
    for (std::vector<std::string>::const_iterator p = v.begin(); p != v.end(); ++p) {
		s += *p;
		if (p != v.end() - 1) s += c;
	}
    return s;
}