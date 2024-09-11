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
		bool auth(const std::string &Pass);

	/* +++ Gerer les channels +++ */
		void newChannel(Channel *chan);
		bool channelAlreadyExist(std::string name);
		bool chanNeedPswd(std::string name);
		Channel *findChan(std::string name);
	/* + + + + + + + + + + + */
	/* +++ Getter +++ */
		client *findClient(std::string name);
	/* + + + + + + + + + + + */
	/* +++ Evenement POLLHUP et client QUIT +++ */
		void handleClientDeconnectionQUIT(client *sender);
	/* + + + + + + + + + + + */
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
	/* + + + + + + + + + + + */

	/* +++ Verif Arguments +++ */
		void SecurArg(const char *port, const char *pass);
	/* + + + + + + + + + + + */

	/* +++ Evenement POLLIN +++ */
		void handle_client_message(int fd);
		void client_connection();
	/* + + + + + + + + + + + */

	/* +++ variables +++ */
		std::vector<struct pollfd> _pfd;
		std::map<int, client *> _clients;
		int	_sockFdToListen;
		int _port;
		std::string _pass;
		struct sockaddr_in socket_adress;
		std::vector<Channel *> _channels;
	/* + + + + + + + + + + + */
};

#endif