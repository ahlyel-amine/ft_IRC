#include "Client.hpp"
#include "Channel.hpp"
#include <queue>

static void modeFlagsPicker(char flag, queue<pair <bool, char> > &modesQueue, Client&  client, bool sign)
{
    switch (flag)
    {
        case 'i':
            modesQueue.push(make_pair(sign, 'i'));
            break ;
        case 't':
            modesQueue.push(make_pair(sign, 't'));
            break ;
        case 'k':
            modesQueue.push(make_pair(sign, 'k'));
            break ;
        case 'o':
            modesQueue.push(make_pair(sign, 'o'));
            break ;
        case 'l':
            modesQueue.push(make_pair(sign, 'l'));
            break ;
        case 's':
            break ;
        case 'n':
            break ;
        default:
            ErrorMsgGenrator(":IRCserv.1337.ma 501 ", " :Unknown MODE flag", client);
            break ;
    }
}

static void getOperations(vector<string> arg, queue<pair <bool, char> > &modesQueue, Client&  client)
{
    if (arg[0].empty())
    {
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " ERR_NEEDMOREPARAMS :Not enough parameters", client);
        return ;
    }
    for (size_t i = 0; i < arg[0].size(); i++)
    {
        size_t j = i + 1;
        switch (arg[0][i])
        {
            case '+':

                if (!arg[0][i + 1])
                    ErrorMsgGenrator(":IRCserv.1337.ma 501 ", " :Unknown MODE flag", client);

                for (j = i + 1; j < arg[0].size() && arg[0][j] != '-' && arg[0][j] != '+'; j++)
                    modeFlagsPicker(arg[0][j], modesQueue, client, true);

                i = j;
                break ;

            case '-':

                if (!arg[0][i + 1])
                    ErrorMsgGenrator(":IRCserv.1337.ma 501 ", " :Unknown MODE flag", client);

                for (j = i + 1; j < arg[0].size() && arg[0][j] != '-' && arg[0][j] != '+'; j++)
                    modeFlagsPicker(arg[0][j], modesQueue, client, false);

                i = j;
                break ;

            default:

                for (; i < arg[0].size() && arg[0][i] != '-' && arg[0][i] != '+'; i++)
                    modeFlagsPicker(arg[0][i], modesQueue, client, true);

                break ;
        }
    }
}

static void inviteOnlyController(Channel* const channel, Client* const client, bool set)
{
    if (set)
    {
        if (!channel->isInviteOnly())
        {
            channel->setIviteOnly();
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" +i");
        }
    }

    else
    {
        if (channel->isInviteOnly())
        {
            channel->unsetInviteOnly();
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" -i");
        }
    }
}

static void topicController(Channel* const channel, Client* const client, bool set)
{
    if (set)
    {
        if (!channel->isTopic())
        {
            channel->setTopicBool();
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" +t");
        }
    }
    else
    {
        if (channel->isTopic())
        {
            channel->unsetTopic();
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" -t");
        }
    }
}

static void keyController(Channel* const channel, Client* const client, bool set, size_t &index, vector<string> arg)
{
    if (set)
    {
        if (!channel->isLocked())
        {
            channel->setPassword(string(arg[index]));
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" +k "+channel->getPassword());
        }
    }
    else
    {
        if (channel->isLocked())
        {
            if (channel->getPassword() == arg[index])
            {
                channel->unsetPassword();
                SendMsg(*client, *channel, "MODE", "", channel->getName()+" -k "+channel->getPassword());
            }
            else
            {
                ErrorMsgGenrator(":IRCserv.1337.ma 475 ", " ERR_BADCHANNELKEY :Wrong channel key", *client);
            }
        }
    }
}

static void    operatorController(Channel* const channel, Client* const client, bool set, size_t &index, vector<string> arg)
{
    Server *server = Server::getInstance();

    if (!server->isClient(arg[index]))
        ErrorMsgGenrator(":IRCserv.1337.ma 401 ", " ERR_NOSUCHNICK :No such nick", *client);

    Client &target = server->getClient(arg[index]);

    if (set)
    {
        if (!channel->isMember(&target))
        {
            ErrorMsgGenrator(":IRCserv.1337.ma 442 ", " ERR_NOTONCHANNEL :You're not on that channel", *client);
        }
        else
        {
            if (!channel->isOperator(&target))
            {
                channel->setOperator(&target);
                SendMsg(*client, *channel, "MODE", "", channel->getName()+" +o "+target.getNckName());
            }
        }
    }
    else
    {
        if (channel->isOperator(&target))
        {
            channel->removeOperator(&target);
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" -o "+target.getNckName());
        }
    }
}

static void    limitController(Channel* const channel, Client* const client, bool set, size_t &index, vector<string> arg)
{
    if (set)
    {
        if (!channel->isLimited())
        {
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" +l "+arg[index]);
            channel->setLimit(atoll(arg[index].c_str()));
        }
    }

    else
    {
        if (channel->isLimited())
        {
            channel->unsetLimit();
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" -l "+arg[index]);
        }
    }
}

static void printModes(Privelege priv, string &chnl, Client &client)
{
    string modes;

    if (priv.invitOnly |priv.topic | priv.password | priv.limit)
        modes += "+";
    if (priv.invitOnly)
        modes += "i";
    if (priv.topic)
        modes += "t";
    if (priv.password)
        modes += "k";
    if (priv.limit)
        modes += "l";
    ErrorMsgGenrator(":IRCserv.1337.ma 324 ", " " + chnl + " " + modes, client);
}

static void    modeSelector(queue<pair <bool, char> > &modesQueue, Channel* const channel, Client* const client, bool set, size_t &index, vector<string> arg)
{
    switch (modesQueue.front().second)
    {
        case 'i':
            inviteOnlyController(channel, client, set);
            break;
        case 't':
            topicController(channel, client, set);
            break ;
        case 'k':
            if (++index >= arg.size())
                ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " ERR_NEEDMOREPARAMS :Not enough parameters", *client);
            else
                keyController(channel, client, set, index, arg);
            break ;
        case 'o':
            if (++index >= arg.size())
                ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " ERR_NEEDMOREPARAMS :Not enough parameters", *client);
            else
                operatorController(channel, client, set, index, arg);
            break ;
        case 'l':
            if (set && ++index >= arg.size())
                ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " ERR_NEEDMOREPARAMS :Not enough parameters", *client);
            else
                limitController(channel, client, set, index, arg);
            break ;
    }
}

bool Client::modeCommand(vector<string> arg)
{
    arg.erase(arg.begin());
    if (arg.size() < 1 || arg[0].empty())
    {
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", "MODE :Not enough parameters", *this);
        return (false);
    }

    map<string, Channel*>   &Channels =  Server::getInstance()->getChannels();
    if (!Server::getInstance()->isChannel(arg[0]))
    {
        ErrorMsgGenrator(":IRCserv.1337.ma 403 ", " " + arg[0] + " :No such channel", *this);
        return (false);
    }
    string chanl(arg[0]);
    arg.erase(arg.begin());
    Channels[chanl]->modeCommand(arg, this);
    return (true);
}

void   Channel::modeCommand(vector<string> arg, Client* const client)
{
    queue<pair <bool, char> >   modesQueue;

    if (!arg.size())
        return (printModes(privelege, this->name, *client));

    if (!this->isOperator(client))
        return (ErrorMsgGenrator(":IRCserv.1337.ma 482 ", " " + this->getName() + " :You're not channel operator", *client));

    getOperations(arg, modesQueue, *client);

    size_t index = 0;

    while (!modesQueue.empty())
    {
        if (modesQueue.front().first)
        {
            modeSelector(modesQueue, this, client, true, index, arg);
        }
        else
        {
            modeSelector(modesQueue, this, client, false, index, arg);
        }
        modesQueue.pop();
    }
}
