#ifndef SERVER_HPP
	#define SERVER_HPP

#include "irc.hpp"

class Server
{
	public:
		~Server();
		Server(char *port, char *pass);
		void Run();

	private:

	/* +++ Coplian Form +++ */
		Server();
		Server &operator=(const Server &other);
		Server(const Server &other);
	/* + + + + + + + + + + + */

	/* +++ Init_socket +++ */
		void initSock();
	/* + + + + + + + + + + + */

	/* +++ Verif Arguments +++ */
		void SecurArg(const char *port, const char *pass);
	/* + + + + + + + + + + + */

		int	_sockFdToListen;
		int _port;
		std::string _pass;
		struct sockaddr_in socket_adress;

};

#endif