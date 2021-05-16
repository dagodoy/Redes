#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

int main (int argc, char** argv)
{
        struct addrinfo hints;
        struct addrinfo * res;

        memset ((void*) &hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

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
		
	listen(sd, 16);

	struct sockaddr client;
	socklen_t clientlen = sizeof(struct sockaddr);

	int client_sd = accept(sd, &client, &clientlen);
        
	char host[NI_MAXHOST];	
	char serv[NI_MAXSERV];
		
        getnameinfo(&client, clientlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Conexión desde " << host << " " << serv << std::endl;

	while(true)
	{
		char buffer[80];
		
		int bytes = recv(client_sd, (void *) buffer, 79, 0);
	
		if (bytes <= 0)
		{
			std::cout << "Conexión terminada\n";
			break;	
		}
	
		buffer[bytes] = '\0';
		//buffer[strcspn(buffer, "\n")] = '\0';
		
		send(client_sd, buffer, bytes, 0);
	}

	close(sd);
        return 0;
}  
