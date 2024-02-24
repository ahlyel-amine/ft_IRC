#ifndef INFORMER_HPP
# define INFORMER_HPP

#include "Bot.hpp"

class Informer : public Bot
{
	private:
		Informer();
		Informer( const Informer& );
		Informer& operator=( const Informer& );
		// std::map<std::string, Channel*>	_channels;
	public:
		Informer(std::string host, std::string port, std::string pass,BotType type);
		void	botReply(std::string msg);
		// Bot*	createBot(std::string host, std::string port, BotType type);
		~Informer();
};


#endif