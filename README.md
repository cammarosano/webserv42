# webserv42

Comments in main.cpp should be interpreted as a (proposed) high-level pseudo-code description of the program.

The actual code, at this point, is not so important, as there's a lot to be improved.

Cryptic parts of the subject yet to be deciphered:
- "Your server should never block and the client should be bounce properly if necessary."
- "A request to your server should never hang forever."
- about CGI:
	- "Because you won’t call the CGI directly use the full path as PATH_INFO"
	- "Just remembers that for chunked request, your server needs to unchun-
ked it and the CGI will expect EOF as end of the body."
	- "Same things for the output of the CGI. if no content_length is returned
from the CGI, EOF will mean the end of the returned data."
	- "the cgi should be run in the correct directory for relativ path file access"

