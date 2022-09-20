/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:29:14 by artmende          #+#    #+#             */
/*   Updated: 2022/09/20 17:24:30 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <netinet/in.h>
#include "Request.hpp"
#include "ServerInfo.hpp"

class Client
{
private:

// what we need :
// listening socket for this client (const ref is ok)
// listening addr for this client (const ref is ok)
// data socket
// data socket addr

    Client();
    Client(Client const & x);
    Client &    operator=(Client const & x);
public:
    Client(int client_socket, struct sockaddr_in client_addrs, ServerInfo const & associated_server)
        : client_socket(client_socket), client_addrs(client_addrs), associated_server(associated_server),
        request_class(request_str), has_header_been_read(false), is_read_complete(false)
    {}

    ~Client() {}

    void    parse_request()
    {
        this->request_class.parse_raw_request();
    }

    void    mark_read_as_complete()
    {
        // performing some action here ! Now we have the full request and we should execute it and generate a response
        this->is_read_complete = true;
    }

    int                         client_socket;
    struct sockaddr_in          client_addrs;

    ServerInfo const &          associated_server;

    std::string                 request_str; // std::string can contain \0
    Request                     request_class;

    bool                        has_header_been_read;
    bool                        is_read_complete;

    //Response class
};
