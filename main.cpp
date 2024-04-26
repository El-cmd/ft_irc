#include "include/irc.hpp"

int main(int ac, char **av)
{
	try
	{
		if (ac == 3)
		{
			Server Serv(av[1], av[2]);
		}
		else
			throw std::invalid_argument("The ircserv needs port and password for get ready");

	}
	catch (const std::exception& e)
	{
		std::cerr << ROUGE << "Error: " << e.what() << REINIT << std::endl;
	}
}