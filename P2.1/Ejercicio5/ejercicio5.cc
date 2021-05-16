#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
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

	int conn = connect(sd, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
	if (conn == -1)
	{
		std::cerr << "[connect]: fallo al conectar\n";
		return -1;
	}
	char buffer[80];
	while (true)
	{
		std::cin >> buffer;
		if (strcmp(buffer,"Q") == 0)
		{
			int snd = send(sd, NULL, 0, 0);
			if (snd == -1)
			{	
				std::cerr << "[send]: error al enviar mensaje" << std::endl;
				return -1;
			}
			close(sd);
			return 0;
		}
		int snd = send(sd, buffer, 79, 0);
		if (snd == -1)
		{	
			std::cerr << "[send]: error al enviar mensaje" << std::endl;
		}		

		int bytes = recv(sd, (void *) buffer, 79, 0);
		if (bytes == -1)
        	{
        		return -1;
        	}
		buffer[bytes] = '\0';
		std::cout << buffer << std::endl;
	}
	close(sd);
        return 0;
}  
