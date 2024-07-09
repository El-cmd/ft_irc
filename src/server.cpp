#include "../include/server.hpp"


/* +++ Coplien Form +++ */
Server::Server()
{
	std::cout << VERT << "Default constructor are called" << REINIT << std::endl;
}

Server::~Server()
{
	_state = false;
	std::cout << ROUGE << "ircserv off" << REINIT << std::endl;
}

Server::Server(char *port, char *pass)
{
	SecurArg(port, pass);
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


/* +++ Init le socket original ;) +++ */

// !!!!!! Fonction un peu brouillon a modifier selon !!!!!!

void Server::initSock()
{
	this->_sockFdToListen = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockFdToListen == -1)
		throw std::runtime_error("(SERVER) echec d'initialisation du socket");
	this->socket_adress.sin_family = AF_INET;
	this->socket_adress.sin_port = htons(this->_port);
	this->socket_adress.sin_addr.s_addr = INADDR_ANY;
	int socketAdresslenght = sizeof(this->socket_adress);
	int bind_adress = bind(this->_sockFdToListen, (struct sockaddr*) &this->socket_adress, socketAdresslenght);
	if (bind_adress == -1)
		throw std::runtime_error("(SERVER) echec de liaison au socket");
	if (listen(this->_sockFdToListen, 1) == -1)
		throw std::runtime_error("(SERVER) echec de l'ecoute des connexion entrante\n");
}

/* ++++++++++++++++++++++++ */

/* +++ POLL +++ */
void Server::initPoll()
{
	pollfd pfd = {this->_sockFdToListen, POLLIN, 0};
	this->_pfd.push_back(pfd);
}
/* ++++++++++++++++++++++++ */

/* +++ SERV RUN  +++ */
// !!!!!! Fonction un peu brouillon a modifier selon !!!!!!
void Server::Run()
{
	initSock();
	initPoll();
	//socklen_t addrlen = sizeof(this->socket_adress);
	//int connectedSockFd = accept(this->_sockFdToListen, (struct sockaddr*) &this->socket_adress, (socklen_t *) &addrlen);
	//if (connectedSockFd == -1)
	//	throw std::runtime_error("(SERVER) Echec d'etablissement de la conexion");
	std::cout << VERT << "SERVEUR CONNECTED AU CLIENT IRSSI" << REINIT << std::endl;
	while (_state)
	{
		if (poll(this->_psfd.begin().base(), _pfd.size(), -1) < 0)
			throw std::runtime_error("Error: polling intit");
		
	}
}

/* ++++++++++++++++++++++++ */


