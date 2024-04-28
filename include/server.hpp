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

	/* +++ Verif Arguments +++ */
		void SecurArg(const char *port, const char *pass);
	/* + + + + + + + + + + + */

		int _port;
		std::string _pass;

};

#endif