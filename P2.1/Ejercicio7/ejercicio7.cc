#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <condition_variable>

std::condition_variable num_cv;
std::mutex num_mutex;
int num_clientes = 0;

#define MAX_THREAD 5

class MessageThread
{
public:
        MessageThread(int socket)
        {
                client_sd = socket;
        }
        int do_message()
        {
		     
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
                
                        send(client_sd, buffer, bytes, 0);
                }
		
		close(client_sd);

		num_mutex.lock();
    		num_clientes--;

    		if(num_clientes < 5)
    		{
        		num_cv.notify_all();
	    	}
	
		num_mutex.unlock();		
		std::cout << "Conexion terminada de verdad de la buena\n";	
       }
private:
        int client_sd;
};


main (int argc, char** argv)
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
	MessageThread *mt;

	while(true)
	{
		struct sockaddr client;
		socklen_t clientlen = sizeof(struct sockaddr);

        	{
        	    	std::unique_lock<std::mutex> lck(num_mutex);
            
        		while (num_clientes >= 5)
	        	{
	                	std::cout << "Esperando un hueco...\n";
		                num_cv.wait(lck);
           		}
        	}
		int client_sd = accept(sd, &client, &clientlen);

		char host[NI_MAXHOST];	
		char serv[NI_MAXSERV];
		
	        getnameinfo(&client, clientlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
	
      		std::cout << "Conexión desde " << host << " " << serv << std::endl;
	        
		num_mutex.lock();
	        num_clientes++;

        	std::cout << "CLIENTES: " << num_clientes << std::endl;
	        num_mutex.unlock();	        
		
		mt = new MessageThread(client_sd);

                std::thread([&mt](){
                        mt->do_message();

                }).detach();
	
		
	}
	delete mt;
	close(sd);
        return 0;
}  
