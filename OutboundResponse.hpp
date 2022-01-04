#ifndef OUTBOUNDRESPONDE_HPP
# define OUTBOUNDRESPONDE_HPP

# include "HttpRequest.hpp"
# include "Vserver.hpp"

class OutboundResponse
{
public:
	HttpRequest	request;
	int			client_socket;	// accepted connection's fd
	Vserver *	vserver;		// pointer to vserver that must process the request
};

#endif