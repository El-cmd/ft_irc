#include "../include/Channel.hpp"

Channel::Channel(std::string name, client *creator, Server *serv): _name(name), _admin(creator)
{
    log_message("The channel " + name + " has been created by " + creator->getNick());
    log_message_client(creator->getFd(), "You are created the " + name + " Channel");
    _whithKey = false;
    _onInvite = false;
    _limits = false;
    _topicRestricted = false;
    _operator.push_back(creator);
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

std::string Channel::getTopic()
{
    return this->_topic;
}

bool Channel::getOnInvite()
{
    return this->_onInvite;
}

bool Channel::getTopicRestricted()
{
    return this->_topicRestricted;
}

bool Channel::itsAnOp(client *sender)
{
    std::vector<client *>::iterator it = find(_operator.begin(), _operator.end(), sender);
    if (it == _operator.end())
        return false;
    return true;
}
/* +++++++++++++++ */

/* +++ Setter +++ */

void Channel::setTopic(std::string &topic)
{
    this->_topic = topic;
    return ;
}

void Channel::setInviteOnly(bool mode)
{
    this->_onInvite = mode;
}

void Channel::setTopicRestricted(bool mode)
{
    this->_topicRestricted = mode;
}

/* +++++++++++++++ */

bool Channel::alreadyIn(client *sender)
{
    int i = sender->getFd();
    std::map<int, client *>::iterator it = this->_clients.begin();
    while (it != this->_clients.end())
    {
        if (it->first == i)
            return true;
        it++;
    }
    return false;
}

void Channel::addClient(client *sender)
{
    if (alreadyIn(sender))
    {
        log_message_client(sender->getFd(), "You are already in " + this->getName() + " Channel");
        return ;
    }
    this->_clients.insert(std::make_pair(sender->getFd(), sender));
    log_message_client(sender->getFd(), "You joined " + this->getName() + " Channel");
    log_message(sender->getNick() + " joined the " + this->getName() + " Channel");
}