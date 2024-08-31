#ifndef COMMAND_HPP
    #define COMMAND_HPP

#include "irc.hpp"

class Command {
public:
    Command();
    ~Command();

    void execute(const std::string &command, client *sender, Server *);

private:

/* +++ Commandes +++ */
    void User(const std::vector<std::string> &params, client *sender, Server *tmp);
    void Pass(const std::vector<std::string> &params, client *sender, Server *tmp);
    void Join(const std::vector<std::string> &params, client *sender, Server *tmp);
    void Nick(const std::vector<std::string> &params, client *sender, Server *tmp);
    void Topic(const std::vector<std::string> &params, client *sender, Server *tmp);
/* +++++++++++++++++ */
    ////void Privmsg(const std::vector<std::string> &params, client *sender);
    //// Ajoutez d'autres commandes ici

    typedef void (Command::*CommandHandler)(const std::vector<std::string> &, client *, Server *);
    std::map<std::string, CommandHandler> _commands;

    //std::vector<std::string> parse_command(const std::string &command);
};

#endif