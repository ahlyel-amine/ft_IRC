#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <sys/types.h>
#include <climits>
#include <csignal>
#include <poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <sstream>
#include <cstdio> 
#include "Server.hpp"
// #include <curl/curl.h>

using namespace std;

// class Server;

class Channel;

class Client {
	private:
		string  	                           SrvPss;     // User Name
		string  	                           UsrName;    // User Name
		string  	                           NckName;    // Nick Name
		string  	                           RlName;        // Real Name
		string  	                           HstName;       // Host Name or IP address of The ClientMachine
        string  	                           Msg;           // Client Messag
        string  	                           bff;           // Client Messag
		int     	                           ClntFd;        // The file discriptor of the Client Socket
        bool    	                           Regestred;     // True if Already registred to the server and False if not 
		vector<string>                         Cmd;		      // Command Vector with the first element as the commad and the reset as the argument
		map<string, bool (Client::*)(vector<string>)> DoCmd;  // The Command that the client is allow to do
		vector<string>						   Chnls;	      // The Channels that the client is part of
	public:
        /*      [CONSTRUCTERS]      */
        Client() {}
        Client(int ClntFd, in_addr *ClntAddr);
        /****************************/
        /*         [GETTERS]        */
        bool           alreadyIn(void) const {return this->Regestred;}
		int            getClntFd(void) const {return this->ClntFd;}
		const string   &getUsrName(void) const {return this->UsrName;}
		const string   &getNckName(void) const {return this->NckName;}
		string         &getMsg(void) {return this->Msg;}
		string         &getBff(void) {return this->bff;}
		const string   &getHstName(void) const {return this->HstName;}
		const string   &getRlName(void) const {return this->RlName;}
		vector<string> &getChnls(void) {return this->Chnls;}
        /****************************/
        /*         [SETTERS]        */
		void		   setCmd(string);
		void		   setMsgDzeb(string rep) {this->Msg = rep;}

        /****************************/
        /*        [OPERATORS]       */
        bool operator==(const Client &obj) const {return this->ClntFd == obj.ClntFd;}
		/****************************/
        /*      [ClientActions]     */
		bool		   ParsAndExec();
		bool		   joinCommand(vector<string>);
		bool		   modeCommand(vector<string>);
		bool		   inviteCommand(vector<string>);
		bool		   SendPrvMsg(vector<string>);
		bool		   Info(vector<string>);
		bool		   setNckName(vector<string>);
		bool		   setUsrName(vector<string>);
		bool		   setSrvPss(vector<string>);
		bool		   QuitServer(vector<string>);
		bool		   btcPrice(vector<string>);
		bool		   anonyMsg(vector<string>);
		bool		   Kick(vector<string>);
		bool		   pong(vector<string>);
		bool		   Topic(vector<string>);
		bool		   getDate(vector<string>);
		bool		   Pong(vector<string> vc) {
			// (void)vc;
			cout << vc[0] << endl;
			return true;
		}
		/****************************/
        /*        [Client Action Utils]       */
		void	sendClientMsg(string&, vector<string>&);
		void	sendChannelMsg(string&, vector<string>&);
		bool	setNewTopic(vector<string>& cmd);
		bool	listChannelTopic(vector<string>& cmd);
		bool	noEnParam(vector<string>& cmd);
		/****************************/
		/*      [DebugActions]     */
		bool		   infoChannel(vector<string>);
};

void    ErrorMsgGenrator(string const &Prefix, string const &Sufix, Client &Sender);
void	SendMsg(Client &Sender, Client &Reciver, string const &Cmd, string const &Msg, string const &Trg);
void    SendMsg(Client &Sender, Channel &Reciver, string const &Cmd, string const &Msg, string const &Trg);
void    SendMsg1(Client &Sender, Channel &Reciver, string const &Cmd, string const &Msg, string const &Trg);
vector<string> getTargets(string, char);
typedef bool (Client::*topControl[])(vector<string>& cmd);
