#ifndef IRC_HPP
	#define IRC_HPP

#include <fstream>
#include <ctime>
#include <sstream>
#include <iostream>
#include <csignal>
#include <set>
#include <string>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <poll.h>
#include <map>
#include <stack>
#include <cerrno>
#include <climits>
#include <algorithm>
#include <exception>
#include <arpa/inet.h>
#include <limits.h>
#include <vector>
#include <unistd.h>
#include <cstdlib> //pour atoi()

class client;
class Server;
class Channel;
#include "Channel.hpp"
#include "Command.hpp"
#include "utils.hpp"
#include "server.hpp"
#include "client.hpp"

/* +++ COLORS +++ */
#define REINIT "\033[0m"
#define NOIR "\033[30m"
#define ROUGE "\033[31m"
#define VERT "\033[32m"
#define JAUNE "\033[33m"
#define BLEU "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define BLANC "\033[37m"



/* +++ Global +++ */
extern bool _state;

#endif