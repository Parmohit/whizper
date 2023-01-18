// Use telnet as users connected to localhost and port "3490"

#include <cstring>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT "3490"
#define BACKLOG 5
#define MAXDATASIZE 256

int get_listener_socket()
{
	struct addrinfo hints, *p{nullptr}, *res{nullptr};

	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	int rv{-1}, sockfd{-1}, yes{1};
	if((rv=getaddrinfo(NULL, PORT, &hints, &res)) != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rv);
		return -1;
	}

	for(p=res; p!=NULL; p=p->ai_next)
	{
		if((sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cerr << "socket creation failed!\n";
			continue;
		}
		
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			std::cerr << "Binding failed!\n";
			continue;
		}
		break;
	}

	freeaddrinfo(res);
	if(p==NULL)
	{
		std::cerr << "No valid addrinfo found for sokcet!\n";
		return -1;
	}

	if(listen(sockfd, BACKLOG) == -1)
	{
		close(sockfd);
		std::cerr << "Listen failed!\n";
		return -1;
	}

	return sockfd;
}

void add_to_pfds(int newfd, int &fd_count, std::vector<struct pollfd>& pfds) //use reinterpret_cast here to convert the pointer back to vector + use unique_pointer for every socket construct
{
	struct pollfd li{newfd, POLLIN, -1};
	pfds.push_back(li);
	++fd_count;
}

void remove_from_pfds(int pos, int &fd_count, std::vector<struct pollfd>& pfds)
{
	pfds.erase(pfds.begin() + pos);
	--fd_count;
}

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
	int listener{-1}, newfd{-1}, fd_count{}, pollcount{};

	struct sockaddr_storage remoteaddr;
	socklen_t addrlen;

	char buf[MAXDATASIZE], remoteIP[INET6_ADDRSTRLEN];

	listener = get_listener_socket();

	if(listener == -1)
	{
		std::cerr << "error on listening. \n";
		return -1;
	}
	fd_count++;
	std::vector<struct pollfd> pfds{};
	//pfds.reserve(BACKLOG);
	
	struct pollfd li{listener, POLLIN, -1};
	pfds.push_back(li);
	
	while(1)
	{
		pollcount = poll(&pfds[0], fd_count, -1); 
		if(pollcount == -1)
		{
			std::cerr << "Polling fails!\n";
			return 1;
		}
		for(int i=0; i<fd_count; ++i)
		{
			if(pfds[i].revents & POLLIN)
			{
				if(pfds[i].fd == listener) // current file descriptor is of listener socket
				{
					addrlen = sizeof(remoteaddr);
					newfd   = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
					
					if(newfd == -1)
					{
						std::cerr << "Accept failed! Continuing...\n";
					}
					else
					{
						//add it to poll list
						add_to_pfds(newfd, fd_count, pfds);
						inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
						std::cout << "pollserver: new connection from: " << remoteIP << "\'" << fd_count << std::endl;
					}
				}
				else //current file descriptor is of remote connection
				{
					int nBytes = recv(pfds[i].fd, buf, MAXDATASIZE-1, 0);
					buf[nBytes] = '\0';
										
					if(nBytes <= 0) // connection is closed by client
					{
						if(nBytes == 0)
						{
							std::cerr << "pollserver: socket " << pfds[i].fd << "hung up\n";
						}
						else
						{
							std::cerr << "recv() failed!\n";
						}
						remove_from_pfds(i, fd_count, pfds);
						close(pfds[i].fd);
					}
					else
					{
						// send data to everyone except listener
						 
						for(int j=0; j<fd_count; ++j)
						{
							if(pfds[j].fd != listener && pfds[j].fd != pfds[i].fd)
							{
								if(send(pfds[j].fd, buf, nBytes, 0) == -1)
								{
									std::cerr << "send() failed!\n";
								}
								std::cout << "Sending to : " << pfds[j].fd << " ";
							}
						}
					}
				}
			}
			
		}
		
	}
	return 0;
}
