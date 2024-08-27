#ifndef UTILS_HPP
	#define UTILS_HPP

#include "irc.hpp"

std::string get_current_time();
void log_message(const std::string& message);
std::string trim_all(const std::string& str);
std::vector<std::string> parse_command(const std::string &message);

#endif