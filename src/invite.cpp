#include "Client.hpp"
#include "Channel.hpp"

void   Channel::inviteCommand(vector<string> arg, Client* const client)
{
    string msg;
    if (!this->isOperator(client))
    {
        msg = "IRCserv.1337.ma 482 " + client->getNckName() + " " + this->getName() + " :You're not channel operator\r\n";
        return ;
    }
    Client *clnt = &Server::getInstance()->getClient(arg[1]);
    if (this->isMember(clnt))
    {
        msg = "IRCserv.1337.ma 443 " + client->getNckName() + " " + arg[1] + " " + this->getName() + " :is already a member of " + this->getName() + "\r\n";
        return ;
    }
    if (this->isInvited(clnt))
    {
        msg = "IRCserv.1337.ma 443 " + client->getNckName() + " " + arg[1] + " " + this->getName() + " :is already invited to " + this->getName() + "\r\n";
        return ;
    }
    this->invite(clnt);
    msg = ":" + client->getNckName() + "!~" + client->getRlName()
                + "@" + client->getHstName() + " INVITE " + clnt->getNckName() +" "+this->getName()+"\r\n";
    send(clnt->getClntFd(), msg.c_str(), msg.size(), 0);
}

bool Client::inviteCommand(vector<string> cmd)
{
    string msg;
    if (cmd.size() < 3)
    {
        msg = "IRCserv.1337.ma 461 " + this->NckName + " INVITE :Not enough parameters\r\n";
        return (false);
    }
    Server *srv = Server::getInstance();
    if (!srv->isClient(cmd[1]))
    {
        msg = "IRCserv.1337.ma 401 " + this->NckName + " " + cmd[1] + " :No such nick/channel\r\n";
        return (false);
    }
    if (!srv->isChannel(cmd[2]))
    {
        msg = "IRCserv.1337.ma 403 " + this->NckName + " " + cmd[2] + " :No such channel\r\n";
        return (false);
    }
    Channel *chnl = srv->getChannels()[cmd[2]];
    chnl->inviteCommand(cmd, this);
    return true;
}
