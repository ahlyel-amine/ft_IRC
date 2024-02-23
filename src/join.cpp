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
// void    ErrorMsgGenrator(string const &Prefix, string const &Sufix, Client &Sender) {
    // string msg = Prefix + ((Sender.getNckName().empty()) ? "*" : Sender.getNckName())
                // + Sufix + "\r\n";
// 
    // if (send(Sender.getClntFd(), msg.c_str(), msg.size(), 0) < 0)
        // Server::getInstance()->RemoveClient(Sender.getClntFd());
// }
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
string listMembers(Channel &ch) {
    string str;
    vector<Client *> Mbrs = ch.getMembers();
    for (size_t i = 0; i < Mbrs.size(); i++)
        str += ((ch.isOperator(Mbrs[i])) ? "@" : "") + Mbrs[i]->getNckName() + ((i+1!=Mbrs.size()) ? " " : ""); 
    return str;
}

bool    Client::joinCommand(vector<string> join)
{
    queue<string>   channels;
    queue<string>   passwords;
    string          msg;

    if (!this->Regestred)
    {
        ErrorMsgGenrator(":IRCserv.1337.ma 451 ", " " + join[0] + " :You have not registered", *this);
        return (false);
    }

    joinParser(join, channels, passwords, *this);

    while (!channels.empty())
    {
        map<string, Channel*>                &Channels =  Server::getInstance()->getChannels();
        map<string, Channel*>::iterator      search   =  Channels.find(channels.front());


        if (channels.front().size() == 1 && channels.front()[0] == '0')
        {
            for (size_t i = 0; i < this->Chnls.size(); i++)
            {
                SendMsg(*this, *Channels[this->Chnls[i]], "PART", "", this->Chnls[i]);
                Channels[this->Chnls[i]]->removeMember(this);
                Channels[this->Chnls[i]]->removeOperator(this);
                Channels[this->Chnls[i]]->removeInvited(this);
            }
            this->Chnls.clear();
            channels.pop();
            msg = "";
            continue;
        }
        if (!Channels.count(channels.front()))
        {
            Channel *chnl = new Channel(this, channels.front());

            Channels[channels.front()] = chnl;

            // cout << "channel created" << Channels[channels.front()]->getName() << " : " << channels.front() << endl;
            SendMsg(*this, *this, join[0], "", channels.front());
            ErrorMsgGenrator("IRCserv.1337.ma 353 ", " = " + channels.front() + " :@"+this->NckName, *this);
            ErrorMsgGenrator("IRCserv.1337.ma 366 ", " " + channels.front() + " :End of /NAMES list.", *this);
            this->Chnls.push_back(channels.front());
        }
        else
        {

            if (search->second->isLocked())
            {
                if (passwords.empty() || (search->second->getPassword() != passwords.front()))
                {
                    ErrorMsgGenrator(":IRCserv.1337.ma 475 ", " " + channels.front() + " :Cannot join channel (+k) - bad key", *this);
                    channels.pop();
                    continue;
                }
                else {
                    passwords.pop();
                }
            }

            if (search->second->isLimited())
            {
                if (search->second->getLimit() <= search->second->getMembers().size())
                {
                    ErrorMsgGenrator(":IRCserv.1337.ma 471 ", " " + channels.front() + " :Cannot join channel (+l) - channel is full", *this);
                    channels.pop();
                    continue;
                }
            }

            if (search->second->isMember(this))
            {
                ErrorMsgGenrator(":IRCserv.1337.ma 443 ", " " + channels.front() + " :You are already on that channel", *this);
                channels.pop();
                continue;
            }

            if (search->second->isInviteOnly())
            {
                if (!search->second->isInvited(this))
                {
                    ErrorMsgGenrator(":IRCserv.1337.ma 473 ", " " + channels.front() + " :Cannot join channel (+i) - You must be invited", *this);
                    // msg += "473  JOIN :Cannot join channel (+i)\r\n";
                    channels.pop();
                    continue;
                }
                else
                {
                    search->second->removeInvited(this);
                }
            }

            search->second->addMember(this);
            this->Chnls.push_back(channels.front());
            SendMsg(*this, *search->second, join[0], "", channels.front());
            ErrorMsgGenrator("IRCserv.1337.ma 353 ", " = " + channels.front() + " :"+listMembers(*search->second), *this);
            ErrorMsgGenrator("IRCserv.1337.ma 366 ", " " + channels.front() + " :End of /NAMES list.", *this);
        }
        channels.pop();
    }
    send(this->ClntFd, msg.c_str(), msg.size(), 0);
    return (true);
}
