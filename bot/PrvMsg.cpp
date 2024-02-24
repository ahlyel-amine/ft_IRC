#include "PrvMsg.hpp"

PrvMsg::PrvMsg(std::string host, std::string port, std::string pass, BotType type)
 : Bot("AnonyMsg", host, port, pass, type) {}

PrvMsg::~PrvMsg() {}

// PrvMsg::PrvMsg( const PrvMsg& ) {}

PrvMsg& PrvMsg::operator=( const PrvMsg& ) { return *this; }


void	PrvMsg::botReply(std::string msg)
{
	std::string	cmd, nick, Msg, reciever, tmpMsg;
	std::stringstream	ss(msg);
	int sendBytes;
	if (msg.size() <= 2)
		return;
	msg.erase(msg.size() - 2);
	ss >> nick >> cmd >> reciever >> Msg;
	while (ss >> tmpMsg)
		Msg += " " + tmpMsg;
	if (cmd != "ANONYMSG")
		return;
	nick = extractNick(msg);
	if (Msg.at(0) == ':')
		Msg.erase(std::remove(Msg.begin(), Msg.end(), ':'), Msg.end());
	std::string finalMsg = static_cast<std::string>("PRIVMSG") + " " + reciever + " :" + Msg + "\r\n";
	if ((sendBytes = send(this->getSocketFd(), finalMsg.c_str(), finalMsg.size(), 0)) <= 0)
	{	if (sendBytes == 0)
			std::cerr << "Connection closed" << std::endl;
		else
			std::cerr << "send: " << strerror(errno) << std::endl;
}}