#include "../include/server.hpp"

Server::Server()
{
	std::cout << VERT << "Default constructor are called" << REINIT << std::endl;
}

Server::~Server()
{
	std::cout << ROUGE << "ircserv off" << REINIT << std::endl;
}

/* +++ Securise les arg +++ */

void Server::SecurArg(char *port, char *pass)
{
	int i = 0;
	std::string sPort = port;
	std::string sPass = pass;
	if (sPort.empty() || sPass.empty())
		throw std::invalid_argument("The arguments are empty");
	// if (sPort > "2147483647")
		// throw std::invalid_argument("Conection port invalid");
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

Server::Server(char *port, char *pass)
{
	SecurArg(port, pass);
}

