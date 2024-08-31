#include "../include/Channel.hpp"

Channel::Channel(std::string name, client *creator, Server *serv): _name(name), _admin(creator)
{
    log_message("The channel " + name + " has been created by " + creator->getNick());
    log_message_client(creator->getFd(), "You are created the " + name + " Channel");
    _whithKey = false;
    _admin = creator;
    creator->getChan().push_back(this);
    serv->newChannel(this);
    this->_clients.insert(std::make_pair(creator->getFd(), creator));
}

Channel::~Channel()
{

}

/* +++ Getter +++ */

std::string Channel::getName()
{
    return this->_name;
}

bool Channel::withKey()
{
    return this->_whithKey;
}

/* +++++++++++++++ */

bool Channel::alreadyIn(client *sender)
{
    int i = sender->getFd();
    std::map<int, client *>::iterator it = this->_clients.begin();
    while (it != this->_clients.end())
    {
        if (it->first == i)
        {
            log_message_client(i, "You are already in " + this->getName() + " Channel");
            return true;
        }
        it++;
    }
    return false;
}

void Channel::addClient(client *sender)
{
    if (alreadyIn(sender))
        return ;
    this->_clients.insert(std::make_pair(sender->getFd(), sender));
    log_message_client(sender->getFd(), "You joined " + this->getName() + " Channel");
    log_message(sender->getNick() + " joined the " + this->getName() + " Channel");
}