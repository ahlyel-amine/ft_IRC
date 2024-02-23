#include "Client.hpp"
#include "Channel.hpp"
#include <queue>

bool Client::modeCommand(vector<string> arg)
{
    arg.erase(arg.begin());
    if (arg.size() < 1)
        return (false);
    if (arg[0][0] != '#' || arg[0].size() < 2 || arg[0].size() > 51)
        return (false);

    map<string, Channel*>   &Channels =  Server::getInstance()->getChannels();
    if (!Server::getInstance()->isChannel(arg[0]))
    {
        /*
        **  403 ERR_NOSUCHCHANNEL
        */
        return (false);
    }
    string chanl(arg[0]);
    arg.erase(arg.begin());
    Channels[chanl]->modeCommand(arg, this);
    return (true);
}

struct modes
{
    bool   invitOnly;
    bool   limit;
    size_t limitSize;
    bool   password;
    string passwordStr;
    bool   topic;
    string topicStr;
};

enum replie
{
    OPERATOR_ADDED,
    OPERATOR_REMOVED,
    INVITE_ONLY_SET,
    INVITE_ONLY_UNSET,
    TOPIC_SET,
    TOPIC_UNSET,
    PASSWORD_SET,
    PASSWORD_UNSET,
    LIMIT_SET,
    LIMIT_UNSET
};

void genericReplies(Channel* const channel, Client* const client, enum replie replie)
{
    vector<Client*>::iterator it =  channel->getMembers().begin();
    vector<Client*>::iterator itend =  channel->getMembers().end();
    string msg = ":"+client->getNckName()+"!~"+client->getUsrName()+"@"+client->getHstName()+" MODE "+channel->getName();
    switch (replie)
    {
        case OPERATOR_ADDED:
            ;// msg += " +o "+(*it)->getNckName()+" \r\n";
            break;
        case OPERATOR_REMOVED:
            ;// msg += " -o "+(*it)->getNckName()+" \r\n";
            break;
        case INVITE_ONLY_SET:
            ;// msg += " +i \r\n";
            break;
        case INVITE_ONLY_UNSET:
            ;// msg += " -i \r\n";
            break;
        case TOPIC_SET:
            ;// msg += " +t \r\n";
            break;
        case TOPIC_UNSET:
            ;// msg += " -t \r\n";
            break;
        case PASSWORD_SET:
            ;// msg += " +k \r\n";
            break;
        case PASSWORD_UNSET:
            ;// msg += " -k \r\n";
            break;
        case LIMIT_SET:
            ;// msg += " +l \r\n";
            break;
        case LIMIT_UNSET:
            ;// msg += " -l \r\n";
            break;
    }
    for (; it != itend; ++it)
    {
        send((*it)->getClntFd(), msg.c_str(), msg.size(), 0);
    }
}

void    getOperations(vector<string> arg, queue<pair <bool, char> > &modesQueue, Client&  client)
{
    for (size_t i = 0; i < arg[0].size(); i++)
    {
        size_t j = i + 1;
        switch (arg[0][i])
        {
            case '+':
                if (!arg[0][i + 1])
                {
                    ErrorMsgGenrator(":irc_server 501 ", " :Unknown MODE flag", client);
                }
                for (j = i + 1; j < arg[0].size() && arg[0][j] != '-' && arg[0][j] != '+'; j++)
                {
                    switch (arg[0][j])
                    {
                        case 'i':
                            modesQueue.push(make_pair(true, 'i'));
                            break ;
                        case 't':
                            modesQueue.push(make_pair(true, 't'));
                            break ;
                        case 'k':
                            modesQueue.push(make_pair(true, 'k'));
                            break ;
                        case 'o':
                            modesQueue.push(make_pair(true, 'o'));
                            break ;
                        case 'l':
                            modesQueue.push(make_pair(true, 'l'));
                            break ;
                        default:
                            ErrorMsgGenrator(":irc_server 501 ", " :Unknown MODE flag", client);
                            break ;
                    }
                    i = j;
                }
                break ;
            case '-':
                if (!arg[0][i + 1])
                    ErrorMsgGenrator(":irc_server 501 ", " :Unknown MODE flag", client);
                for (j = i + 1; j < arg[0].size() && arg[0][j] != '-' && arg[0][j] != '+'; j++)
                {
                    switch (arg[0][j])
                    {
                        case 'i':
                            modesQueue.push(make_pair(false, 'i'));
                            break ;
                        case 't':
                            modesQueue.push(make_pair(false, 't'));
                            break ;
                        case 'k':
                            modesQueue.push(make_pair(false, 'k'));
                            break ;
                        case 'o':
                            modesQueue.push(make_pair(false, 'o'));
                            break ;
                        case 'l':
                            modesQueue.push(make_pair(false, 'l'));
                            break ;
                        default:
                            ErrorMsgGenrator(":irc_server 501 ", " :Unknown MODE flag", client);
                            break ;
                    }
                    i = j;
                }
                break ;
            default:
                ErrorMsgGenrator(":irc_server 501 ", " :Unknown MODE flag", client);
                break ;
        }
    }
}

void inviteOnlyController(Channel* const channel, Client* const client, bool set)
{
    if (!channel->isOperator(client))
    {
        ErrorMsgGenrator(":irc_server 482 ", channel->getName() + " :You're not channel operator", *client);
        return ;
    }
    if (set)
    {
        if (!channel->isInviteOnly())
        {
            channel->setIviteOnly();
                SendMsg(*client, *channel, "MODE", "", channel->getName()+" +i");
            // SendMsg(*client, *channel, "MODE", "", "+i");
            // genericReplies(channel, client, INVITE_ONLY_SET);
        }
        else
        {
            ErrorMsgGenrator(":irc_server 473 ", channel->getName() + " :Channel is already invite only", *client);
            ;// msg += "IRC_SERVER 473 ERR_INVITEONLYCHAN :Channel is already invite only\r\n";
        }
    }
    else
    {
        if (channel->isInviteOnly())
        {
            channel->unsetInviteOnly();
                SendMsg(*client, *channel, "MODE", "", channel->getName()+" -i");
            // SendMsg(*client, *channel, "MODE", "", "-i");
            // genericReplies(channel, client, INVITE_ONLY_UNSET);
        }
        else
        {
            ;// msg += "IRC_SERVER 473 ERR_INVITEONLYCHAN :Channel is already not invite only\r\n";
        }
    }
}

void topicController(Channel* const channel, Client* const client, bool set)
{
    if (!channel->isOperator(client))
    {
        ;// msg += ":"+client->getHstName()+" 482 "+client->getNckName()+" "+channel->getName()+" :You're not channel operator\r\n";
        return ;
    }
    if (set)
    {
        if (!channel->isTopic())
        {
            channel->setTopicBool();
                SendMsg(*client, *channel, "MODE", "", channel->getName()+" +t");
            // SendMsg(*client, *channel, "MODE", "", "+t");
            // genericReplies(channel, client, TOPIC_SET);
        }
        else
        {
            ;// msg += "IRC_SERVER  442 ERR_NOTOPIC :Channel topic is already public\r\n";
        }
    }
    else
    {
        if (channel->isTopic())
        {
            channel->unsetTopic();
                SendMsg(*client, *channel, "MODE", "", channel->getName()+" -t");
            // SendMsg(*client, *channel, "MODE", "", "-t");
            // genericReplies(channel, client, TOPIC_UNSET);
        }
        else
        {
            ;// msg += "IRC_SERVER  442 ERR_NOTOPIC :Channel topic is already private\r\n";
        }
    }
}

void    keyController(Channel* const channel, Client* const client, bool set, size_t &index, vector<string> arg)
{
    if (!channel->isOperator(client))
    {
        ;// msg += ":"+client->getHstName()+" 482 "+client->getNckName()+" "+channel->getName()+" :You're not channel operator\r\n";
        return ;
    }
    if (++index >= arg.size())
    {
        ;// msg += "IRC_SERVER 461 ERR_NEEDMOREPARAMS :Not enough parameters\r\n";
        return ;
    }
    if (set)
    {
        if (!channel->isLocked())
        {
            channel->setPassword(string(arg[index]));
                SendMsg(*client, *channel, "MODE", "", channel->getName()+" +k "+channel->getPassword());
            // SendMsg(*client, *channel, "MODE", "", "+k "+channel->getPassword());
            // genericReplies(channel, client, PASSWORD_SET);
        }
        else
        {
            ;// msg += "IRC_SERVER 467 ERR_KEYSET :Channel key is already set\r\n";
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
                // genericReplies(channel, client, PASSWORD_UNSET);
            }
            else
                ;// msg += "IRC_SERVER 475 ERR_BADCHANNELKEY :Wrong channel key\r\n";
        }
        else
        {
            ;// msg += "IRC_SERVER 467 ERR_KEYSET :Channel key is already unset\r\n";
        }
    }
}

void    operatorController(Channel* const channel, Client* const client, bool set, size_t &index, vector<string> arg)
{
    if (++index >= arg.size())
    {
        ;// msg += ":"+client->getHstName()+" 461 "+client->getNckName()+" MODE :Not enough parameters\r\n";
        return ;
    }
    if (!channel->isOperator(client))
    {
        ;// msg += ":"+client->getHstName()+" 482 "+client->getNckName()+" "+channel->getName()+" :You're not channel operator\r\n";
        return ;
    }
    map<int, Client>::iterator it = Server::getInstance()->getClients().begin();
    map<int, Client>::iterator endit = Server::getInstance()->getClients().end();
    for (; it != endit; ++it)
    {
        if (it->second.getNckName() == arg[index])
            break ;
    }
    if (it == endit)
    {
        ;// msg += ": 401 ERR_NOSUCHNICK :No such nick\r\n";
    }
    if (set)
    {
        if (!channel->isMember(&it->second))
        {
            ;// msg += ": 401 ERR_NOSUCHNICK :No such nick\r\n";
        }
        else
        {
            if (!channel->isOperator(&it->second))
            {
                channel->setOperator(&it->second);
                // :irc.example.com 221 YourNick #example_channel +o OperatorNick
                // ErrorMsgGenrator(":irc_server 221 ", " " + client->getNckName() + " " + channel->getName() + " +o " + it->second.getNckName(), *client);
                SendMsg(*client, *channel, "MODE", "", channel->getName()+" +o "+it->second.getNckName());
                // genericReplies(channel, client, OPERATOR_ADDED);
            }
            else
            {

                ;// msg += ":IRC_SERVER is already admin " + arg[index] + " :is an operator\r\n";
            }
        }
    }
    else
    {
        if (channel->isOperator(&it->second))
        {
            channel->removeOperator(&it->second);
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" -o "+it->second.getNckName());
            // genericReplies(channel, client, OPERATOR_REMOVED);
        }
        else
        {
            ;// msg += ": 482 MODE " + arg[index] + " : not channel operator\r\n";
        }
    }
}

void    limitController(Channel* const channel, Client* const client, bool set, size_t &index, vector<string> arg)
{
    if (!channel->isOperator(client))
    {
        
        ;// msg += ":"+client->getHstName()+" 482 "+client->getNckName()+" "+channel->getName()+" :You're not channel operator\r\n";
    }
    if (set)
    {
        if (++index >= arg.size())
        {
            ;//msg += ":"+client->getHstName()+" 461 "+client->getNckName()+" MODE :Not enough parameters\r\n";
            ;// msg += ": 696 MODE : "+channel->getName()+" +l not enough parameters for limit mode\r\n";
            return ;
        }
        if (!channel->isLimited())
        {
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" +l "+arg[index]);
            // SendMsg(*client, *channel, "MODE", "", "+l "+arg[index]);
            channel->setLimit(atoll(arg[index].c_str()));
            // genericReplies(channel, client, LIMIT_SET);
        }
        else
        {
            ;// msg += "IRC_SERVER limited :Channel is already limited\r\n";
        }
    }
    else
    {
        if (channel->isLimited())
        {
            channel->unsetLimit();
            SendMsg(*client, *channel, "MODE", "", channel->getName()+" -l "+arg[index]);
            // SendMsg(*client, *channel, "MODE", "", "-l "+arg[index]);
            // genericReplies(channel, client, LIMIT_UNSET);
        }
        else
        {
            ;// msg += "IRC_SERVER  unlimited :Channel is already not unlimited\r\n";
        }
    }
}
static void printModes(Privelege priv, Client &client)
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
    ErrorMsgGenrator(":irc_server 324 ", " " + modes, client);
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
            keyController(channel, client, set, index, arg);
            break ;
        case 'o':
            operatorController(channel, client, set, index, arg);
            break ;
        case 'l':
            limitController(channel, client, set, index, arg);
            break ;
    }
}

void   Channel::modeCommand(vector<string> arg, Client* const client)
{
    modes   mode;
    queue<pair <bool, char> >   modesQueue;

    if (!arg.size())
        return (printModes(privelege, *client));
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
    // send(client->getClntFd(), msg.c_str(), msg.size(), 0);
}
