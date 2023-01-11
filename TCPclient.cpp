// NOTE: Check out below link for detailed explanation 
// https://pubs.opengroup.org/onlinepubs/009604499/

#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include <iostream>
#include <exception>
//#include <unp.h>

int main(int argc, char **argv)
{
	try
	{
		// Create a TCP socket
	
		// SOCKET(domain, type, protocol) : create an endpoint for communication 
		// protocol=0, means choose default
		int socketfd = socket(AF_INET, SOCK_STREAM, 0);

		if(socketfd < 0)
		{
			std::cerr << "Could'nt create endpoint!\n";
			return -1;
		}
	
		// Specify server's IP address and port
		// Establish connection with server
		// Read and display server's reply
		// Terminate program
	}
	catch(const std::exception& exception)
	{
		std::cerr << "Exception raised: "<< exception.what() << std::endl;
	}
	
	return 0;
}
