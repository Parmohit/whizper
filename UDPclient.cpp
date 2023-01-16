#include <iostream>
#include <exception>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT "4950"

int main(int argc, char **argv)
{
	try
	{
		if(argc != 3)
		{
			std::cerr << "Input format: ProgramName hostname Message\n";
			return -1;
		}

		int sockfd{-1}, rv{-1};
		struct addrinfo hints, *p{nullptr}, *res{nullptr};

		memset(&hints, 0, sizeof(hints));
		hints.ai_family   = AF_INET6;
		hints.ai_socktype = SOCK_DGRAM;

		if((rv=getaddrinfo(argv[1], PORT, &hints, &res)) != 0)
		{
			std::cerr << "getaddrinfo failed:" <<gai_strerror(rv) << std::endl;
			return 1;
		}

		for(p=res; p!=NULL;p=p->ai_next)
		{
			if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			{
				std::cerr << "socket creation failed\n";
				continue;
			}
			break;
		}

		freeaddrinfo(res);
		if(p==NULL)
		{
			close(sockfd);
			std::cerr << "No valid socket available\n";
			return 2;
		}
		
		int nBytes{0};
		if((nBytes=sendto(sockfd, argv[2], sizeof(argv[2]), 0, p->ai_addr, p->ai_addrlen)) == -1)
		{
			close(sockfd);
			std::cerr << "Sendto failed\n";
			return 3;
		}
		std::cout <<"Message: " << argv[2] << " To: " << argv[1];

		return 0;
	} catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return -1;
	}
}
