#include "Informer.hpp"
#include "PrvMsg.hpp"
#include <sstream>

typedef Bot* (*botCreator[])(std::string host, std::string port, std::string pass,BotType type);

Bot *createInfBot(std::string host, std::string port, std::string pass, BotType type)
{
	return new Informer(host, port, pass, type);
}

Bot *createPrvMsgBot(std::string host, std::string port, std::string pass, BotType type)
{
	return new PrvMsg(host, port, pass, type);
}
Bot *skip(std::string host, std::string port, BotType type)
{
	(void)host;
	(void)port;
	(void)type;
	return NULL;
}
Bot* createBot(std::string host, std::string port, std::string pass, BotType type)
{
	botCreator creators = {&createInfBot, &createPrvMsgBot};
	int8_t funcPosition = (type == GETPRICE) * 1 + (type == ANNOMSG) * 2;
	return creators[funcPosition - 1](host, port, pass, type);
}

Bot* bot_init(char **argv)
{
	int16_t sockFd;
	BotType bType = static_cast<BotType>(std::stoi(*(argv + 4)));
	if (bType != GETPRICE && bType != ANNOMSG)
	{
		std::cerr << "Invalid bot type" << std::endl;
		exit(1);
	}
	Bot *bot = createBot(*(argv + 1), *(argv + 2), *(argv + 3), bType);
	sockFd = connectToServer(*bot);
	if (sockFd == -1)
	{
		std::cerr << "Connection failed" << std::endl;
		exit(1);
	}
	// bot Authorize
	if (!bot->autoRegister())
	{
		std::cerr << "Server connection Authorization failed" << std::endl;
		delete bot;
		close(sockFd);
		bot = NULL;
	}
	return (bot);
}

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
	bool reg = false;
	if (ac != 5)
	{
		std::cerr << "Usage: " << av[0] << " <hostname> <port> <pass> <bottype>" << std::endl;
		std::cerr << "\t\tbottype: 1 - btcprice, 2 - privmsg" << std::endl;
		exit(1);
	}
	Bot *bot = bot_init(av);

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