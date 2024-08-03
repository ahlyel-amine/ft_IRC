# **ft_IRC**

## **Navigation Table:**

1. [What is IRC protocole.](#what-is-irc-protocole)
1. [Network System calls](#network-system-calls)
	1. [What is a Socket](#what-is-a-socket)
	1. [Types of Internet Sockets](#types-of-internet-sockets)
	1. [System Calls](#system-calls)
1. [ft_IRC Usage](#ft_irc-usage)
    1. [Running the Server](#running-the-server)
    1. [Running the Bot](#running-the-bot)
1. [Available Commands](#available-commands)
1. [Authers & Final Mark](#authers--final-mark)
1. [Subject](./en.subject.pdf)

# **What is IRC protocole?**

Internet Relay Chat (IRC) is a text-based chat system for instant messaging. IRC is designed for group communication in discussion forums, called channels, but also allows one-on-one communication via private messages as well as chat and data transfer, including file sharing.[Read more](https://en.wikipedia.org/wiki/IRC)

---

# **Network System calls we used:**

Before start building we need of course to know the core finctionalties of some Network system calls as well.

## *What is a socket?*

- socket is a way to speak to other programs using standard Unix file descriptors.
- those files are special designed for network communication.

* For now rise the quation, How we get those kind of files?
- By a simple discription "socket()" system call came to take care of creating for us an endpoint for 
that pertuculer communication and return back a socket descriptor. by now you can communicaye through it using 
that socket by "send()" and "recv()" calls. keep in mind that u can use "read()" and "write()" but the previous
ones offer much greater control over your data transmission.

### *Types of Internet Sockets*

1. **TCP Sockets or Stream Sockers**.
	* TCP sockets provide reliable, connection-oriented communication between processes over a network.
	* They guarantee that data will be delivered in the same order it was sent and without errors.
	* TCP sockets establish a connection before transferring data, ensuring a reliable stream of data between the    	client  and server.
	* They are suitable for applications where data integrity and order are crucial, such as web browsing, email, file transfer (e.g., FTP), and remote shell access (e.g., SSH).
2. **UDP Sockets or Datagram Sockets**.
	* UDP sockets provide unreliable, connectionless communication between processes over a network.
	* They do not guarantee delivery or order of data and do not perform error checking or correction.
	* UDP sockets are more lightweight and have lower overhead compared to TCP sockets, making them suitable for real-time applications where low latency is more critical than data reliability.
	* They are commonly used in applications such as online gaming, streaming media, DNS (Domain Name System), and voice/video conferencing.
3. **Raw Sockets**.
	* Raw sockets allow direct access to network protocols at the transport layer (e.g., IP, ICMP). They provide low-level access to network packets and are often used for network monitoring, packet sniffing, and protocol development.
4. **Unix Domain Sockets.**
	*  Unix domain sockets provide communication between processes on the same host using the file system namespace instead of the network stack. They are used for inter-process communication (IPC) on Unix-like operating systems and offer higher performance and security than TCP or UDP sockets.

#### *SYSTEM CALLS*

1. **socket()** :
	* Get for u the file Descriptor, in case of a failure it retuen "-1"
2. **bind()** :
	* It has a crucial role by associatting the socket within specific network address and port number.
3. **setsocketopt()** ;
	* The setsockopt() function sets options associated with a socket. Options can exist at multiple protocol levels.
	
	
	```C
	#define _XOPEN_SOURCE_EXTENDED 1
	#include <sys/socket.h>

	int setsockopt(int socket, int level, int option_name,
				const void *option_value, socklen_t option_length);

	```

   	|    Parameter	 |  	 Description         				|
	| :-------------:| :--------------------------------------------- 	|
   	|  socket 	 | The socket descriptor.    				|
	|		 |							|
	|  level         | The level for which the option is being set.		|
	|		 |	     						|
   	|  option_name	 | The name of a specified socket option. 		|
	|		 | 				       			|
   	|  option_value  | The pointer to option data. 				|
	|		 |                    					|
   	|  option_length | The length of the option data.			|

4. **Listen()** :
	* *Before you can accept incoming connections on a socket, you need to inform the operating system that the socket will be used for listening. This is where the listen() function comes in.*

5. **accept()** :
	*  Is a fundamental function in socket programming that allows a server to accept incoming connections from clients, creating a new socket for each accepted connection and enabling bidirectional communication between the server and client

	```C
		#include <sys/types.h>
		#include <sys/socket.h>
		int accept(int socket, struct sockaddr *addr, socklen_t *addrlen);
	```

	|    Parameter	 |  	 Description         				 |
	| :-------------:| :---------------------------------------		 |
	|	socket   | The listen()ing socket descriptor    		 |
	|		 |							 |
	| 	 addr    | The struct the holds infos about incomming connection.|
	|		 |	      						 |
	|	addrlen	 | The lenght of sockaddr struct.			 |

	
5. **send()/recv()**
	* *These two functions are for communicating over stream sockets or connected datagram sockets.*

6. **poll()—Synchronous I/O Multiplexing**
	* *to monitor multiple file descriptors (such as sockets, pipes, or files) for events without blocking the execution of the program.*

	```C
	#include <poll.h>
	int poll(struct pollfd fds[], nfds_t nfds, int timeout);
	```
	|     Parameter	 |  	 Description         					|
	| :-------------:| :----------------------------------------------------------  |
	|fds		 | The array of information (which sockets to monitor for what) |
	|		 |								|
	|addr	         | The count of elements in the array				|
	|		 |	     							|
	|timeout	 | The timeout in milliseconds		 			|

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
## **Running with Docker Compose:**

1. **Build and Start the Server and Bot:**

   Ensure you have Docker and Docker Compose installed. Navigate to the root directory of your project, then run:

   	```bash
   docker-compose up --build -d
	```
2. **Show logs**

	```c
	make C=container_name logs 
	```

	|     container_name	 |  	 Description         					|
	| :-------------:| :----------------------------------------------------------  |
	| irc_server  | Attach terminal to irc_server logs|
	|anonymous_bot| Attach terminal to anonymous_bot logs|
	|btc_price_bot| Attach terminal to btc_price_bot logs|

> N.B : if C didnt specified the default is irc_server
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

This project was done by [**jaguar-ks**](https://github.com/jaguar-ks), [**ahlyel-amine**](https://github.com/ahlyel-amine) and [**bleuuone**](https://github.com/bleuuone).

![final_mark](./final_mark.png)
