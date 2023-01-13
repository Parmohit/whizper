#include <sys/socket.h>
#include <exception>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		std::cerr << "Follow format: showip hostname\n";
		return 1;
	}

	try
	{
		int status{};
		char ipstr[INET6_ADDRSTRLEN];
		struct addrinfo hints, *p{nullptr};
		memset(&hints, 0, sizeof(hints));

		hints.ai_family   = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		
		if((status=getaddrinfo(argv[1], NULL, &hints, &p)) !=0)
		{
			std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
			return 2;
		}		
		
		std::cout << "IP address of " << argv[1] << std::endl;

		for(; p!=NULL; p=p->ai_next)
		{
			void *addr;
			char *ipver;
			if(p->ai_family == AF_INET) //IPv4
			{
				struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;	
				addr = &(ipv4->sin_addr);
				ipver = (char*)"IPv4";
			}
			else //IPv6
			{
				struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)p->ai_addr;
				addr = &(ipv6->sin6_addr);
				ipver = (char*)"IPv6";
			}

			inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
			std::cout << ipver << " " << ipstr << std::endl;

		}

		freeaddrinfo(p);
	} catch(const std::exception& e)
	{
		std::cerr << "Exception raised: " << e.what() << std::endl;
		return -1;
	}
}
