/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:29:14 by artmende          #+#    #+#             */
/*   Updated: 2022/09/28 13:25:04 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <netinet/in.h>
#include "Request.hpp"
#include "Response.hpp"
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

        std::string                 request_str;
        Request                     request;

        std::string                 response_str;
        std::string                 response_file;
        // Response                    response;

        bool                        header_is_read_complete;
        bool                        is_read_complete;       

        Client();
        Client(Client const & x);
        Client &    operator=(Client const & x);

    public:
        Client( int client_socket, struct sockaddr_in client_addrs, ServerInfo *associated_server );
        ~Client();

        void                    parseHeader();

        // GETTERS
        int                     getClientSocket()   const;
        struct sockaddr_in      getClientAddress()  const;
        ServerInfo *            getServerInfo()     const;
        std::string             getRequestString()  const;
        Request                 getRequest()        const;
        std::string             getResponseString() const;
        std::string             getResponseFile()   const;

        // SETTERS
        void                    setRequest(std::string const &s);
        void                    setRequestString(std::string s);
        // code is response code
        // msg is page URI as requested, or redirection, or "" if code is error code
        // root is path to website root folder ( as in config )
        void                    setResponseString(std::string code, std::string location, std::string root);
        void                    setResponseFile(std::string file);

        // BOOLS
        bool                    isReadComplete();
        void                    setReadAsComplete(bool state);
        bool                    headerIsReadComplete();
        void                    setHeaderReadAsComplete(bool state);

        //Response class
};
