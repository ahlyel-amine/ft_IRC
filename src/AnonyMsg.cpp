#include "Client.hpp"
#include "Server.hpp"

bool	Client::anonyMsg(vector<string> cmd)
{
	if (cmd.size() == 3)
	{
		vector<string> recievers = getTargets(cmd[1], ',');
		for (uint16_t iter = 0; iter < recievers.size(); iter++)
			sendClientMsg(recievers[iter], cmd);
		return true;
	}
	else if (cmd.size() <= 2)
	{
		uint8_t	failReplay = (cmd.size() == 2) * 1 + (cmd.size() < 2) * 2;
		switch (failReplay)
		{
			case 1:
				ErrorMsgGenrator(":IRCserv.1337.ma 412 ", " :No text to send", *this);
				break;
			case 2:
				ErrorMsgGenrator(":IRCserv.1337.ma 411 ", " :No recipient given", *this);
				break;
		}
	}
	return false;
}