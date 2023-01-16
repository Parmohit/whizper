// NOTE: Check out below link for detailed explanation 
// https://pubs.opengroup.org/onlinepubs/009604499/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <exception>

#define PORT "3490"
#define MAXDATASIZE 100

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET) //IPv4
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	else
	{
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}
}

int main(int argc, char **argv)
{
	try
	{
		if(argc != 2)
		{
			std::cerr << "Usage: client hostname\n";
			return -1;
		}

		struct addrinfo hints, *p{nullptr}, *res{nullptr};
		int sockfd{},rv{};
		char s[INET6_ADDRSTRLEN];
			
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype   = SOCK_STREAM;

		if((rv=getaddrinfo(argv[1], PORT, &hints, &res)) != 0)
		{
			std::cerr << "getaddrinfo failed: " << gai_strerror(rv);
			return 1;
		}
		
		for(p=res; p!=NULL;p=p->ai_next)
		{
			if((sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			{
				std::cerr << "socket creation failed\n";
				continue;
			}
			if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
			{
				close(sockfd);
				std::cerr << "connect failed\n";
				continue;
			}

			break;
		}
			
		freeaddrinfo(res);
		if(p == NULL)
		{
			std::cerr << "client failed to connect\n";
			return 2;
		}
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof(s));
		
		std::cout << "Connected to IP:" << s << std::endl;
		
		int numBytes{1};
		char buf[MAXDATASIZE];
		while(1) // exit this loop with input from GUI
		{	 // use send here as well
			if((numBytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
			{
				std::cerr << "Receive failed!!";
				return -1;
			}
		
			buf[numBytes] = '\0';
			std::cout << "Client received : " << buf << std::endl;
		}
		close(sockfd);
		return 0;

	}
	catch(const std::exception& exception)
	{
		std::cerr << "Exception raised: "<< exception.what() << std::endl;
	}
	
	return 0;
}
