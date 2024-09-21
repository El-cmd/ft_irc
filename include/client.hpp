#ifndef CLIENT_HPP
	#define CLIENT_HPP

#include "irc.hpp"

class client 
{
    public:

    /* RPL*/
    std::string get_prefix() const;
    void sendRpl(const std::string &message);
    void write(const std::string &message) const;
    void sendchat(const std::string& reply, client* sender);
    /* + + + + + + + + + + + */
    client(int fd);
    ~client();
    /* +++ Getter +++ */
    bool isInvited(Channel *chan);
    int getFd();
    std::string getIp();
    int getPort();
    bool getAuth();
    std::string getNick();
    std::string getUser();
    std::string getRealname();
    std::vector<Channel *>& getChan();
    std::vector<Channel *>& getInviteChan();
    bool isRegister();
    /* + + + + + + + + + + + */
    /* +++ Setter +++ */
    void setNick(std::string str);
    void setUsername(std::string str);
    void setRealname(std::string str);
    void setAuth();
    void addNewInvite(Channel *invite);
    /* + + + + + + + + + + + */
    private:

    /* +++ Coplian Form +++ */
    client();
    client &operator=(client const &other);
    client(client const &other);
    /* + + + + + + + + + + + */

    /* +++ variable +++ */
    int _fd;
    std::string _client_ip;
    int _client_port;
    //Pour la connection au seveur
    bool _auth;
    std::string _nick;
    std::string _username;
    std::string _hostname;
    std::string _domain;
    std::string _realname;
    std::vector<Channel *> _channels;
    std::vector<Channel *> _invitationChannel; // push les invitation channel dedans
    /* + + + + + + + + + + + */
};

#endif