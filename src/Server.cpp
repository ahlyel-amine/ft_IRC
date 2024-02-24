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

/*
    This member function open a socket
    for the server and bind it otherwise
    if a problem accured and error is
    printed indicating the what causing
    the problem  
*/
void Server::SetSockFd(string &port) {
    struct addrinfo *ptr, *tmp, hnt;
    memset(&hnt, 0, sizeof(hnt));
    // char    str[256];
    // if (gethostname(str, sizeof(str)) < 0) {
    //     cerr << "Getting Host Name : " << strerror(errno) << endl;
    //     exit(1);
    // }
    hnt.ai_family = AF_INET;
    hnt.ai_protocol = IPPROTO_TCP;
    hnt.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo("0.0.0.0", port.c_str(), &hnt, &ptr), opt_val = 1;
    // int status = getaddrinfo(str, port.c_str(), &hnt, &ptr), opt_val = 1;
    if (status) {
        cerr << "Getting Address Info : " << gai_strerror(status) << endl;
        exit(1);
    }
    for (tmp = ptr; tmp; tmp = tmp->ai_next) {
        this->SockFd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
        if (this->SockFd < 0)
            continue;
        setsockopt(this->SockFd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
        if (bind(this->SockFd, tmp->ai_addr, tmp->ai_addrlen))
        {
            close(this->SockFd);
            continue;
        }
        break ;
    }
    freeaddrinfo(ptr);
    if (!tmp) {
        cerr << "Binding The Socket : " << strerror(errno) << endl;
        close(this->SockFd);
        exit(1);
    }
}

/*
    Making an Instance of the Server
    and setting up and make it ready
    to lunch if it does not already 
    exists and if an error happend
    an error is getting printed discribing
    what happend
*/
Server *Server::InstanceServer(string &port, string &psw) {
    if (!Instance) {
        Instance = new Server();
        if (port.find_first_not_of("0123456789") != string::npos){
            cerr << "Invalid Argument: " << (!psw.empty() ? "The port must contain only numbers." : "Empty Password.") << endl;
            exit(1);
        }
        Instance->SetSockFd(port);
        fcntl(Instance->SockFd, F_SETFD, O_NONBLOCK);
        Instance->Pswd = psw;
        if (listen(Instance->SockFd, max_connection)) {
            cerr << "Listening : " << strerror(errno) << endl;
            close(Instance->SockFd);
            exit(1);
        }
        Instance->ClFds.push_back(pollfd());
        Instance->ClFds.back().fd = Instance->SockFd;
        Instance->ClFds.back().events = POLLIN;
		Instance->LocalTime = localTime(time(0));
    }
    return Instance;
}

// Welcoming Function
string Server::Welcome() {
    string Wlcm;
    Wlcm += "\t██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗\n";
    Wlcm += "\t██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝\n";
    Wlcm += "\t██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗  \n";
    Wlcm += "\t██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝  \n";
    Wlcm += "\t╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗\n";
    Wlcm += "\t ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝\n";
    Wlcm += "\t\tMade By : 0xJ4GU4R | 0x54B4 | 0xM0RPH5\n";
    return Wlcm;
}

/*
    Checking if any of the socket
    file discriptors are ready to
    for to read from as behaving
    according to which file
*/
void Server::launchServer() {
    int count = poll(&this->ClFds[0], this->ClFds.size(), 0);
    if (count < 0)
        cerr << "Poll() function : " << strerror(errno) << endl;
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
/*
    Taking the incomming message form
    the client and and behaving acordding
    to what does he sent otherwise an error
    discribing the problem is printed
*/
// bool Server::ReplyToClient(Client &Clnt) {
//     char    Buff[3000];
//     memset(Buff, 0, 3000);
//     int val = recv(Clnt.getClntFd(), Buff, 3000, 0);
//     if (val > 0 && strlen(Buff)) {
//         string Msg(Buff);
//         Clnt.getMsg() += Msg;
//         if (Clnt.getMsg().find('\n') == string::npos)
//             return true;
//         // Clnt.getMsg().erase(Clnt.getMsg().size() - 2);
//         Clnt.getMsg().erase(0, Clnt.getMsg().find_first_not_of(" \t\n\v\f\r"));
//         cout << "ClinetRequest from[" << Clnt.getHstName() << "]: " << Clnt.getMsg() << endl;
//         return (Clnt.getMsg().empty()) ? true : Clnt.ParsAndExec();
//     }
//     cerr << "Reading Client[" << Clnt.getHstName() << "] Message : " << (val ? strerror(errno) : "Connection Closed.") << endl;
//     this->RemoveClient(Clnt.getClntFd());
//     return false;
// }

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
