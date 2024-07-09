#ifndef CLIENT_HPP
	#define CLIENT_HPP

#include "../include/irc.hpp"

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
    /* + + + + + + + + + + + */
};

#endif