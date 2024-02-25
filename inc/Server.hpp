#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>
#include <climits>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include "Client.hpp"
#include "Channel.hpp"
#include <fcntl.h>

using namespace std;

class Channel;

class Client;

#define SERVER_NAME "IRCserv.1337.ma"
#define VERSION "1.0"

#define max_connection 128

# define BLU "\033[34;1m"
# define CYN "\033[36;1m"
# define PRP "\033[35;1m"
# define GRN "\033[32;1m"
# define RED "\033[31;1m"
# define YLW "\033[33;1m"
# define C_CLS "\033[0m"

class Server {
    private:
        int						SockFd;		// File discriptor of the server socket
		bool					isConnected;
        static Server			*Instance;	// This pointer will make the class have only one inctance
        string					Pswd;		// This string represent the password the client shold provide to log to the server
		string					Lport;		// The Server Listenning port
		string					LocalTime;	// The local time of creating the server
        vector<pollfd>			ClFds;		// This vector will hold an array of the struct used to send to poll() function
        map<int, Client>		Clients;	// A map of Clients of which the key is the client SocketFd and the value is the Client
        map<string, Channel*>	Channels;	// A map of Clients of which the key is the client SocketFd and the value is the Client

        /*[Constructers and operatores overload]*/
        Server(const Server &obj) {*this=obj;}
        Server &operator=(const Server &obj) {(void)obj;return *this;}
        /****************************************/
    public:
        Server() {}
        ~Server();
        /*    [Server Actions]   */
        void                    launchServer();
        bool                    JoinServer();
        bool                    ReplyToClient(Client &Clnt);
        void                    SetSockFd(string &port);
        void                    RemoveClient(int);
        /*************************/
        Client                  &getClient(int ClntFd) {return this->Clients[ClntFd];}


        Client                  &getClient(string &NckName);
        Channel                 *getChannel(string &NckName);

        bool                    isClient(string &NckName);
        bool                    isClient(int ClntFd);
        bool                    isChannel(string &chnl);

        /*       [GETTERS]       */
        string                  getLocalTime(){
            return (this->LocalTime);
        }
        static Server           *InstanceServer(string &port, string &Pswd);
        int                     getSockFd() const {return this->SockFd;}
        string                  getPswd() const {return this->Pswd;}
        map<int, Client>        &getClients() {return this->Clients;}
        map<int, Client>        getClientst() {return this->Clients;}
        map<string, Channel*>   &getChannels() {return this->Channels;}
        static Server           *getInstance() {return Instance;}
        int                     getClientByNckName(string &NckName);
        /*************************/
        static string           Welcome();
		static void			    RegistMsgReply( const Client& );
		void				    BroadCastMsg( const Client& reciever, const stringstream& msg ) const;
		void					SocketListen( void );
};

template <typename T>

void    VcRemove(vector<T> &, T);

template <typename T>

bool    VcFind(vector<T> const &vc, T trg) {return find(vc.begin(), vc.end(), trg) != vc.end();}

template <typename T>

void VcRemove(vector<T> &vc, T trg) {
    size_t i = 0;
    for (;i < vc.size(); i++)
        if (vc[i] == trg)
            break ;
    if (i != vc.size())
        vc.erase(vc.begin() + i);
}

string localTime(time_t);
