#include "Server.hpp"

bool interpted = false;

void hundl(int) {interpted = true;}

void f(void){system("lsof -c ircserv");}

int main(int ac, char **av) {
    atexit(f);
    if(ac == 3) {
        signal(SIGINT, hundl);
        signal(SIGPIPE, SIG_IGN);
        string pwd(av[2]);
        string prt(av[1]);
		
        Server *srv = Server::InstanceServer(prt, pwd);
		cout << '\n' << srv->Welcome() << endl;
        cout << "\t\t[Server Started]" << endl << "Port: " << prt << endl;
        while (!interpted)
            srv->launchServer();
        delete srv;
        system("leaks ircserv");
    }
    else {
        cerr << "Invalide Arguments : Usage : ./ircserv <port> <password>" << endl;
        return 1;
    }
}
