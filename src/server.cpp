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
	std::cout << VERT << "ircserv on" << REINIT << std::endl;
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
	log_message("client connecté");
}

/* ++++++++++++++++++++++++ */

/* +++ Handle Message +++ */

void Server::handle_client_message(int fd)
{
	char buffer[512];
	memset(buffer, 0, sizeof(buffer));
	int bytes_received = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
		return ;
	buffer[bytes_received] = '\0';
	std::cout << buffer;
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
		if (poll(this->_pfd.begin().base(), _pfd.size(), -1) < 0)
			throw std::runtime_error("Error: polling init");

		// Parcourt les événements du poll
		for(_itpfd tmp = _pfd.begin(); tmp != _pfd.end(); tmp++)
		{
			if (tmp->revents == 0)
				continue ;
			// Gestion de l'événement POLLHUP (fermeture de connexion)
			if ((tmp->revents & POLLHUP) == POLLHUP)
			{
				break ;
			}
			// Gestion de l'événement POLLIN (données disponibles en lecture)
			if ((tmp->revents & POLLIN) == POLLIN)
			{
				if (tmp->fd == this->_sockFdToListen)
				{
					client_connection();
					break ;
				}
				handle_client_message(tmp->fd);
			}

		}
	}
}
/* ++++++++++++++++++++++++ */



