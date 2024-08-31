#include "include/irc.hpp"

bool _state = true;

void handle_sigInt(int sig)
{
	(void)sig;
	_state = false;
}

int main(int ac, char **av)
{
	try
	{
		signal(SIGINT, handle_sigInt);
		if (ac == 3)
		{
			Server Serv(av[1], av[2]);
			Serv.Run();
		}
		else
			throw std::invalid_argument("The ircserv needs port and password for get ready");
	}
	catch (const std::exception& e)
	{
		std::cerr << ROUGE << "Error: " << e.what() << REINIT << std::endl;
	}
}