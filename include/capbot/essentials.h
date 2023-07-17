#ifndef CAPBOT_ESSENTIALS_H_
#define CAPBOT_ESSENTIALS_H_
#include <string>
#include <vector>

void replace_all(std::string& str, const std::string& from, const std::string& to);

void join(const std::vector<std::string>& v, char c, std::string& s);

std::string join_vec(const std::vector<std::string>& v, std::string c);

#endif