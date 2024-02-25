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
				ErrorMsgGenrator("IRCserv.1337.ma 412 ", " :No text to send", *this);
				break;
			case 2:
				ErrorMsgGenrator("IRCserv.1337.ma 411 ", " :No recipient given", *this);
				break;
		}
	}
	return false;
}

bool	Client::getDate(vector<string> cmd) {
	
	if (cmd.size() == 1 && cmd[0] == "DATE")
	{
		map<int, Client>::iterator it = Server::getInstance()->getClients().begin();
		map<int, Client>::iterator ite = Server::getInstance()->getClients().end();
		for (; it != ite; it++)
			if (it->second.getNckName() == "Emet")
				break;
		if (it != ite)
			SendMsg(*this, it->second, cmd[0], "DATE ", ":Current time");
		else
			ErrorMsgGenrator((SERVER_NAME + string(" 401 ")), " :Currently out of Service", *this);
		return true;
	}
	ErrorMsgGenrator((SERVER_NAME + string(" 400 ")), " :Invalid Command forma", *this);
	return false;
}

bool	Client::btcPrice(vector<string> cmd) {
	
	if (cmd.size() == 1 && cmd[0] == "BTCPRICE")
	{
		map<int, Client>::iterator it = Server::getInstance()->getClients().begin();
		map<int, Client>::iterator ite = Server::getInstance()->getClients().end();
		for (; it != ite; it++)
			if (it->second.getNckName() == "Emet")
				break;
		if (it != ite)
			SendMsg(*this, it->second, cmd[0], "BTC Price: ", "get Price");
		else
			ErrorMsgGenrator((SERVER_NAME + string(" 401 ")), ":Currently out of Service", *this);
		return true;
	}
	ErrorMsgGenrator((SERVER_NAME + string(" 400 ")), ":Invalid Command forma", *this);
	return false;
}