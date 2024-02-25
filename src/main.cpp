#include "Server.hpp"

bool interpted = false;

void hundl(int) {interpted = true;}

// void f(void){system("lsof -c ircserv");}

int main(int ac, char **av) {
    // atexit(f);
    if(ac == 3) {
        signal(SIGINT, hundl);
        signal(SIGPIPE, SIG_IGN);
        string pwd(av[2]);
        string prt(av[1]);
        char hstname[256];
		Server *srv = Server::InstanceServer(prt, pwd);
		if (gethostname(hstname, sizeof(hstname)) == -1) {
            cerr << "Error :" << strerror(errno) << endl;
            exit(1);
        }
        cout << srv->Welcome() << endl;
        cout << BLU << "\t\t[Server Started]\n" << endl
            << "\tPort[" << C_CLS << prt << BLU << "]\t|\tIP["
            << C_CLS << hstname << BLU << "]" << C_CLS << endl;
        while (!interpted)
        {
			try {
				srv->launchServer();
			}
            catch(std::exception& e) {
				cerr << e.what() << endl;
				break;
			}
		}
        delete srv;
        // system("leaks ircserv");
    }
    else {
        cerr << "Invalide Arguments : Usage : ./ircserv <port> <password>" << endl;
        return 1;
    }
}
