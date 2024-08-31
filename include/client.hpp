#ifndef CLIENT_HPP
	#define CLIENT_HPP

#include "irc.hpp"

class client 
{
    public:

    client(int fd);
    ~client();
    /* +++ Getter +++ */
    int getFd();
    std::string getIp();
    int getPort();
    bool getAuth();
    std::string getNick();
    std::string getUser();
    std::string getRealname();
    std::vector<Channel *> getChan();
    /* + + + + + + + + + + + */
    /* +++ Setter +++ */
    void setNick(std::string str);
    void setUsername(std::string str);
    void setRealname(std::string str);
    void setAuth();
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
    std::string _realname;
    std::vector<Channel *> _channels;
    /* + + + + + + + + + + + */
};

#endif