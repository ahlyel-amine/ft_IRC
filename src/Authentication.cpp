#include"Client.hpp"

/**
 * @brief Sets the nickname for the client.
 * 
 * This function sets the nickname for the client based on the provided command.
 * The nickname must meet certain criteria, such as starting with a letter or underscore,
 * and only containing alphanumeric characters, square brackets, curly brackets, vertical bars, backslashes, or underscores.
 * 
 * If the nickname is valid and not already in use by another client, it is set as the client's nickname.
 * Otherwise, an error message is sent to the client.
 * 
 * @param cmd The command containing the nickname to be set.
 * @return True if the nickname was successfully set, false otherwise.
 */

bool    Client::setNckName(vector<string> cmd)
{
    if (cmd.size() == 2) {
        if (isalpha(cmd[1].at(0)) || cmd[1].at(0) == '_') {
            string allowed = "[]{}|\\_";
            size_t i = 1;
            for (; i < cmd[1].size(); i++)
                if (!isalnum(cmd[1].at(i)) && allowed.find(cmd[1].at(i)) == string::npos)
                    break ;
            if (i != cmd[1].size())
                ErrorMsgGenrator(":IRCserv.1337.ma 432 ", " :Erroneus nickname", *this);
            else {
				if (Server::getInstance()->getClientByNckName(cmd[1]) > 0)
                    ErrorMsgGenrator(":IRCserv.1337.ma 433 ", " :Nickname is already in use", *this);
                else {
                    for (size_t i = 0; i < this->Chnls.size(); i++)
                        SendMsg(*this, *Server::getInstance()->getChannel(this->Chnls[i]), cmd[0], this->Chnls[i], cmd[1]);
                    this->NckName = cmd[1];
                    return true;
                }
            }
        }
        else
            ErrorMsgGenrator(":IRCserv.1337.ma 432 ", " :Erroneus nickname", *this);
    }
    else if (cmd.size() < 2)
        ErrorMsgGenrator(":IRCserv.1337.ma 431 ", " :No nickname given", *this);
    else
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " " + cmd[0] + " :To much parameters", *this);
    return (false);
}


/**
 * @brief Sets the username and real name of the client.
 * 
 * This function is responsible for setting the username and real name of the client based on the provided command.
 * If the command has the correct number of parameters and the client is not already registered, the username and real name are set.
 * Otherwise, an error message is sent to the client.
 * 
 * @param cmd The command containing the username and real name.
 * @return True if the username and real name are set successfully, false otherwise.
 */

bool		  Client::setUsrName(vector<string> cmd)
{
    if (cmd.size() == 5){
        if (!this->Regestred){
            this->UsrName = cmd[1];
            this->RlName = cmd[4];
            return true;
        }
        else
            ErrorMsgGenrator(":IRCserv.1337.ma 462 ", " :You may not reregister", *this);
    }
    else
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " " + cmd[0] + ((cmd.size() < 5) ? " :Not enough parameters" : " :Too much parameters"), *this);
    return (false);
}


/**
 * @brief Sets the server password for the client.
 * 
 * This function sets the server password for the client based on the provided command.
 * If the command has two elements and the client is not already registered, it checks if the provided password matches the server's password.
 * If the password is correct, it sets the server password for the client and returns true.
 * If the client is already registered, it sends an error message indicating that the client may not reregister.
 * If the password is incorrect, it sends an error message indicating that the password is incorrect.
 * If the command does not have enough parameters, it sends an error message indicating that there are not enough parameters.
 * 
 * @param cmd The command containing the server password.
 * @return True if the server password is set successfully, false otherwise.
 */

bool		  Client::setSrvPss(vector<string> cmd)
{
    if (cmd.size() == 2) {
        if (this->Regestred)
            ErrorMsgGenrator(":IRCserv.1337.ma 462 ", " :You may not reregister", *this);
        else if(Server::getInstance()->getPswd() == cmd[1]) {
            this->SrvPss = cmd[1];
            return true;
        }
        else
            ErrorMsgGenrator(":IRCserv.1337.ma 464 ", " :Password incorrect", *this);
    }
    else
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " " + cmd[0] + ((cmd.size() < 2) ? " :Not enough parameters" : " :Too much parameters"), *this);
    return (false);
}
