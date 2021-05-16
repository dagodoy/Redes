#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#define MAX_THREAD 5

class MessageThread
{
public:
	MessageThread(int socket)
	{
		sd = socket;	
	}
	int do_message()
	{
		while(true)
		{
			char buffer[80];
	        	char host[NI_MAXHOST];
			char serv[NI_MAXSERV];		

			time_t timesec;
			struct tm * ftime;
			char timebuffer[80];
	
			struct sockaddr client;
			socklen_t clientlen = sizeof(struct sockaddr);

			int bytes = recvfrom(sd, (void *) buffer, 79, 0, &client, &clientlen);
			buffer[bytes] = '\0';
			buffer[strcspn(buffer, "\n")] = '\0';
	                getnameinfo(&client, clientlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
	                std::cout << bytes << " bytes de " << host << ":" << serv << " id: " << std::this_thread::get_id() << std::endl;

			if (bytes == -1)
			{
				return -1;
			}
			if (strcmp(buffer, "t") == 0)
			{
				memset(timebuffer, 0, sizeof timebuffer);	
				time(&timesec);
				ftime = localtime(&timesec);
				strftime(timebuffer, sizeof timebuffer,"%X %p", ftime);
				sendto(sd, timebuffer, sizeof timebuffer, 0, &client, clientlen); 
			}
			else if (strcmp(buffer, "d") == 0)
			{
				memset(timebuffer, 0, sizeof timebuffer);
				time(&timesec);	
				ftime = localtime(&timesec);
				strftime(timebuffer, sizeof timebuffer,"%F", ftime);
				sendto(sd, timebuffer, sizeof timebuffer, 0, &client, clientlen); 
			}
			else if (strcmp(buffer, "q") == 0)
			{ 
				std::cout << "Saliendo..." << std::endl;
				close(sd);	
				return 0;
			}
			else
			{
				std::cout << "Comando no soportado " << buffer;
			}
			sleep(20);
		}
	}
private:
	int sd;
};
int main (int argc, char** argv)
{
        struct addrinfo hints;
        struct addrinfo * res;

        memset ((void*) &hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

        int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
        if (rc != 0)
        {
                std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
                return -1;
        }

	int sd = socket(res->ai_family, res->ai_socktype, 0);
	if (sd == -1)
	{
		std::cerr << "[socket]: error" << std::endl;
		return -1;
	}

	bind(sd, res->ai_addr, res->ai_addrlen);

        freeaddrinfo(res);

	for (int i = 0; i < MAX_THREAD; i++)
	{
		MessageThread *mt = new MessageThread(sd);

		std::thread([&mt](){
			mt->do_message();

			delete mt;
		}).detach();
	}
	char in[80];
	while (strcmp(in, "q") != 0)
	{
		std::cin >> in;
	}
	
	close(sd);
        return 0;
}  
