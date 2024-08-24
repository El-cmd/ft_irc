#include "../include/Command.hpp"

//command::command(std::vector<std::string> arg, client *client)
//{
//    if (arg[0] == "NICK")
//    {
//        client->setNick(arg[1]);
//    }
//}
Command::Command()
{
    _commands["NICK"] = &Command::Nick;
    _commands["PASS"] = &Command::Pass;
    _commands["USER"] = &Command::User;

}

Command::~Command()
{

}