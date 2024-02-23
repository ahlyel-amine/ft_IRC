#include "Client.hpp"
#include "Channel.hpp"

void   Channel::inviteCommand(vector<string> arg, Client* const client)
{
    if (!this->isOperator(client))
    {
        ErrorMsgGenrator("IRCserv.1337.ma 482 ", " " + arg[2] + " :You're not channel operator", *client);
        return ;
    }
    Client *clnt = &Server::getInstance()->getClient(arg[1]);
    if (this->isMember(clnt))
    {
        ErrorMsgGenrator(":IRCserv.1337.ma 443 ", " " + arg[1] + " :Is already on channel", *client);
        return ;
    }
    // if (this->isInvited(clnt))
    // {
    //     ErrorMsgGenrator(":IRCserv.1337.ma 443 ", " " + arg[1] + " :Already on that channel", *client);
    //     return ;
    // }
    if (!this->isInvited(clnt))
        this->invite(clnt);
    SendMsg(*client, *clnt, arg[0], "", arg[1] + " " + arg[2]);
    ErrorMsgGenrator(":IRCserv.1337.ma 341 ", " " + arg[1] + " " + arg[2], *client);
}

bool Client::inviteCommand(vector<string> cmd)
{
    string msg;
    if (cmd.size() < 3)
    {

        ErrorMsgGenrator("IRCserv.1337.ma 461 ", " " + cmd[0] + " :Not enough parameters", *this);
        return (false);
    }
    Server *srv = Server::getInstance();
    if (!srv->isClient(cmd[1]))
    {
        ErrorMsgGenrator("IRCserv.1337.ma 401 ", " " + cmd[1] + " :No such nick", *this);
        return (false);
    }
    if (!srv->isChannel(cmd[2]))
    {
        ErrorMsgGenrator("IRCserv.1337.ma 403 ", " " + cmd[1] + " :No such channel", *this);
        return (false);
    }
    Channel *chnl = srv->getChannels()[cmd[2]];
    chnl->inviteCommand(cmd, this);
    return true;
}
