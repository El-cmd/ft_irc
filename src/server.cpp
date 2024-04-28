#include "../include/server.hpp"


/* +++ Coplien Form +++ */
Server::Server()
{
	std::cout << VERT << "Default constructor are called" << REINIT << std::endl;
}

Server::~Server()
{
	this->_state = false;
	std::cout << ROUGE << "ircserv off" << REINIT << std::endl;
}

Server::Server(char *port, char *pass)
{
	SecurArg(port, pass);
	this->_state = true;
}

/* ++++++++++++++++++++++++ */


/* +++ Securise les args +++ */

void Server::SecurArg(const char *port, const char *pass)
{
	int i = 0;
	std::string sPort = port;
	std::string sPass = pass;
	if (sPort.empty() || sPass.empty())
		throw std::invalid_argument("The arguments are empty");
	while (port[i])
	{
		if (!isdigit(port[i]))
			throw std::invalid_argument("The connection port needs just digit");
		i++;
	}

	long int tmp = atol(sPort.c_str());
	if (tmp > INT_MAX)
		throw std::overflow_error("Alert: Int overflow");
	else if (tmp == 0)
		throw std::invalid_argument("The connection port can't take zero fir value");
	this->_port = tmp;
	this->_pass = sPass;
	std::cout << VERT << "ircserv on" << REINIT << std::endl;
}

/* ++++++++++++++++++++++++ */

/* +++ Handle_signal +++ */
void Server::handle_sigInt(int sig)
{
	std::cout << ROUGE << "ircserv off" << REINIT << std::endl;
	exit(sig);
}
/* ++++++++++++++++++++++++ */

/* +++ SERV RUN  +++ */

void Server::Run()
{
	signal(SIGINT, handle_sigInt);
	while (this->_state)
	{
		;
	}
}

/* ++++++++++++++++++++++++ */

