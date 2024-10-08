#ifndef COMMAND_HPP
    #define COMMAND_HPP

#include "irc.hpp"

class Command
{
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
        void Mode(const std::vector<std::string> &params, client *sender, Server *tmp);
        void Quit(const std::vector<std::string> &params, client *sender, Server *tmp);
        void Kick(const std::vector<std::string> &params, client *sender, Server *tmp);
        void Invite(const std::vector<std::string> &params, client *sender, Server *tmp);
        void PrivMsg(const std::vector<std::string> &params, client *sender, Server *tmp);
    /* +++++++++++++++++ */
    /* +++ Mode option +++*/
        void applyMode(Channel* chan, client* sender, char mode, bool addMode, const std::vector<std::string>& params, Server* tmp, int& paramIndex);
        void handleInviteOnlyMode(Channel* chan, bool addMode);
        void handleTopicRestrictionMode(Channel* chan, bool addMode);
        void handleChannelKeyMode(Channel* chan, client* sender, bool addMode, const std::vector<std::string>& params, int& paramIndex);
        void handleOperatorMode(Channel* chan, client* sender, bool addMode, const std::vector<std::string>& params,Server* tmp, int& paramIndex);
        void handleUserLimitMode(Channel *chan, client* sender, bool addMode, const std::vector<std::string>& params, int& paramIndex);
    /* +++++++++++++++++ */

        typedef void (Command::*CommandHandler)(const std::vector<std::string> &, client *, Server *);
        std::map<std::string, CommandHandler> _commands;
};

#endif