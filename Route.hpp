#ifndef ROUTE_HPP
# define ROUTE_HPP

struct Route
{
    std::string location;   // ex: /static (the uri)
    std::string root;       // ex: /data/www 

    bool        auto_index;  // turn on or off directory listing
    std::string index;      // default file to answer if the request is a directory

    std::string cgi_extension; // which extension should trigger cgi
    std::string cgi_script; // which program will be execd as cgi

    bool        accept_upload;  // accept or not upload in this route
    std::string upload_dir;     // where to store uploaded files
};



#endif