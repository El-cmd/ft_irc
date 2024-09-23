#include "../include/server.hpp"


/* +++ Coplien Form +++ */
// Constructeur par défaut
Server::Server()
{
	std::cout << VERT << "Default constructor are called" << REINIT << std::endl;
}

// Destructeur
Server::~Server()
{
	_state = false;
	std::cout << ROUGE << "ircserv off" << REINIT << std::endl;
}

// Constructeur paramétré
Server::Server(char *port, char *pass)
{
	SecurArg(port, pass);
}
/* ++++++++++++++++++++++++ */


/* +++ Securise les args +++ */
// Méthode pour sécuriser les arguments fournis (port et mot de passe)
void Server::SecurArg(const char *port, const char *pass)
{
	int i = 0;
	std::string sPort = port;
	std::string sPass = pass;

	// Vérifie si les arguments sont vides
	if (sPort.empty() || sPass.empty())
		throw std::invalid_argument("The arguments are empty");

	// Vérifie si le port ne contient que des chiffres
	while (port[i])
	{
		if (!isdigit(port[i]))
			throw std::invalid_argument("The connection port needs just digit");
		i++;
	}

	// Conversion du port en entier long et vérification des limites
	long int tmp = atol(sPort.c_str());
	if (tmp > INT_MAX)
		throw std::overflow_error("Alert: Int overflow");
	else if (tmp == 0)
		throw std::invalid_argument("The connection port can't take zero for value");

	// Assignation des valeurs sécurisées
	this->_port = tmp;
	this->_pass = sPass;
	log_message("Server started");
	//std::cout << VERT << "ircserv on" << REINIT << std::endl;
}
/* ++++++++++++++++++++++++ */

/* +++ Getter +++ */
client *Server::findClient(std::string name)
{
	std::map<int, client*>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNick() == name)
			return it->second;
		it++;
	}
	return NULL;
}

bool Server::clientExist(std::string name)
{
	std::map<int, client*>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNick() == name)
			return true;
		it++;
	}
	return false;
}

/* ++++++++++++++++++++++++ */


/* +++ Init le socket original +++ */
// Initialisation du socket
void Server::initSock()
{
	// Création du socket
	this->_sockFdToListen = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockFdToListen == -1)
		throw std::runtime_error("(SERVER) echec d'initialisation du socket");
	//Configuration de l'option pour fermer le port imeddiatement apres la fin du programme
	int opt = 1;
    if (setsockopt(_sockFdToListen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		throw std::runtime_error("(SERVER) echec setsockopt");

	// Configuration de l'adresse du socket
	this->socket_adress.sin_family = AF_INET;
	this->socket_adress.sin_port = htons(this->_port);
	this->socket_adress.sin_addr.s_addr = INADDR_ANY;
	int socketAdresslenght = sizeof(this->socket_adress);

	// Liaison du socket
	int bind_adress = bind(this->_sockFdToListen, (struct sockaddr*) &this->socket_adress, socketAdresslenght);
	if (bind_adress == -1)
		throw std::runtime_error("(SERVER) echec de liaison au socket");

	// Mise en écoute du socket
	if (listen(this->_sockFdToListen, 1) == -1)
		throw std::runtime_error("(SERVER) echec de l'ecoute des connexion entrante\n");
}
/* ++++++++++++++++++++++++ */


/* +++ POLL +++ */
// Initialisation de poll pour surveiller les événements sur le socket
void Server::initPoll()
{
	pollfd pfd = {this->_sockFdToListen, POLLIN, 0};
	this->_pfd.push_back(pfd);
}

void Server::client_connection() {
    // Définit la taille de la structure d'adresse du socket.
    socklen_t addrlen = sizeof(this->socket_adress);

    // Attend une connexion sur le socket d'écoute (_sockFdToListen).
    // accept() bloque le processus jusqu'à ce qu'un client se connecte.
    // Il retourne un nouveau descripteur de fichier (socket) pour cette connexion.
    int connectedSockFd = accept(this->_sockFdToListen, (struct sockaddr*) &this->socket_adress, &addrlen);

    // Si accept() retourne -1, cela signifie qu'une erreur a eu lieu lors de la tentative de connexion.
    if (connectedSockFd == -1)
        throw std::runtime_error("(SERVER) Echec d'etablissement de la conexion");

    // Prépare un struct pollfd pour utiliser avec poll().
    // POLLIN indique que nous attendons des données en lecture sur ce socket.
    pollfd pfd = {connectedSockFd, POLLIN, 0};

    // Ajoute ce nouveau pollfd au vecteur de pollfd géré par le serveur.
    this->_pfd.push_back(pfd);

    // Crée un nouvel objet client qui encapsule le descripteur de fichier du socket connecté.
    // Supposons que 'client' est une classe qui gère la communication avec un client connecté.
    client *Nclient = new client(connectedSockFd);

    // Ajoute le nouveau client à la map des clients avec son descripteur de fichier comme clé.
    // std::make_pair est utilisé pour créer la paire qui sera insérée dans la map.
    this->_clients.insert(std::make_pair(connectedSockFd, Nclient));
	log_message("Client from " + Nclient->getIp() + " connected but not authenticate");
}


/* ++++++++++++++++++++++++ */
/* +++ Pass +++ */

bool Server::auth(const std::string &Pass)
{
	if (this->_pass == Pass)
		return true;
	return false;
}

/* ++++++++++++++++++++++++ */

/* +++ Gerer les channels  +++ */

void Server::newChannel(Channel *chan)
{
	this->_channels.push_back(chan);
}

bool Server::isValidNick(std::string name)
{
	std::map<int, client*>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (name == it->second->getNick())
			return false;
		it++;
	}
	return true;
}

bool Server::channelAlreadyExist(std::string name)
{
	std::vector<Channel *>::iterator it = this->_channels.begin();
	while (it != this->_channels.end())
	{
		if ((*it)->getName() == name)
			return true;
		it++;
	}
	return false;
}

bool Server::chanNeedPswd(std::string name)
{
	std::vector<Channel *>::iterator it = this->_channels.begin();
	while (it != this->_channels.end())
	{
		if ((*it)->getName() == name)
			return (*it)->withKey();
		it++;
	}
	return false;
}



Channel *Server::findChan(std::string name)
{
	std::vector<Channel *>::iterator it = this->_channels.begin();
	while (it != this->_channels.end())
	{
		if ((*it)->getName() == name)
			return *it;
		it++;
	}
	return NULL;
}

/* ++++++++++++++++++++++++ */

/* +++ Handle Message +++ */

std::string Server::read_message(int fd)
{
    std::string message;
    char buffer[100];
    int bytes_received;

    while (true)
    {
        bzero(buffer, 100);
        bytes_received = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0)
		{
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                        continue;
            throw std::runtime_error("Error while reading buffer from a client!");
		}
        else if (bytes_received == 0)
		{
            log_message(_clients[fd]->getNick() + " disconnected (double Ctrl+D or connection closed)");
			handleClientDeconnectionQUIT(_clients[fd]);
        	return message;
        }
        message.append(buffer, bytes_received);
        if (message.find("\n") != std::string::npos || message.find("\r\n") != std::string::npos)
            break;
    }
    return message;
}

void Server::handle_client_message(int fd)
{
	//char buffer[512];
	//memset(buffer, 0, sizeof(buffer));
	//int bytes_received = recv(fd, buffer, sizeof(buffer) - 1, 0);
	//if (bytes_received < 0)
	//	return ;
	//else if (bytes_received == 0)
	//{
	//	close(fd);
	//	return ;
	//}
	//buffer[bytes_received] = '\0';
	std::string buffer = read_message(fd);
	if (buffer.empty())
		return ;
	std::map<int, client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return ;
	Command command;
	command.execute(buffer, it->second, this);
}
/* ++++++++++++++++++++++++ */

/* +++ Evenement POLLHUP et client QUIT +++ */

void Server::handleClientDeconnectionQUIT(client *sender)
{
	//sendQuitMessageToAllChannels(sender, nullptr);
    int fd = sender->getFd();  // Récupérer le descripteur avant suppression

    // Supprimer l'entrée pollfd pour le client
    for (size_t i = 0; i < _pfd.size(); ++i) {
        if (_pfd[i].fd == fd) {
            _pfd.erase(_pfd.begin() + i);
            break;
        }
    }
    // Supprimer le client de la map des clients
    _clients.erase(fd);
    // Fermer la socket
    close(fd);
    delete sender;
    // Libérer la mémoire du client
}

void Server::deleteAllChannels(std::vector<Channel*>& channels)
{
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
        if (*it != NULL)
		{
            delete *it;  // Libérer la mémoire
            *it = NULL;  // Éviter les doubles suppressions
        }
    }
    channels.clear();  // Vider le vecteur
}

/* ++++++++++++++++++++++++ */
/* +++ SERV RUN  +++ */
// Méthode principale de l'exécution du serveur
void Server::Run()
{
	initSock();
	initPoll();
	while (_state)
	{
		// Surveillance des événements sur le socket
		if (poll(this->_pfd.begin().base(), _pfd.size(), -1) < 0 && _state)
			throw std::runtime_error("Error: polling init");

		// Parcourt les événements du poll
		for(_itpfd tmp = _pfd.begin(); tmp != _pfd.end() && _state;)
		{
			if (_pfd.empty())
				continue;
			if (tmp->revents == 0)
			{
				++tmp;
				continue; 
			}
			// Gestion de l'événement POLLHUP (fermeture de connexion)
			if ((tmp->revents & POLLHUP) == POLLHUP)
			{
				int fd = tmp->fd;
				++tmp;
				//tmp = _pfd.erase(tmp);
				if (_clients.find(fd) != _clients.end())
				{
					client* clientToDisconnect = _clients[fd];
					handleClientDeconnectionQUIT(clientToDisconnect);
				}
				continue; 
			}
			// Gestion de l'événement POLLIN (données disponibles en lecture)
			if ((tmp->revents & POLLIN) == POLLIN)
			{
				if (tmp->fd == this->_sockFdToListen)
				{
					client_connection();
					break;
				}
				handle_client_message(tmp->fd);
				++tmp;
				break;
			}
		}
	}
	std::map<int, client*>::iterator free = _clients.begin();
	if (this->_clients.size() > 0)
	{
		while (_clients.begin() != _clients.end())
		{
			handleClientDeconnectionQUIT(free->second);
			free = _clients.begin();
		}
	}
	close(this->_sockFdToListen);
	if (!this->_channels.empty())
		deleteAllChannels(_channels);
}
/* ++++++++++++++++++++++++ */
