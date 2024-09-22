#include "../include/Channel.hpp"

Channel::Channel(std::string name, client *creator): _name(name)
{
    log_message("The channel " + name + " has been created by " + creator->getNick());
    //log_message_client(creator->getFd(), "You are created the " + name + " Channel");
    _whithKey = false;
    _onInvite = false;
    _limits = false;
    _topicRestricted = false;
    _operator.push_back(creator);
    creator->getChan().push_back(this);
    //serv->newChannel(this);
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

std::map<int, client*>& Channel::getClientsInChan()
{
    return this->_clients;
}

std::string Channel::getKey()
{
    return this->_mdp;
}

size_t Channel::getLimit()
{
    return this->_nbLim;
}

bool Channel::getBoolLimit()
{
    return this->_limits;
}

std::string Channel::getUserList()
{
    std::string userList;
    
    for (std::map<int, client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        client* currentClient = it->second;
        userList += currentClient->getNick();
        if (itsAnOp(currentClient))
            userList = "@" + userList;
        userList += " ";
    }
    if (!userList.empty() && userList[userList.size() - 1] == ' ')
        userList.erase(userList.size() - 1);
    return userList;
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

void Channel::setKey(std::string pswd)
{
    this->_whithKey = true;
    this->_mdp = pswd;
}

void Channel::clearKey()
{
    this->_whithKey = false;
    this->_mdp = "";
}

void Channel::setUserLimit(int limit)
{
    _limits = true;
    _nbLim = limit;
}

void Channel::clearUserLimit()
{
    _limits = false;
    _nbLim = -1;
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
    sender->getChan().push_back(this);
    this->_clients.insert(std::make_pair(sender->getFd(), sender));
    log_message(sender->getNick() + " joined the " + this->getName() + " Channel");
}

void Channel::addOperator(client *newOp)
{
    if (!itsAnOp(newOp))
        _operator.push_back(newOp);
}

void Channel::removeOperator(client *newOp)
{
    if (itsAnOp(newOp))
        _operator.erase(std::remove(_operator.begin(), _operator.end(), newOp), _operator.end());
}

void Channel::channelAllMessage(std::string message)
{
    std::map<int, client*>::iterator it = _clients.begin();
    while (it != _clients.end())
    {
        it->second->sendRpl(message);
        it++;
    }
}

void Channel::channelAllMessageWithoutSender(std::string message, client *sender)
{
    std::map<int, client*>::iterator it = _clients.begin();
    while (it != _clients.end())
    {
        if(it->second != sender)
            it->second->sendRpl(message);
        it++;
    }
}

void Channel::removeClient(client *tokick)
{
    if (_clients.find(tokick->getFd()) != _clients.end())
        _clients.erase(tokick->getFd());
    if (itsAnOp(tokick))
        _operator.erase(std::remove(_operator.begin(), _operator.end(), tokick), _operator.end());
}

