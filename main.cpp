#include "Vserver.hpp"
#include "ListenEndPoint.hpp"
#include "utils.hpp"
#include <vector>
#include <cstdio> // perror

int setup(int argc, char **argv, std::vector<ListenEndPoint> & listen_end_points)
{
    // IT SHOULD: parse config file into a list of Vservers

    // for now: get TWO vserver for testing
    (void)argc;
    (void)argv;
    Vserver vserver0;
    Vserver vserver1;

    vserver0.host_IP = "127.0.0.1";
    vserver0.port = 8080;

    vserver1.host_IP = "127.0.0.1";
    vserver1.port = 8081;

    
    // IT SHOULD: build list of ListenEndPoints out of the list of Vservers
    // (each ListenEndPoint contains a list a vservers)

    // for now: get one ListenEndPoint for each vserver we have
    listen_end_points.push_back(ListenEndPoint());
    listen_end_points.push_back(ListenEndPoint());

    listen_end_points[0].host_IP = vserver0.host_IP;
    listen_end_points[0].port = vserver0.port;
    listen_end_points[0].vservers.push_back(vserver0);

    listen_end_points[1].host_IP = vserver1.host_IP;
    listen_end_points[1].port = vserver1.port;
    listen_end_points[1].vservers.push_back(vserver1);

    // get a listening socket for each listen_end_point
    for (size_t i = 0; i < listen_end_points.size(); i++)
    {
        int ret = listen_end_points[i].get_listen_socket();
        if (ret == -1) // error (close already opened sockets)
            return (-1);
    }
    return (0);
}

// prepare fd_sets for select() call
void setup_select_sets(std::vector<ListenEndPoint> & listen_end_points,
                        fd_set & read_fds, fd_set & write_fds)
{
   // reset fd_sets for select()
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    // for each listening endpoint
    for (size_t i = 0; i < listen_end_points.size(); i++)
    {
        ListenEndPoint &lep = listen_end_points[i];

        // add its listen_socket to read_set
        FD_SET(lep.listen_socket, &read_fds);

        // add its accepted connections fds to read_set
        for (size_t j = 0; j < lep.accepted_connections.size(); j++)
            FD_SET(lep.accepted_connections[j], &read_fds);

        // add its outbound connections fds to write_set
        for (size_t j = 0; j < lep.outbound_responses.size(); j++)
            FD_SET(lep.outbound_responses[j].client_socket, &write_fds);
    }
}

int main(int argc, char **argv)
{
    std::vector<ListenEndPoint> listen_end_points;
    fd_set read_fds, write_fds;

    // parse config file and build list of listen_end_points
    setup(argc, argv, listen_end_points);

    while (1)
    {
        // prepare fd_sets for select() call
        setup_select_sets(listen_end_points, read_fds, write_fds);
     
        // call select()
        int fd_count = select(max_fd(listen_end_points) + 1,
                                &read_fds, &write_fds, NULL, NULL);
        if (fd_count == -1)
        {
            perror("select");
            return (1);
        }

        // for each listen_end_point:
        for (size_t i = 0; i < listen_end_points.size(); i++)
        {
            ListenEndPoint & lep = listen_end_points[i];

            // check if listen_fd is ready to read (ready to accept connections)
            if (FD_ISSET(lep.listen_socket, &read_fds))
            {
                // accept new connection
                // (or multiple new connectionS, until we get a WOULDBLOCK error?)
                lep.accept_connection(); // TODO: error checking
            }
            
            // for each fd in list of accepted connections:
            for (int i = 0; i < static_cast<int>(lep.accepted_connections.size()); i++)
            {
                int fd = lep.accepted_connections[i];

                // check if fd is ready to read
                if (FD_ISSET(fd, &read_fds))
                {
                    // receive request
                    std::string raw_request;
                    int ret = lep.receive_request(fd, raw_request);
                    if (ret == 0) // connection closed by the client
                    {
                        lep.close_connection(fd);
                        FD_CLR(fd, &read_fds); // not sure this is needed
                        --i; // because close_connection removes 1 elem from the vector
                    }
                    // TODO: error checking (ret == -1)

                    // parse raw request to a Request object
                    HttpRequest request = lep.parse_request(raw_request);

                    // create and OutboundResponse
                    OutboundResponse obresp;

                    obresp.client_socket = fd;
                    obresp.request = request;

                    // resolve virtual server
                    obresp.vserver = lep.resolve_vserver(request);

                    // add it to list of OutboundResponses
                    lep.outbound_responses.push_back(obresp);
                }
            }
            
            // for each outbound_response in list of OutboundResponses:
            for (int i = 0; i < static_cast<int>(lep.outbound_responses.size()); i++)
            {
                OutboundResponse obresp = lep.outbound_responses[i];

                // check if client_socket is ready to WRITE
                if (FD_ISSET(obresp.client_socket, &write_fds))
                {
                    // process request and send response
                    obresp.vserver->respond(obresp.client_socket, obresp.request);

                    // close connection (fd is removed from accepted_connections as well)
                    // should we close the connection?? What about http1.1, keep-alive... ?
                    lep.close_connection(obresp.client_socket);
                    
                    // remove obresp from the list OutboundResponses
                    lep.outbound_responses.erase(lep.outbound_responses.begin() + i);
                    --i; // as the present element was erased from the vector and elems to its right were shifted left
                }
            }

        } // listen_end_points loop

    } // while loop

}

// Obs:
// TODO: make use of fd_count to stop the loop earlier

// A better order for the loop would be:
// check for write (as accepted_connections might be removed as reponse is sent)
// check for read in accepted connections
// check for incoming connections (listen_fd), as this might make the accepted connections grow.