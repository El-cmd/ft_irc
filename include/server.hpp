#ifndef SERVER_HPP
	#define SERVER_HPP

#include "irc.hpp"

class Server
{
	typedef std::vector<pollfd>::iterator _itpfd;


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


	/* +++ POLL +++ */
		void initPoll();
		void client_connection();
	/* + + + + + + + + + + + */

	/* +++ Verif Arguments +++ */
		void SecurArg(const char *port, const char *pass);
	/* + + + + + + + + + + + */

	/* +++ variables +++ */
		std::vector<struct pollfd> _pfd;
		std::map<int, client *> _clients;
		int	_sockFdToListen;
		int _port;
		std::string _pass;
		struct sockaddr_in socket_adress;

};

#endif