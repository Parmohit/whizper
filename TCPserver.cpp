#include <iostream>
#include <sys/socket.h>
#include <exception>
#include <netdb.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10
#define PORT "3490"

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

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char **argv)
{
	try
	{
		int status{0};
		struct addrinfo hints, *p{nullptr}, *res{nullptr};

		memset(&hints, 0, sizeof(hints));
		hints.ai_family   = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags    = AI_PASSIVE;

		if((status=getaddrinfo(NULL, PORT, &hints, &res)) != 0)
		{
			std::cerr << "GETADDRINFO FAILED!\n";
			return 1;
		}
		
		int sockfd{};
		for(p=res; p!=NULL; p=p->ai_next)
		{
			if((sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			{
				std::cerr << "Socket creation failed\n";
				continue;
			}
			
			int yes = 1;
			if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
			{
				std::cerr << "setsocketopt failed\n";
				return 3;
			}
			if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
			{
				close(sockfd);
				std::cerr << "Binding operation failed\n";
				continue;
			}
			break;
			
		}

		freeaddrinfo(res);
		// check if any valid node was picked
		if(p==NULL)
		{
			std::cerr << "No valid nodes in returned local address\n";
			return 2;
		}

		if(listen(sockfd, BACKLOG) == -1)
		{
			std::cerr << "listen operation failed!\n";
			return 4;
		}
		
		struct sigaction sa;
		sa.sa_handler = sigchld_handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;

		if(sigaction(SIGCHLD, &sa, NULL) == -1) // termination signal of child is SIGCHLD
		{
			std::cerr << "sigaction\n";
			return 7;
		}	

		struct sockaddr_storage theiraddr;
		socklen_t sin_size{sizeof(theiraddr)}; 
		
		int new_fd {};
		char s[INET6_ADDRSTRLEN];
		while(1)
		{
			sin_size = sizeof(theiraddr);
			new_fd = accept(sockfd, (struct sockaddr*)&theiraddr, &sin_size);
			if(new_fd == -1)
			{
				std::cerr << "Accept failed!\n";
				continue;
			}
		
			inet_ntop(theiraddr.ss_family, get_in_addr((struct sockaddr*)&theiraddr), s, sizeof(s));
				
			std::cout << "server: got connection from:" << s << "\n";
			if(!fork())
			{
				close(sockfd );
				std::string str;
				while(1) // exit this loop with input from GUI
				{
					std::getline(std::cin, str);
					if(send(new_fd, str.c_str(), str.length(), 0) == -1)
					{
						std::cerr << "sending failed\n";
					}
				}
				close(new_fd);
				exit(0);
			}
			close(new_fd);
		
		}
		close(sockfd);
		return 0;

	} catch(const std::exception& e)
	{
		std::cerr << "Exception :" << e.what() << std::endl;
		return -1;
	}
}
