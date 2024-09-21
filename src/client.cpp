#include "../include/client.hpp"

client::client(int fd): _fd(fd)
{
    _auth = false;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    _hostname = "localhost";
    _domain = "ft_irc";
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

bool client::isInvited(Channel* chan)
{
    std::vector<Channel *>::iterator it = this->_invitationChannel.begin();
    while (it != this->_invitationChannel.end())
    {
        if (chan == *it)
            return true;
        it++;
    }
    return false;
}

std::vector<Channel *>& client::getInviteChan()
{
    return this->_invitationChannel;
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

/* +++ RPL +++ */

std::string     client::get_prefix() const 
{
    std::string username = _username.empty() ? "" : "!" + _username;
    std::string hostname = _hostname.empty() ? "" : "@" + _hostname;

    return _nick + username + hostname;
}

void    client::sendRpl(const std::string& reply)
{
    this->write(":" + get_prefix() + " " + reply);
}

void    client::sendchat(const std::string& reply, client* sender)
{
    this->write(":" + sender->get_prefix() + " " + reply);
}

void    client::write(const std::string &message) const
{
    std::string buff = message + "\r\n";
    if (send(_fd, buff.c_str(), buff.size(), 0) < 0)
        throw std::runtime_error("Error while sending message");
}

/* ++++++++++++++++++++++++++++++ */