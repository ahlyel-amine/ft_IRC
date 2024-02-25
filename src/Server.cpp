#include "Server.hpp"

Server *Server::Instance = NULL;

std::string	localTime(time_t now)
{
	std::string t = ctime(&now);
	t.erase(t.length() - 1);
	return (t);
}

bool isdigit_b(int c) {return isdigit(c);}

int Server::getClientByNckName(string &NckName) {
    map<int, Client>::const_iterator it = this->Clients.begin();
    for (; it != this->Clients.end(); it++)
        if (it->second.getNckName() == NckName)
            return it->second.getClntFd();
    return -1;
}

Server::~Server() {
    for (map<string, Channel*>::iterator it = this->Channels.begin(); it != this->Channels.end(); it++)
        delete it->second;
    this->Channels.clear();
    for (map<int, Client>::iterator it = this->Clients.begin(); it != this->Clients.end(); it++)
        close(it->first);
    this->Clients.clear();
    this->ClFds.clear();
    close(this->SockFd);
}

/*
    This member function open a socket
    for the server and bind it otherwise
    if a problem accured and error is
    printed indicating the what causing
    the problem  
*/
void Server::SetSockFd(string &port) {
    struct addrinfo *ptr, *tmp, hnt;
    char    str[256];
    
    if (gethostname(str, sizeof(str)) < 0) {
        cerr << "Getting Host Name : " << strerror(errno) << endl;
        exit(1);
    }
    memset(&hnt, 0, sizeof(hnt));
    hnt.ai_family = AF_INET;
    hnt.ai_protocol = IPPROTO_TCP;
    hnt.ai_socktype = SOCK_STREAM;
    // int status = getaddrinfo("0.0.0.0", port.c_str(), &hnt, &ptr), opt_val = 1;
    int status = getaddrinfo(str, port.c_str(), &hnt, &ptr), enable = 1;
    if (status) {
        cerr << "Getting Address Info : " << gai_strerror(status) << endl;
        exit(1);
    }
    for (tmp = ptr; tmp; tmp = tmp->ai_next) {
        this->SockFd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
        if (this->SockFd < 0)
            continue;
        if (setsockopt(this->SockFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof enable) == -1)
		{
		    freeaddrinfo(ptr);
			close(this->SockFd);
			throw std::runtime_error(strerror(errno));
		}
        if (bind(this->SockFd, tmp->ai_addr, tmp->ai_addrlen) == -1)
        {
            close(this->SockFd);
            continue;
        }
        break ;
    }
    freeaddrinfo(ptr);
    if (!tmp) {
        close(this->SockFd);
        throw std::runtime_error("Error: Could not Bind the socket");
    }
	this->isConnected = true;
}

/*
    Making an Instance of the Server
    and setting up and make it ready
    to lunch if it does not already 
    exists and if an error happend
    an error is getting printed discribing
    what happend
*/
void	Server::SocketListen( void )
{
	if (!this->isConnected){
		Instance->SetSockFd(this->Lport);
		fcntl(Instance->SockFd, F_SETFD, O_NONBLOCK);			
		if (listen(Instance->SockFd, max_connection)) {
			close(Instance->SockFd);
			throw std::runtime_error(strerror(errno));
		}
		Instance->ClFds.push_back(pollfd());
		Instance->ClFds.back().fd = Instance->SockFd;
		Instance->ClFds.back().events = POLLIN;
	}
	return ;
}
Server *Server::InstanceServer(string &port, string &psw) {
    if (!Instance) {
        Instance = new Server();
        if (port.find_first_not_of("0123456789") != string::npos || atoi(port.c_str()) < 1024 || psw.empty()){
            cerr << "Invalid Argument: " << (!psw.empty() ? "The port must contain only numbers." : "Empty Password.") << endl;
            exit(1);
        }
        for (size_t i = 0; i < psw.size(); i++) {
            if (!isalnum(psw[i])) {
                cerr << "Error : Invalide PassWord policy" << endl;
                exit(1);
            }
        }
        Instance->Lport = port;
		Instance->Pswd = psw;
		Instance->isConnected = false;
		Instance->LocalTime = localTime(time(0));
    }
    return Instance;
}

// Welcoming Function
string Server::Welcome() {
    string Wlcm;
    Wlcm += GRN;
    Wlcm += "\t██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗\n";
    Wlcm += "\t██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝\n";
    Wlcm += "\t██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗  \n";
    Wlcm += "\t██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝  \n";
    Wlcm += "\t╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗\n";
    Wlcm += "\t ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝\n";
    Wlcm += "\t\t[Kamikazesābā IRC]\n\n";
    Wlcm += "\tMade By :" + static_cast<string>(C_CLS) + " 0xJ4GU4R "
        + static_cast<string>(GRN) + "|" + static_cast<string>(C_CLS)
        + " 0xSABA " + static_cast<string>(GRN) + "|" + static_cast<string>(C_CLS)
        + " 0xM0RPH5\n";
    return Wlcm;
}

/*
    Checking if any of the socket
    file discriptors are ready to
    for to read from as behaving
    according to which file
*/
void Server::launchServer() {
    int count;
	this->SocketListen();
	count = poll(&this->ClFds[0], this->ClFds.size(), 0);
    if (count < 0) {
		close(this->SockFd);
        throw std::runtime_error(strerror(errno));
    }
    for (size_t i = 0; i < this->ClFds.size(); i++)
        if (this->ClFds[i].revents & POLLIN)
            (this->SockFd == this->ClFds[i].fd) ? this->JoinServer() : this->ReplyToClient(this->Clients[this->ClFds[i].fd]); //? this->JoinServer() : this->;  // ? new client : client request;
}

/*
    If an incomming connection tring
    to connect to the server it get
    identified and registred if no
    error accured if it does an error
    discribing the problem happend is
    gettign pritned
*/
bool Server::JoinServer() {
    sockaddr_in ClntAddr;
    socklen_t len = sizeof(sockaddr_in);
    memset(&ClntAddr, 0, len);
    int ClntFd = accept(this->SockFd, (sockaddr *)&ClntAddr, &len);
    if (ClntFd < 0) {
        cerr << "Establishing Connection : " << strerror(errno) << endl;
        return false;
    }
    fcntl(ClntFd, F_SETFD, O_NONBLOCK);
    this->Clients.insert(pair<int, Client>(ClntFd, Client(ClntFd, &ClntAddr.sin_addr)));
    this->ClFds.push_back(pollfd());
    this->ClFds.back().fd = ClntFd;
    this->ClFds.back().events = POLLIN;
    return true;
}

/// @brief shank the recieve buffer into lines and feed them to the client as separeted messages cmd
/// @param Clnt Client Instance
/// @param buffer to be shanked
/// @return status of the parsing
bool	BufferFeed(Client &Clnt, string &buffer)
{
	bool			parsed;
	stringstream	feed(buffer);	
	string			tmp;

	while (getline(feed, tmp))
	{
		if (feed.eof() && tmp.empty())
			break;
		if (tmp.size() && tmp[tmp.size() - 1] == '\r')
			tmp.erase(tmp.size() - 1);
		Clnt.setMsgDzeb(tmp);
        cout << tmp << endl;
		if (!Clnt.getMsg().empty())
            parsed = Clnt.ParsAndExec();
        if (Clnt.getCmd() == "QUIT") {
            Server::getInstance()->RemoveClient(Clnt.getClntFd());
            return false;
        }
		tmp.clear();
	}
    buffer = "";
	return parsed;
}

/*
    Taking the incomming message form
    the client and and behaving acordding
    to what does he sent otherwise an error
    discribing the problem is printed
*/
bool Server::ReplyToClient(Client &Clnt) {
    char    Buff[3000];
    memset(Buff, 0, 3000);
    int val = recv(Clnt.getClntFd(), Buff, 3000, 0);
    if (val > 0 && strlen(Buff)) {
        string Msg(Buff);
        Clnt.getBff() += Msg;
	    if (Clnt.getBff().find('\n') == string::npos)
            return true;
        Msg.erase(0, Msg.find_first_not_of(" \t\n\v\f\r"));
    	return BufferFeed(Clnt, Clnt.getBff());
	}
    cerr << "Reading Client[" << Clnt.getHstName() << "] Message : " << (val ? strerror(errno) : "Connection Closed.") << endl;
    vector<string> vc;
    vc.push_back("QUIT");
    Clnt.QuitServer(vc);
    this->RemoveClient(Clnt.getClntFd());
    return false;
}

void    Server::RemoveClient(int fd) {
    for (size_t i = 0; i < this->ClFds.size(); i++) {
        if (this->ClFds[i].fd == fd) {
            this->ClFds.erase(this->ClFds.begin() + i);
            break ;
        }
    }
    if (!this->Clients[fd].getChnls().empty()) {
        for (size_t i = 0; i < this->Clients[fd].getChnls().size(); i++) {
            this->Channels[this->Clients[fd].getChnls()[i]]->removeInvited(&this->Clients[fd]);
            this->Channels[this->Clients[fd].getChnls()[i]]->removeOperator(&this->Clients[fd]);
            this->Channels[this->Clients[fd].getChnls()[i]]->removeMember(&this->Clients[fd]);
            if (this->Channels[this->Clients[fd].getChnls()[i]]->getMembers().empty()) {
                delete this->Channels[this->Clients[fd].getChnls()[i]];
                this->Channels.erase(this->Clients[fd].getChnls()[i]);
            }
        }
    }
    this->Clients.erase(fd);
    close(fd);
}


Client &               Server::getClient(string &NckName)
{
    map<int, Client>::iterator it = Server::getInstance()->getClients().begin();
    map<int, Client>::iterator itend = Server::getInstance()->getClients().end();
    for (; it != itend; it++)
        if (it->second.getNckName() == NckName)
            return it->second;
    return it->second;
}

Channel *              Server::getChannel(string &NckName)
{
    return this->Channels[NckName];
}

bool                Server::isClient(string &NckName) {
    for (map<int, Client>::iterator it = this->Clients.begin(); it != this->Clients.end(); it++)
        if (it->second.getNckName() == NckName)
            return true;
    return false;
}

bool                Server::isClient(int ClntFd)
{
    return this->Clients.find(ClntFd) != this->Clients.end();
}

bool                Server::isChannel(string &chnl)
{
    return this->Channels.find(chnl) != this->Channels.end();
}