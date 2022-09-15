/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:29:14 by artmende          #+#    #+#             */
/*   Updated: 2022/09/15 17:47:53 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <netinet/in.h>

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
    : client_socket(client_socket), client_addrs(client_addrs), listening_socket(listening_socket), listening_addrs(listening_addrs)
    {}

    ~Client() {}

    int                         client_socket;
    struct sockaddr_in          client_addrs;
    int const                 listening_socket;
    struct sockaddr_in const &  listening_addrs;
};
