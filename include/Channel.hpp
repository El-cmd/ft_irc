#ifndef CHANNEL_HPP
    #define CHANNEL_HPP

#include "irc.hpp"

class Channel
{
    public:
        Channel(std::string args, client *creator, Server *serv);
        ~Channel();
        void addClient(client *sender);
        bool alreadyIn(client *sender);
    /* +++ Getter +++ */
        std::string getName();
        bool withKey();
    /* ++++++++++++++ */
    private:
        Channel();
        bool _whithKey;
        std::map<int, client *> _clients;
        std::string _name;
        client *_admin;
        std::string _topic;
};

#endif