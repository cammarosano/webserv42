#ifndef VSERVER_HPP
# define VSERVER_HPP

# include <string>
# include <vector>
# include "Route.hpp"
# include "HttpRequest.hpp"

// multiple virtual servers could be listening on the same IP:port.
// virtual server resolution is made based on the server_name, and there
// will always be a default virtual server for that IP:port.
// See: https://nginx.org/en/docs/http/request_processing.html
class Vserver
{
public:
	std::string	host_IP;		// ex: 127.0.0.1
	unsigned short int port;	// 16 bits: 0 - 65535
	std::string server_name;	// ex: www.servername.com
	std::string root;			// ex: /data/www
	std::vector<Route>	routes;	// a list of routes

	void respond(int client_socket, HttpRequest & request); // TODO
};

#endif

// obs: idea for storing IP address (int_addr unfriendly, though)
	// unsigned char host_IP[4];	// 4 bytes, ex: 127.0.0.1

