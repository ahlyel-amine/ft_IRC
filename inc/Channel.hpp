#pragma once

#include "Client.hpp"

class Client;

using namespace std;

typedef struct Privelege
{
    bool    invitOnly;
    bool    limit;
    bool    password;
    bool    topic;
} Privelege;

class Channel
{
    size_t              limit;
    string              name;
    string              password;
    string              topic;
    vector <Client*>    members;
    vector <Client*>    operators;
    vector <Client*>    invited;
    Privelege           privelege;
    
    public :
    Channel(){};

        void    addMember(Client* const);
        void    addOperator(Client* const);
        void    invite(Client* const);

        void    removeMember(Client* const);
        void    removeOperator(Client* const);
        void    removeInvited(Client* const);

        vector <Client*> &getMembers() ;
        vector <Client*> &getOperators() ;

        bool    isMember(Client* const) const;
        bool    isOperator(Client* const) const;
        bool    isInvited(Client* const)  const;

        void    setOperator(Client* const);
        void    unsetOperator(Client* const);

        void    kickUser(Client* constadmin, Client* constmember);

        void    setTopicBool();
        void    setLimit(const size_t limit);
        void    setTopic(const string topic);
        void    setPassword(const string password);
        void    setIviteOnly();

        void    unsetLimit();
        void    unsetTopic();
        void    unsetPassword();
        void    unsetInviteOnly();

        bool    isInviteOnly() const;
        bool    isLimited() const;
        bool    isLocked() const;
        bool    isTopic() const;

        string getTopic() const;
        string getPassword() const;
        size_t      getLimit() const;
        vector <Client*>      &getInvited(){return this->invited;};

        Channel(Client* const client, string name);
        ~Channel();
        void   modeCommand(vector<string> arg, Client* const client);
        void   inviteCommand(vector<string> arg, Client* const client);
        const string   &getName() const;        
};
