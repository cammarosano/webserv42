#include "Vserver.hpp"
#include <sys/types.h>
#include <sys/socket.h>

// little helper function to convert a number to text
#include <sstream>
std::string NumberToString (size_t nb )
{
	std::ostringstream ss;
	ss << nb;
	return ss.str();
}

void Vserver::respond(int client_socket, HttpRequest & request)
{
	// TODO
	// for now, a standard response

	std::string body =
	"<!DOCTYPE html><html><body>"
	"<h1>A proper response to your request is a big TODO on our list!</h1>"
	"<p>By the way, your request was: </p> <p>" + request.raw_request + "</p>"
	"</body></html>";

	std::string header =	"HTTP/1.1 200 OK\n"
							"Content-Type: text/html\n"
							"Content-Lenght: "
							+ NumberToString(body.length()) + "\n";

	std::string msg = header + "\n" + body;

	// for now, assuming all data will be sent in one single call to send()
	send(client_socket, msg.c_str(), msg.length(), 0);

	// should I close the connection?
	
}
