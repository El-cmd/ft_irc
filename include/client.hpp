#ifndef CLIENT_HPP
	#define CLIENT_HPP

#include "irc.hpp"

class client 
{
    public:
    client(int fd);
    ~client();

    private:
    /* +++ Coplian Form +++ */
    client();
    client &operator=(client const &other);
    client(client const &other);
    /* + + + + + + + + + + + */

    /* +++ variable +++ */
    int _fd;

    //Pour la connection au seveur
    bool _auth;
    std::string _nick;
    std::string _username;
    std::string _realname;
    /* + + + + + + + + + + + */
};

#endif