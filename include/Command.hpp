#ifndef COMMAND_HPP
    #define COMMAND_HPP

#include "irc.hpp"

//class command
//{
//    public:
//    command(std::vector<std::string> arg, client *client);
//    ~command();
//    private:
//};

class Command {
public:
    Command();
    ~Command();

    void execute(const std::string &command, client *sender);

private:
    void User(const std::vector<std::string> &params, client *sender);
    void Pass(const std::vector<std::string> &params, client *sender);
    //void Join(const std::vector<std::string> &params, client *sender);
    void Nick(const std::vector<std::string> &params, client *sender);
    //void Privmsg(const std::vector<std::string> &params, client *sender);
    // Ajoutez d'autres commandes ici

    typedef void (Command::*CommandHandler)(const std::vector<std::string> &, client *);
    std::map<std::string, CommandHandler> _commands;

    std::vector<std::string> parse_command(const std::string &command);
};

#endif