#include"Client.hpp"

/**
 * @brief Kicks a client from a channel.
 * 
 * @param cmd The command containing the channel name and the nickname of the client to be kicked.
 * @return True if the client was successfully kicked, false otherwise.
 */

// bool    Client::Kick(vector<string> cmd) {
//     Server *srv = Server::getInstance();
//     if (cmd.size() == 4 || cmd.size() == 3)
//     {
//         if (srv->isChannel(cmd[1]))//srv->getChannels().find(cmd[1]) != srv->getChannels().end()) // 
//         {
//             Channel *chnl =srv->getChannel(cmd[1]);
//             if (!chnl->isMember(this)) 
//             {
//                 if (chnl->isOperator(this)) 
//                 {
//                     if (srv->getClientByNckName(cmd[2]) < 0) 
//                     {
//                         ErrorMsgGenrator(":IRCserv.1337.ma 401 ", " " + cmd[2] + " :No such nick", *this);
//                         return false;
//                     }
//                     vector<Client *>::iterator it = chnl->getMembers().begin();
//                     for (; it != chnl->getMembers().end(); it++)
//                     {
//                         if ((*it)->NckName == cmd[2])
//                             break;
//                     }
//                     if (it == chnl->getMembers().end()) 
//                     {
//                         ErrorMsgGenrator(":IRCserv.1337.ma 441 ", " " + cmd[2] + " " + cmd[1] + " :They aren't on that channel", *this);
//                         return false;
//                     }
//                     if (cmd.size() == 3)
//                         SendMsg(*this, *chnl, cmd[0], " " , cmd[1] + " " + cmd[2]);
//                     else
//                         SendMsg(*this, *chnl, cmd[0], cmd[3] , cmd[1] + " " + cmd[2]);
//                     VcRemove((*it)->Chnls, cmd[1]);
//                     chnl->kickUser(this, *it);
//                     if (chnl->getMembers().empty()) 
//                     {
//                         delete chnl;
//                         srv->getChannels().erase(cmd[1]);
//                     }
//                     return true;
//                 }
//                 else 
//                 {
//                     ErrorMsgGenrator(":IRCserv.1337.ma 482 ", " " + cmd[1] + " :You're not channel operator", *this);
//                     return false;
//                 }
//             }
//             else 
//             {
//                 ErrorMsgGenrator(":IRCserv.1337.ma 442 ", " " + cmd[1] + " :You're not on that channel", *this);
//                 return false;
//             }
//         }
//         else
//         {
//             ErrorMsgGenrator(":IRCserv.1337.ma 403 ", " " + cmd[1] + " :No such channel", *this);
//             return false;
//         }
//     }
//     else
//     {
//     }
// }

bool Client::Kick(vector<string> cmd) {
    if (cmd.size() == 3  || cmd.size() == 4) {
        Server *srv = Server::getInstance();
        if (srv->isChannel(cmd[1])) {
            Channel *chnl = srv->getChannel(cmd[1]);
            if (chnl->isOperator(this)) {
                if (srv->isClient(cmd[2])) {
                    Client &clt = srv->getClient(cmd[2]);
                    if (chnl->isMember(&clt)) {
                        SendMsg(*this, *chnl, cmd[0], ((cmd.size() == 4) ? cmd[3] : ""), cmd[1] + " " + cmd[2]);
                        VcRemove(clt.Chnls, cmd[1]);
                        chnl->kickUser(this, &clt);
                        if (chnl->getMembers().empty()) {
                            delete chnl;
                            srv->getChannels().erase(cmd[1]);
                        }
                        return true;
                    }
                    else
                        ErrorMsgGenrator(":IRCserv.1337.ma 441 ", " " + cmd[2] + " " + cmd[1] + " :They aren't on that channel", *this);
                }
                else
                    ErrorMsgGenrator(":IRCserv.1337.ma 401 ", " " + cmd[2] + " :No such nick", *this);
            }
            else if (chnl->isMember(this))
                ErrorMsgGenrator(":IRCserv.1337.ma 482 ", " " + cmd[1] + " :You're not channel operator", *this);
            else
                ErrorMsgGenrator(":IRCserv.1337.ma 442 ", " " + cmd[1] + " :You're not on that channel", *this);
        }
        else
            ErrorMsgGenrator(":IRCserv.1337.ma 403 ", " " + cmd[1] + " :No such channel", *this);
    }
    else
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " " + cmd[0] + ((cmd.size() < 4) ? " :Not enough parameters" : " :Too much parameters"), *this);
    return false;
}

// bool Client::Kick(vector<string> cmd) {

//     if (cmd.size() != 3  && cmd.size() != 4)
//     {
//         ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " " + cmd[0] + ((cmd.size() < 4) ? " :Not enough parameters" : " :Too much parameters"), *this);
//         return (false);
//     }

//     Server *srv = Server::getInstance();
//     if (!srv->isChannel(cmd[1]))
//     {
//         ErrorMsgGenrator(":IRCserv.1337.ma 403 ", " " + cmd[1] + " :No such channel", *this);
//         return (false);
//     }

//     Channel *chnl = srv->getChannel(cmd[1]);
//     if (!chnl->isMember(this))
//     {
//         ErrorMsgGenrator(":IRCserv.1337.ma 442 ", " " + cmd[1] + " :You're not on that channel", *this);
//         return (false);
//     }

//     if (!chnl->isOperator(this))
//     {
//         ErrorMsgGenrator(":IRCserv.1337.ma 482 ", " " + cmd[1] + " :You're not channel operator", *this);
//         return (false);
//     }

//     if (!srv->isClient(cmd[2])) 
//     {
//         ErrorMsgGenrator(":IRCserv.1337.ma 401 ", " " + cmd[2] + " :No such nick", *this);
//         return (false);
//     }

//     Client &clt = srv->getClient(cmd[2]);
//     if (chnl->isMember(&clt)) 
//     {
//         ErrorMsgGenrator(":IRCserv.1337.ma 441 ", " " + cmd[2] + " " + cmd[1] + " :They aren't on that channel", *this);
//         return (false);
//     }

//     SendMsg(*this, *chnl, cmd[0], ((cmd.size() == 4) ? cmd[3] : ""), cmd[1] + " " + cmd[2]);
//     VcRemove(clt.Chnls, cmd[1]);
//     chnl->kickUser(this, &clt);
//     if (chnl->getMembers().empty()) 
//     {
//         delete chnl;
//         srv->getChannels().erase(cmd[1]);
//     }
//     return true;
// }

