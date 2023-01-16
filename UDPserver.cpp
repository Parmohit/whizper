#include <iostream>
#include <exception>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

#define PORT "4950"
#define MAXBUFLEN 20

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET)
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
		struct addrinfo hints, *p{nullptr}, *res{nullptr};
		struct sockaddr_storage theiraddr;
		int status{}, sockfd{-1}, yes = 1;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family   = AF_INET6;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags     = AI_PASSIVE;

		if((status=getaddrinfo(NULL, PORT, &hints, &res)) != 0)
		{
			std::cerr << gai_strerror(status);
			return 1;
		}

		for(p=res; p!=NULL; p=p->ai_next)
		{
			if((sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			{
				std::cerr << "socket creation failed\n";
				continue;
			}

			if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))==-1)
			{
				std::cerr << "socketopt failed\n";
				return 2;
			}
			if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
			{
				close(sockfd);
				std::cerr << "failed to bind\n";
				continue;
			}
			break;
		}
		
		freeaddrinfo(res);
		if(p==NULL)
		{
			std::cerr << "listener failed to bind\n";
			return 3;
		}	
		
		socklen_t addr_len;
		int nBytes{0};
		char buff[MAXBUFLEN], s[INET6_ADDRSTRLEN];
		addr_len = sizeof(addr_len);

		if((nBytes=recvfrom(sockfd, buff, MAXBUFLEN-1, 0,  (struct sockaddr*)&theiraddr, &addr_len)) == -1)
		{
			close(sockfd);
			std::cerr << "recvfrom failed\n";
			return 4;
		}
		buff[nBytes] = '\0';
		inet_ntop(theiraddr.ss_family, get_in_addr((struct sockaddr*)&theiraddr), s, sizeof(s));
		
		std::cout << "Received from IP " << s << ":" << buff << std::endl;
		close(sockfd);

	} catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return -1;
	}
}
