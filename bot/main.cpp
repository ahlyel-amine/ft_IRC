#include "Bot.hpp"
#include "Informer.hpp"
#include "PrvMsg.hpp"
#include <sstream>


bool	ParseFirstReplay(std::string reply)
{
	int pos = reply.find_first_of(' ');
	reply.erase(0, pos + 1);
	reply.erase(3, reply.size());
	if (reply != "001")
		return false;
	return true;
}
int main(int ac, char **av)
{
	char buf[4096];
	int nbytes;
	Bot *bot;
	bool reg = false;
	if (ac != 5)
	{
		std::cerr << "Usage: " << av[0] << " <hostname> <port> <pass> <bottype>" << std::endl;
		std::cerr << "\t\tbottype: 1 - Emet, 2 - privmsg" << std::endl;
		exit(1);
	}
	bot = bot_init(av);
	while (bot)
	{
		memset(buf, 0, sizeof(buf));
		if ((nbytes = recv(bot->getSocketFd(), buf, sizeof(buf) - 1, 0)) <= 0)
		{
			if (nbytes == 0)
				std::cerr << "Connection closed" << std::endl;
			else
				std::cerr << "recv: " << strerror(errno) << std::endl;
			close(bot->getSocketFd());
			exit(1);
		}
		else
		{
			if (!reg)
			{
				std::cerr << buf << std::endl;
				reg = true;
				if (!ParseFirstReplay(buf))
				{
					close(bot->getSocketFd());
					delete bot;
					return 1;
				}
			}
			bot->botReply(buf);
		}
	}
	return 0;
}

//prive message