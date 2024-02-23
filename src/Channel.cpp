#include "Channel.hpp"

Channel::~Channel()
{
    delete this;
}

/**
 * @brief Constructs a Channel object.
 * 
 * @param client The client object to be added as an operator and member of the channel.
 * @param name The name of the channel.
 */
Channel::Channel(Client* const client, string name)
: name(name)
{
    memset(&this->privelege, 0, sizeof(this->privelege));
    this->operators.push_back(client);
    this->members.push_back(client);
    // Server::getInstance()->getChannels()[name] = this;
}

const std::string &Channel::getName() const
{
    return (this->name);
}

void   Channel::addMember(Client* const member)
{
    try
    {
        this->members.push_back(member);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::addOperator(Client* const op)
{
    try
    {
        this->operators.push_back(op);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void    Channel::invite(Client* const client)
{
    try
    {
        this->invited.push_back(client);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
void    Channel::removeMember(Client* const member)
{
    try
    {
        for (vector <Client *>::iterator it = this->members.begin(); it != this->members.end(); it++)
        {
            if ((*it)->getClntFd() == member->getClntFd())
            {
                this->members.erase(it);
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
void    Channel::removeOperator(Client* const op)
{
    try
    {
        for (vector <Client *>::iterator it = this->operators.begin(); it != this->operators.end(); it++)
        {
            if ((*it)->getClntFd() == op->getClntFd())
            {
                this->operators.erase(it);
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::removeInvited(Client* const client)
{
    try
    {
        for (vector <Client *>::iterator it = this->invited.begin(); it != this->invited.end(); it++)
        {
            if ((*it)->getClntFd() == client->getClntFd())
            {
                this->invited.erase(it);
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

vector <Client*> &Channel::getMembers() 
{
    return (this->members);
}

vector <Client*> &Channel::getOperators() 
{
    return (this->operators);
}

bool    Channel::isMember(Client* const member) const
{
    for (vector<Client*>::const_iterator it = this->members.begin(); it != this->members.end(); it++)
        if ((*it)->getClntFd() == member->getClntFd())
            return true;
    return (false);
}

bool    Channel::isOperator(Client* const admin) const
{
    for (vector<Client*>::const_iterator it = this->operators.begin(); it != this->operators.end(); it++)
        if ((*it)->getClntFd() == admin->getClntFd())
            return true;
    return (false);
}

bool    Channel::isInvited(Client* const client) const
{
    for (vector<Client*>::const_iterator it = this->invited.begin(); it != this->invited.end(); it++)
        if ((*it)->getClntFd() == client->getClntFd())
            return true;
    return (false);
}

void    Channel::setOperator(Client* const client)
{
    try
    {
        this->addOperator(client);
        // this->removeMember(client);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::unsetOperator(Client* const client)
{
    try
    {
        // this->addMember(client);
        this->removeOperator(client);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::kickUser(Client* const admin, Client* const user)
{
    try
    {
        if (this->isOperator(admin) && (this->isMember(user)))
        {
            this->removeMember(user);
        }
        else if (this->isOperator(admin) && this->isOperator(user))
        {
            this->removeOperator(user);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::setLimit(const size_t limit)
{
    this->privelege.limit = true;
    this->limit = limit;
}

void    Channel::setTopicBool()
{
    this->privelege.topic = true;
}

void    Channel::setTopic(const std::string topic)
{
    this->privelege.topic = true;
    this->topic = topic;
}

void    Channel::setPassword(const std::string password)
{
    this->privelege.password = true;
    this->password = password;
}

void    Channel::setIviteOnly()
{
    this->privelege.invitOnly = true;
}

void    Channel::unsetLimit()
{
    this->privelege.limit = false;
}

void    Channel::unsetTopic()
{
    this->privelege.topic = false;
}

void    Channel::unsetPassword()
{
    this->privelege.password = false;
}

void    Channel::unsetInviteOnly()
{
    this->privelege.invitOnly = false;
}

bool    Channel::isInviteOnly() const
{
    return (this->privelege.invitOnly);
}

bool    Channel::isLimited() const
{
    return (this->privelege.limit);
}

bool    Channel::isLocked() const
{
    return (this->privelege.password);
}

bool    Channel::isTopic() const
{
    return (this->privelege.topic);
}

std::string Channel::getTopic() const
{
    return (this->topic);
}

std::string Channel::getPassword() const
{
    return (this->password);
}

size_t      Channel::getLimit() const
{
    return (this->limit);
}
