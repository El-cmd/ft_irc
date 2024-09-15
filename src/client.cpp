#include "../include/client.hpp"

client::client(int fd): _fd(fd)
{
    _auth = false;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    // Utiliser getpeername pour obtenir les infos du client
    if (getpeername(fd, (struct sockaddr*)&addr, &addr_len) == -1)
    {
        this->_client_ip = "Unknown client";
        this->_client_port = -1;
    }
    this->_client_ip = inet_ntoa(addr.sin_addr);
    this->_client_port = ntohs(addr.sin_port);
}

client::~client()
{}

client::client()
{}

/* +++ Getter +++ */

bool client::isRegister()
{
    if (!_auth || _nick.empty() || _username.empty() || _realname.empty())
        return false;
    return true;
}

std::vector<Channel *>& client::getChan()
{
    return this->_channels;
}

int client::getPort()
{
    return this->_client_port;
}

int client::getFd()
{
    return this->_fd;
}

std::string client::getIp()
{
    return this->_client_ip;
}

bool client::getAuth()
{
    return this->_auth;
}

std::string client::getNick()
{
    return this->_nick;
}

std::string client::getUser()
{
    return this->_username;
}

std::string client::getRealname()
{
    return this->_realname;
}
/* ++++++++++++++++++++++++++++++ */

/* +++ Setter +++ */

void client::setNick(std::string str)
{
    this->_nick = str;
}

void client::setUsername(std::string str)
{
    this->_username = str;
}

void client::setRealname(std::string str)
{
    this->_realname = str;
}

void client::setAuth()
{
    this->_auth = true;
}
/* ++++++++++++++++++++++++++++++ */

void client::addNewInvite(Channel *invite)
{
    this->_invitationChannel.push_back(invite);
}