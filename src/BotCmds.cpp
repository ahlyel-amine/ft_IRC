#include "Client.hpp"
#include "Server.hpp"

void	Client::sendBotMsg(Client* Sender,string& target, vector<string>& cmd)
{
	map<int, Client>::iterator	startIt = Server::getInstance()->getClients().begin();
	map<int, Client>::iterator	endIt = Server::getInstance()->getClients().end();
	for (; startIt != endIt; startIt++)
		if (startIt->second.NckName == target)
			break;
	if (startIt != endIt)
		SendMsg(*Sender, startIt->second, "PRIVMSG", cmd[2], target);
	else
		ErrorMsgGenrator(":IRCserv.1337.ma 401 ", " " + target + " :No such nick", *this);
}

bool	Client::anonyMsg(vector<string> cmd)
{
	if (cmd.size() == 3)
	{
		map<int, Client>::iterator it = Server::getInstance()->getClients().begin();
		map<int, Client>::iterator ite = Server::getInstance()->getClients().end();
		for (; it != ite; it++)
			if (it->second.getNckName() == "AnonyMsg")
				break;
		if (it != ite)
		{
			vector<string> recievers = getTargets(cmd[1], ',');
			for (uint16_t iter = 0; iter < recievers.size(); iter++)
				sendBotMsg(&(it->second), recievers[iter], cmd);
		}
		else
			ErrorMsgGenrator((SERVER_NAME + string(" 401 ")), " :Currently out of Service", *this);
		
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