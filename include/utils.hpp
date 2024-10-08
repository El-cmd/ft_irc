#ifndef UTILS_HPP
	#define UTILS_HPP

#include "irc.hpp"

std::string get_current_time();
void log_message(const std::string& message);
std::vector<std::string> trim_all(const std::string& str);
std::vector<std::string> parse_command(const std::string &message);
void log_message_client(int fd, const std::string &message);
bool verifOptionMode(std::string param);
std::stack<char> initQueue(std::string param);
bool isValidNumber(const std::string& str);
void sendQuitMessageToAllChannels(client* sender, const std::vector<std::string> params);
std::string joinStringsExcludingFirst(const std::vector<std::string>& vec);
void sendMessageToChannel(Channel* channel, const std::string& message, client* sender);
bool is_whitespace(char c);

#endif