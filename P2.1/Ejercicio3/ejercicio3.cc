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

	int send = sendto(sd, argv[3], sizeof argv[3], 0, res->ai_addr, res->ai_addrlen);
	if (send == -1)
	{	
		std::cerr << "[sendto]: error al enviar mensaje" << std::endl;
	}

	char buffer[80];	
	int bytes = recvfrom(sd, (void *) buffer, 79, 0, res->ai_addr, &res->ai_addrlen);
	if (bytes == -1)
        {
        	return -1;
        }
	std::cout << buffer << std::endl;
        freeaddrinfo(res);
	close(sd);
        return 0;
}  
