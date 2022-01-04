#ifndef LISTENENDPOINT_HPP
# define LISTENENDPOINT_HPP

# include <vector>
# include <string>
# include "Vserver.hpp"
# include "HttpRequest.hpp"
# include "OutboundResponse.hpp"

// each unique IP:port combination is a ListenEndPoint

class ListenEndPoint
{    
public:
    std::string host_IP;	// or an unsigned char [4] ?
	unsigned short int port;
	int	listen_socket;	// a file descriptor
	int	max_fd;			// keeping track of the biggest fd number (important for select())

	std::vector<Vserver> vservers;	// list of Vservers
	std::vector<int>	accepted_connections; // list of client fds (see obs)
	std::vector<OutboundResponse> outbound_responses; // list of responses to be done, waiting for socket to be ready to write to

    int get_listen_socket();
    int accept_connection(); // accept connection and add fd to list of accepted connections
	void close_connection(int client_socket);
    int receive_request(int fd, std::string &raw_request);
	HttpRequest parse_request(std::string &raw_request); // TODO
    Vserver * resolve_vserver(HttpRequest &request);	// TODO
};


#endif

// obs: maybe use a class AcceptedConnection, instead of just the fd, to hold more
// info about the client (like host, service...)

// super-obs: accepted_connections and outbound_reponses will suffer lots of insertions
// and deletions. A linked-listed is a much better suited data-structure for this.
// a set would be great for accepted_connections

