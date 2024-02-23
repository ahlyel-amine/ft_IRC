#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

bool	isValidChannel(string channel)
{
	if (channel.size() == 1 && channel[0] == '0')
		return (true);
	if (channel.size() < 2 || channel.size() > 51 || channel[0] != '#')
		return (false);
	channel.erase(0, 1);
	for (size_t i = 0; i < channel.size(); i++)
	{
		if (channel[i] == '\0' || channel[i] == '\a' || channel[i] == '\r' || channel[i] == '\n' || channel[i] == ' ' || channel[i] == ',' || channel[i] == ':')
			return (false);
	}
	return (true);
}
bool	Client::Topic(vector<string> cmd) {
	
	if (cmd.size() < 2)
	{
		ErrorMsgGenrator(":ircserv 461 ", " " + cmd[0] + " :Not enough parameters", *this);
		return true;
	}
	if (cmd.size() == 2)
	{
		
		map<string, Channel*>::iterator it = Server::getInstance()->getChannels().find(cmd[1]);
		if (it != Server::getInstance()->getChannels().end())
		{
			if (it->second->isMember(this))
			{
				if (it->second->getTopic().empty())
					ErrorMsgGenrator(":ircserv 331 ", " " + cmd[0] + " :No topic is set", *this);
				else
					ErrorMsgGenrator(":ircserv 332 ", " " + cmd[0] + " :" + it->second->getTopic(), *this);
			}
			else
				ErrorMsgGenrator(":ircserv 403 ", " " + cmd[0] + " :You are not on this channel", *this);
		}
		else
			ErrorMsgGenrator(":ircserv 403 ", " " + cmd[0] + " :No such channel", *this);
		return true;
	}
	if (cmd.size() == 3)
	{
		map<string, Channel*>::iterator it = Server::getInstance()->getChannels().find(cmd[1]);
		if (it != Server::getInstance()->getChannels().end())
		{
			string msg;
			if (it->second->isMember(this))
			{
				msg = ":" + this->NckName + "!" + this->RlName + "@" + this->HstName + " TOPIC " + cmd[1] + " ";
				if (it->second->isTopic() && it->second->isOperator(this))
				{
					if (cmd[2].size() == 1)
					{
						it->second->setTopic("");
						cmd[2].erase(0, 1);
					}	
					else
						it->second->setTopic(cmd[2]);	
					for (size_t i = 0; i < it->second->getMembers().size(); i++)
					{
						msg += cmd[2] + "\r\n";
						send(it->second->getMembers()[i]->getClntFd(), msg.c_str(), msg.size(), 0);
					}	
				}
				else if (!it->second->isTopic())
				{
					if (cmd[2].size() == 1)
					{
						it->second->setTopic("");
						cmd[2].erase(0, 1);
					}	
					else
						it->second->setTopic(cmd[2]);	
					for (size_t i = 0; i < it->second->getMembers().size(); i++)
					{
						msg += cmd[2] + "\r\n";
						send(it->second->getMembers()[i]->getClntFd(), msg.c_str(), msg.size(), 0);
					}	
				}
				else
					ErrorMsgGenrator(":ircserv 482 ", " " + cmd[0] + " :You're not channel operator", *this);
			}
			else
					ErrorMsgGenrator(":ircserv 482 ", " " + cmd[0] + " :You're not on the channel", *this);
				
		}
		else
			ErrorMsgGenrator(":ircserv 403 ", " " + cmd[0] + " :No such channel", *this);
	}
	return false;
}