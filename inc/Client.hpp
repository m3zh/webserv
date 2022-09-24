/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:29:14 by artmende          #+#    #+#             */
/*   Updated: 2022/09/24 18:25:55 by mlazzare         ###   ########.fr       */
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

        int                         client_socket;
        struct sockaddr_in          client_addrs;

        ServerInfo *                associated_server;

        std::string                 request_str; // std::string can contain \0
        Request                     request;

        bool                        header_is_read_complete;
        bool                        is_read_complete;

        Client();
        Client(Client const & x);
        Client &    operator=(Client const & x);

    public:
        Client( int client_socket, struct sockaddr_in client_addrs, ServerInfo *associated_server );
        ~Client();

        void                    parseHeader();
        void                    handleRequest();

        // GETTERS
        int                     getClientSocket()   const;
        struct sockaddr_in      getClientAddress()  const;
        ServerInfo *            getServerInfo()     const;
        std::string             getRequestString()  const;
        Request&                getRequest()        ;

        // SETTERS
        void                    setRequest(std::string const &s);
        void                    setRequestString(std::string s);

        // BOOLS
        bool                    isReadComplete();
        void                    setReadAsComplete(bool state);
        bool                    headerIsReadComplete();
        void                    setHeaderReadAsComplete(bool state);

        //Response class
};
