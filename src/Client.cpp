#include"Client.hpp"

/**
 * @brief Constructor for the Client class.
 * 
 * @param ClntFd The file descriptor of the client.
 * @param ClntAddr The address of the client.
 */

Client::Client(int ClntFd, in_addr *ClntAddr) : ClntFd(ClntFd), Regestred(false) {
    this->DoCmd["PASS"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setSrvPss);
    this->DoCmd["NICK"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setNckName);
    this->DoCmd["USER"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setUsrName);
    this->DoCmd["JOIN"] = static_cast<bool (Client::*)(vector<string>)>(&Client::joinCommand);
    this->DoCmd["PRIVMSG"] = static_cast<bool (Client::*)(vector<string>)>(&Client::SendPrvMsg);
    this->DoCmd["INFO"] = static_cast<bool (Client::*)(vector<string>)>(&Client::Info);
    this->DoCmd["QUIT"] = static_cast<bool (Client::*)(vector<string>)>(&Client::QuitServer);
	this->DoCmd["BTCPRICE"] = static_cast<bool (Client::*)(vector<string>)>(&Client::btcPrice);
	this->DoCmd["ANONYMSG"] = static_cast<bool (Client::*)(vector<string>)>(&Client::anonyMsg);
	this->DoCmd["KICK"] = static_cast<bool (Client::*)(vector<string>)>(&Client::Kick);
	this->DoCmd["TOPIC"] = static_cast<bool (Client::*)(vector<string>)>(&Client::Topic);
	this->DoCmd["MODE"] = static_cast<bool (Client::*)(vector<string>)>(&Client::modeCommand);
	this->DoCmd["INVITE"] = static_cast<bool (Client::*)(vector<string>)>(&Client::inviteCommand);
	this->DoCmd["INFOC"] = static_cast<bool (Client::*)(vector<string>)>(&Client::infoChannel);
	this->DoCmd["PONG"] = static_cast<bool (Client::*)(vector<string>)>(&Client::Pong);
    this->DoCmd["DATE"] = static_cast<bool (Client::*)(vector<string>)>(&Client::getDate);
    this->DoCmd["PART"] = static_cast<bool (Client::*)(vector<string>)>(&Client::partCommand);
	this->HstName = inet_ntoa(*ClntAddr);
}

bool Client::pong(vector<string> cmd)
{
    (void)cmd;
    return true;
}

void listClient(vector<Client *> vc, string& msg, string const &grade) {
    msg +=  grade;
    for (size_t i = 0; i < vc.size(); i++)
    {
        msg +=  vc[i]->getNckName() + " | ";
    }
    msg +=  "\n";
}

bool Client::infoChannel(vector<string> cmd)
{
    try
    {    
        if (cmd.size() != 2 || cmd[1].empty() || !Server::getInstance()->isChannel(cmd[1]))
            return false;
        Channel *chnl = Server::getInstance()->getChannel(cmd[1]);
        if (!chnl)
            return false;
        string msg;
        msg +=  "Channel: " + cmd[1] + "\n";
        listClient(chnl->getMembers(), msg, "Members: ");
        listClient(chnl->getOperators(), msg, "Operators: ");
        listClient(chnl->getInvited(), msg, "Invited: ");
        msg += "Topic: ";
        if (chnl->isTopic()) msg += "✓ : ";
        else msg += "✕ ";
        msg += chnl->getTopic() + "\n";
        msg += "Invite Only: ";
        if (chnl->isInviteOnly()) msg += "✓ : ";
        else msg += "✕ ";
        msg +=  "\nPassword: ";
        if (chnl->isLocked()) msg += "✓ : ";
        else msg += "✕ ";
        if (chnl->isLocked()) msg += chnl->getPassword();
        msg +=  "\nLimit: ";
        if (chnl->isLimited()) msg += "✓ : ";
        else msg += "✕ ";
        if (chnl->isLimited()) msg += to_string(chnl->getLimit());
        msg +=  "\n";
        send(this->ClntFd, msg.c_str(), msg.size(), 0);
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return true;
}

/**
 * @brief Sets the command for the client.
 * 
 * This function parses the input line and sets the command for the client.
 * The input line is split into individual command arguments based on spaces.
 * If a colon (:) is encountered, the remaining part of the line is considered as a single argument.
 * 
 * @param line The input line containing the command.
 */

void    Client::setCmd(string line) {
    string tmp;
    for (size_t i = 0; i < line.size(); i++){
        if (line.at(i) == ':') {
            tmp = line.substr(i+1);
            i = line.size() - 1;
        }
        else if (line.at(i) != ' ')
            tmp += line.at(i);
        if (line.at(i) == ' ' || i == line.size() - 1) {
            for (; i+1 < line.size() && line.at(i+1) == ' '; i++);
            this->Cmd.push_back(tmp);
            tmp = "";
        }
    }
}

void Client::Welcome() {
	const string&	nickName = this->getNckName();
    string msg;
    msg = ":" + static_cast<string>(SERVER_NAME) + " 001 " + nickName
        + " :Welcome to Kamikazi network, " + nickName + "\r\n";
    msg += ":" + static_cast<string>(SERVER_NAME) + " 002 " + nickName
        + " :Your host is [" + static_cast<string>(SERVER_NAME) + "], running version "
        + static_cast<string>(VERSION) + "\r\n";
    msg += ":" + static_cast<string>(SERVER_NAME) + " 003 " + nickName
        + " :This Server[" + static_cast<string>(SERVER_NAME) + "] was created on "
        + Server::getInstance()->getLocalTime() + "\r\n";
    if (send(this->ClntFd, msg.c_str(), msg.size(), 0) <= 0)
		cerr << "Error: " << strerror(errno) << endl;
}

/**
 * @brief Parses and executes the client command.
 * 
 * This function parses the command received from the client and executes it accordingly.
 * It checks if the client is registered and sends a welcome message if not.
 * It then checks if the command is valid and calls the corresponding function to execute it.
 * If the command is not recognized, it sends an error message to the client.
 * 
 * @return true if the command was executed successfully, false otherwise.
 */

bool    Client::ParsAndExec() {
    bool rt;

    this->Cmd.clear();
    this->setCmd(this->Msg);
    for (size_t i = 0; i < this->Cmd[0].size(); i++)
        if (isalpha(this->Cmd[0][i]) && islower(this->Cmd[0][i]))
            this->Cmd[0][i] = toupper(this->Cmd[0][i]);
    if (!this->Regestred && (this->Cmd[0] != "PASS" && this->Cmd[0] != "NICK" && this->Cmd[0] != "USER" && this->Cmd[0] != "QUIT")) {
        ErrorMsgGenrator(":IRCserv.1337.ma 451 ", " " + this->Cmd[0] + " :You have not registered", *this);
        return false;
    }
    if (this->DoCmd.find(this->Cmd[0]) != this->DoCmd.end())
        rt = (this->*DoCmd[this->Cmd[0]])(this->Cmd);
    else {
        ErrorMsgGenrator(":IRCserv.1337.ma 421 ", " " + this->Cmd[0] + " :Unknown command", *this);
        rt = false;
    }
    if (!this->SrvPss.empty() && !this->NckName.empty() && !this->UsrName.empty()) {
        if (!this->Regestred)
        {
			this->Welcome();
			cout << BLU << "[ INFO ]\t" << WHT << this->getHstName() << " Authenticat Successfuly to "
			<< YLW << SERVER_NAME << C_CLS << " " << WHT << localTime(time(0)) << endl; 
		}    
        this->Regestred = true;
    }
    this->Msg = "";
    return rt;
}


// ERRORREPLYFORMAT [":<ServerName> <StatusCode> <CLientNick> :<Msg>"]

// MSGRPLFORMAT [":<NCIKNAME>!~<REALNAME>@<HOSTNAME> <CMD> <TARGET> :<MSG>"]

bool Client::Info(vector<string> cmd) {
    (void)cmd;
    string msg =  "Channels:[";
    for (size_t i = 0; i < this->Chnls.size(); i++)
        msg += this->Chnls[i] + " | ";
    msg += "]\n";
    for (size_t i = 0; i < this->Chnls.size(); i++) {
        msg += this->Chnls[i] + " Admine:[";
        map<string, Channel*> chnl = Server::getInstance()->getChannels();
        if (VcFind(chnl[this->Chnls[i]]->getOperators(), this))
            msg += "✓]\n";
        else
            msg +="✕]\n";
        msg += "ChanelMembers:[";
        vector<Client*> channelMembers = chnl[this->Chnls[i]]->getMembers();
        for (size_t j = 0; j < channelMembers.size(); j++)
            msg += channelMembers[j]->getNckName() + ((j+1 == channelMembers.size()) ? "]\n" : " | ");
    }
    send(this->ClntFd, msg.c_str(), msg.size(), 0);
    return true;
}

bool    Client::QuitServer(vector<string> cmd) {
    vector<Client> Friends;

    if (cmd.size() == 1)
        cmd.push_back("");
    for (size_t i = 0; i < this->Chnls.size(); i++) {
        Channel *chnl = Server::getInstance()->getChannels()[this->Chnls[i]];
        SendMsg(*this, *chnl, cmd[0], cmd[cmd.size()-1], "Quit");
        if (chnl->getOperators().size() == 1 && chnl->isOperator(this) && chnl->getMembers().size() > 1) {
            chnl->addOperator(chnl->getMembers()[chnl->getMembers()[0] == this]);
            SendMsg(*this, *chnl, "MODE", "", this->Chnls[i] + " +o " + chnl->getMembers()[chnl->getMembers()[0] == this]->NckName);
        }
    }
    return true;
}


/**
 * @brief Generates an error message with the given prefix and suffix, and sends it to the client.
 * 
 * @param Prefix The prefix of the error message.
 * @param Sufix The suffix of the error message.
 * @param Sender The client who will receive the error message.
 */

void    ErrorMsgGenrator(string const &Prefix, string const &Sufix, Client &Sender) {
    string msg = Prefix + ((Sender.getNckName().empty()) ? "*" : Sender.getNckName())
                + Sufix + "\r\n";

    send(Sender.getClntFd(), msg.c_str(), msg.size(), 0);
}


/**
 * @brief Sends a message from one client to another.
 * 
 * @param Sender The client sending the message.
 * @param Reciver The client receiving the message.
 * @param Cmd The command associated with the message.
 * @param Msg The content of the message.
 * @param Trg The target of the message.
 */

void    SendMsg(Client &Sender, Client &Reciver, string const &Cmd, string const &Msg, string const &Trg) {
    string msg = ":" + Sender.getNckName() + "!" + Sender.getRlName()
                + "@" + Sender.getHstName() + " " + Cmd + " " + Trg
                + ((Msg.empty()) ? "\r\n" : " :" + Msg + "\r\n");

    send(Reciver.getClntFd(), msg.c_str(), msg.size(), 0);
}


/**
 * @brief Sends a message from a client to a channel.
 * 
 * This function constructs a message string using the sender's nickname, real name, hostname,
 * command, target, and message. It then sends the message to all members of the specified channel,
 * except for the sender. If the send operation fails, the client is removed from the server.
 * 
 * @param Sender The client sending the message.
 * @param Reciver The channel receiving the message.
 * @param Cmd The command of the message.
 * @param Msg The content of the message.
 * @param Trg The target of the message.
 */

void    SendMsg(Client &Sender, Channel &Reciver, string const &Cmd, string const &Msg, string const &Trg) {
    string msg = ":" + Sender.getNckName() + "!" + Sender.getRlName()
                + "@" + Sender.getHstName() + " " + Cmd + " " + Trg
                + ((Msg.empty()) ? "\r\n" : " :" + Msg + "\r\n");

    for (size_t i = 0; i < Reciver.getMembers().size(); i++)
        send(Reciver.getMembers()[i]->getClntFd(), msg.c_str(), msg.size(), 0);
}



