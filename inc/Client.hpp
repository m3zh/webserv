/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:29:14 by artmende          #+#    #+#             */
/*   Updated: 2022/09/20 15:06:44 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <netinet/in.h>
#include "Request.hpp"

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
    Client(int client_socket, struct sockaddr_in client_addrs, int listening_socket, struct sockaddr_in const & listening_addrs)
        : client_socket(client_socket), client_addrs(client_addrs), listening_socket(listening_socket),
        listening_addrs(listening_addrs), request_class(request_str), has_header_been_read(false), is_read_complete(false)
    {}

    ~Client() {}

    void    parse_request()
    {
        this->request_class.parse_raw_request();
    }

    int                         client_socket;
    struct sockaddr_in          client_addrs;
    const int                   listening_socket;
    struct sockaddr_in const &  listening_addrs;

    std::string                 request_str; // std::string can contain \0
    Request                     request_class;

    bool                        has_header_been_read;
    bool                        is_read_complete;
};
