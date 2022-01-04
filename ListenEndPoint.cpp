#include "ListenEndPoint.hpp"

#include <iostream>
#include <algorithm>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> // struct sockadd_in
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#define QUEUE_MAX_SIZE 10

// return 0 upon success, -1 if error
int ListenEndPoint::get_listen_socket()
{
    // socket() syscall returns a file descriptor
    listen_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1)
    {
        perror("socket");
        return (-1); // change these return statements into thrown exceptions ?
    }

	// initialize max_fd
	max_fd = listen_socket;
	
    // enable socket to reuse address (avoid "Address already in use" error)
    int yes = 1;
	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        perror("setsockopt");
        return (-1);
    }

    // make fd non-blocking (fcntl)
    if (fcntl(listen_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        perror("fcntl");
        return(-1);
    }

    // set up address (host_ip:port) for the bind() call
    sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host_IP.c_str());

    // bind a name (specific address) to socket
	if (bind(listen_socket, reinterpret_cast<struct sockaddr *>(&address),
		 		sizeof(address)) == -1)
	{
		perror("bind");
		return (-1);
	}

    // finally, make it a listening socket
	if (listen(listen_socket, QUEUE_MAX_SIZE) == -1)
    {
        perror("listen");
        return (-1);
    }

	// output a confimation message to the terminal
	std::cout   << "Listening on " << host_IP << ":" << port << "...\n"
                << std::endl;

    return (0);
}

// return 0 upon success, -1 if error
int ListenEndPoint::accept_connection()
{
	int         new_socket;
	sockaddr    client_addr;
	socklen_t   addrlen = sizeof(client_addr);

    // accept new connection
    new_socket = accept(listen_socket, &client_addr, &addrlen);
    if (new_socket == -1)
    {
        perror("accept");
        return (-1);
    } // obs: a distinct return code could be set if errno is WOULDBLOCK

	// update max_fd
	if (new_socket > max_fd)
		max_fd = new_socket;

    // add to list of clients
    accepted_connections.push_back(new_socket);

	// log to terminal
    std::cout	<< "Connection accepted on " << host_IP << ":" << port
                << std::endl;
    
    // get info about the host and display
    char host[1024];
    char service[20];
    if (getnameinfo(&client_addr, addrlen, host, 1024, service, 20, NI_NOFQDN)
        != 0)
        std::cerr << "getnameinfo() returned an error" << std::endl;
    else
	{
        std::cout << "Service: " << service << " Host: " << host << "\n" << std::endl;
	}
	
	return (0);
}

// read from socket (receive raw request)
// returns 0 if connection closed, -1 if error, 1 if ok
int ListenEndPoint::receive_request(int fd, std::string &raw_request)
{
	// FOR NOW, assuming everything will be received at once and will fit
	// in the buffer
	const int BUF_SIZE = 4096;
	char buffer[BUF_SIZE];
	int	ret;

	ret = recv(fd, buffer, BUF_SIZE - 1, 0);
	if (ret < 0)
	{
		perror("recv");
		return (-1);
	}
	if (ret == 0) // connection was close by the client
		return (0);

	// terminate string with NULL character
	buffer[ret] = '\0';

	// log to terminal (for debugging porpouses)
	std::cout << "Received data from fds "<< fd << ":\n" << buffer << std::endl;

	// transform C-string into a cpp string
	raw_request = buffer;

	return (1);
}

// index of the accepted_connections vector
void ListenEndPoint::close_connection(int client_socket)
{
	// close file descriptor
	close(client_socket);

	// remove it from accepted_connections list
	for (std::vector<int>::iterator it = accepted_connections.begin();
			it != accepted_connections.end(); ++it)
	{
		if (*it == client_socket)
		{
			accepted_connections.erase(it);
			break;
		}
	}
	
	// update max_fd
	if (client_socket == max_fd)
	{
		max_fd = *std::max_element(accepted_connections.begin(),
				accepted_connections.end());
		if (listen_socket > max_fd) // that would be unlikely...
			max_fd = listen_socket;
	}
}

HttpRequest ListenEndPoint::parse_request(std::string &raw_request)
{
	// TODO
	// for now just, copying the raw request into the new Request object

	HttpRequest request;
	request.raw_request = raw_request;
	return (request);
}

Vserver * ListenEndPoint::resolve_vserver(HttpRequest &request)
{
	// TODO

	// for now, just return the first vserver of the list
	(void)request;
	return (&vservers.front());
}
