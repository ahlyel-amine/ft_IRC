# **ft_IRC**

## **Navigation Table:**

1. [What is IRC protocole.](#what-is-irc-protocole)
1. [ft_IRC Usage](#ft_irc-usage)
    1. [Running the Server](#running-the-server)
    1. [Running the Bot](#running-the-bot)
1. [Available Commands](#available-commands)
1. [Authers & Final Mark](#authers--final-mark)
1. [Subject](./en.subject.pdf)

# **What is IRC protocole?**

Internet Relay Chat (IRC) is a text-based chat system for instant messaging. IRC is designed for group communication in discussion forums, called channels, but also allows one-on-one communication via private messages as well as chat and data transfer, including file sharing.[Read more](https://en.wikipedia.org/wiki/IRC)

---

# **ft_IRC Usage:**

## **Running the server:**

Run the makefile then wait for the project to compile until the executable is ready, you will find it in **bin** directory
after that, you can run the server as follows:

```bash
    ./ircserv <Port> <PassWord>
```

> N.B : You can connect to the server using the netcat command **nc** or the client used as reference which is [LimeChat](http://limechat.net/mac/).

---

## **Running the bot:**

To run the bot you can run the `make bot` and than wait for the executable to be ready which you can find in **bot/bin** directory, and you can use it as follows:

```bash
    ./bot < Server Name | Server IP > <Port> <PassWord> < 1 | 2 >
```

which the last parameter determines which type of bot you want to use.
1- is Informer(Emet) bot type which by useing it you can get BTC price by calling BTCPRICE (btcprice),
 or by using DATE (date) as a command you get the date of the day.
2- is ANONYMSG as the name suggest it help a user to send a completly annonymous msg to another user.

> N.B : If you are using another OS rather than macOS you may need to Download the `curl` library:
> ```bash
>   sudo apt-get install libcurl4-openssl-dev
>```

---

# **Available commands:**

| Commands | Usage | Discription |
|----------|-------|-------------|
| **PASS** | `pass <PassWord>` | The PASS command is used to set a ‘connection password’. |
| **NICK** | `nick <NickName>` | The NICK command is used to give the client a nickname or change the previous one. |
| **USER** | `user <username> 0 * <realname>` | The USER command is used at the beginning of a connection to specify the username and realname of a new user. |
| **JOIN** | `join <channel>{,<channel>} [<key>{,<key>}]` | This command is used to join an existing channel or create a new one if it does not exist |
| **PART** | `part <channel> <reason>` | The PART command removes the client from the given channel. On sending a successful PART command, the user will receive a PART message from the server for each channel they have been removed from. reason is the reason that the client has left the channel. |
| **KICK** | `kick <channel> <user> <reason>` | The KICK command can be used to request the forced removal of a user from a channel. It causes the user to be removed from the channel by force.|
| **QUIT** | `quit <reason>` | The QUIT command is used to terminate a client’s connection to the server. |
| **TOPIC** | `topic <channel> [<topic>]` | The TOPIC command is used to change or view the topic of the given channel. |
| **INVITE** | `invite <user> <channel>` | The INVITE command is used to invite a user to a channel. |
| **MODE** | `mode <target> [<modestring> [<mode arguments>...]]` | The MODE command is used to set or remove options (or modes) from a given target. |
| **PRIVMSG** | `privmsg <targets>{,<targets>} <message>` | The PRIVMSG command is used to send private messages between users, as well as to send messages to channels. |
| **ANONYMSG** | `anonymsg <target> <message>` | This is on of the bots the command it sends the targeted user an anonymus message. |
| **BTCPRICE** | `btcprice` | This command is also a bot command it is used to know the live price of the **BitCoin** |
| **DATE** | `date` | This command is also a bot command it is used to know the date of the **day** |
> **N.B:** To know more about the commands and there usage you can go [here](https://modern.ircdocs.horse/).

# **Authers & Final mark:**

This project was done bye [**jaguar-ks**](https://github.com/jaguar-ks), [**AhlyelAmine**](https://github.com/AhlyelAmine) and [**bleuuone**](https://github.com/bleuuone).

![final_mark](./final_mark.png)