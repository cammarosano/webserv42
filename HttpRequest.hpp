#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>

class HttpRequest
{
public:
    
	// request line
	std::string	method; // verb (GET, POST, etc)
	std::string	uri;	// ex: /docs/test.html
	std::string	protocol;

	// request headers
	std::string	host;
	// etc...

    // body
	std::string body;

	// for debugging porpouses:
	std::string raw_request;
};

// obs: stuff like method and protocol could use an enum(ints) instead of strings

#endif