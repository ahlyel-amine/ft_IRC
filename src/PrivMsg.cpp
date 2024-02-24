#include"Client.hpp"

/**
 * @brief Splits a string into multiple targets based on comma delimiter.
 * 
 * This function takes a string as input and splits it into multiple targets based on the comma delimiter.
 * Each target is stored as a separate element in the returned vector.
 * 
 * @param str The input string to be split into targets.
 * @return vector<string> A vector containing the individual targets.
 */

vector<string> getTargets(string str, char sep) {
    vector<string> trg;
    string tmp;
    for (size_t i = 0; i < str.size(); i++){
        if (str.at(i) != sep)
            tmp += str.at(i);
        if (str.at(i) == sep || i+1 == str.size()) {
            trg.push_back(tmp);
            tmp = "";
        }
    }
    return trg;
}


/**
 * @brief Sends a private message to one or more recipients.
 * 
 * This function sends a private message to one or more recipients. It checks if the client is registered
 * and then determines whether the message is intended for a channel or a specific client. If the message
 * is intended for a channel, it checks if the channel exists and if the client is a member of the channel.
 * If both conditions are met, the message is sent to all other members of the channel. If the message is
 * intended for a specific client, it checks if the client exists and sends the message directly to that client.
 * 
 * @param cmd The command containing the message and recipient(s).
 * @return Returns true if the message was sent successfully, false otherwise.
 */

void	Client::sendChannelMsg(string& target, vector<string>& cmd)
{
	map<string, Channel*>::iterator eit = Server::getInstance()->getChannels().end();
	if (Server::getInstance()->getChannels().find(target) != eit)
	{
		if (find(this->Chnls.begin(), this->Chnls.end(), target) != this->Chnls.end())
		{
			vector<Client *> &channelMembers = Server::getInstance()->getChannels()[target]->getMembers();
			for (uint16_t j = 0; j < channelMembers.size(); ++j)
			{
				if (channelMembers[j]->ClntFd != this->ClntFd)
					SendMsg(*this, *channelMembers[j], cmd[0], cmd[2], target);
			}
			// SendMsg(*this, *Server::getInstance()->getChannel(target), cmd[0], cmd[2], target);
		}
		else
			ErrorMsgGenrator(":IRCserv.1337.ma 404 ", " " + target + " :Cannot send to channel", *this);
	}
	else
		ErrorMsgGenrator(":IRCserv.1337.ma 403 ", " " + target + " :No such channel", *this);
}

void	Client::sendClientMsg(string& target, vector<string>& cmd)
{
	map<int, Client>::iterator	startIt = Server::getInstance()->getClients().begin();
	map<int, Client>::iterator	endIt = Server::getInstance()->getClients().end();
	for (; startIt != endIt; startIt++)
		if (startIt->second.NckName == target)
			break;
	if (startIt != endIt)
		SendMsg(*this, startIt->second, cmd[0], cmd[2], target);
	else
		ErrorMsgGenrator(":IRCserv.1337.ma 401 ", " " + target + " :No such nick", *this);
}

bool		  Client::SendPrvMsg(vector<string> cmd) {
    bool           rt = true;
    
    if (cmd.size() == 3) {
        vector<string> targets = getTargets(cmd[1], ',');
        for (size_t i = 0; i < targets.size(); i++){
		    if (*targets[i].begin() == '#')
               this->sendChannelMsg(targets[i], cmd);
            else
               this->sendClientMsg(targets[i], cmd);
		}
    }
    else if (cmd.size() <= 2) {
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
        rt = false;
    }
    return rt;
}
