/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:29:14 by artmende          #+#    #+#             */
/*   Updated: 2022/10/05 10:48:24 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <string>
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

        std::string                 response_str; // header of the response
        std::ifstream               response_file_stream; // stream open on the file to send (if there is one)
        std::string                 response_file;
        std::string                 remaining_buffer_to_send; // In case send() couldn't transmit the full buffer
        bool                        there_is_a_file_to_send;
        bool                        header_has_been_sent;

        bool                        header_is_read_complete;
        bool                        is_read_complete;       
        bool                        FileToSend;       

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
        std::ifstream &         getResponseFileStream(); // probably not const as reading will affect the state of the stream
        std::string &           getRemainingBufferToSend();

        // SETTERS
        void                    setRequest(std::string const &s);
        void                    setRequestString(std::string s);
        // code is response code
        // msg is page URI as requested, or redirection, or "" if code is error code
        // root is path to website root folder ( as in config )
        void                    setResponseString(std::string code, std::string content, std::string root);
        void                    setResponseFile(std::string file);

        // BOOLS
        bool                    isReadComplete();
        void                    setReadAsComplete(bool state);
        bool                    headerIsReadComplete();
        void                    setHeaderReadAsComplete(bool state);
        bool                    noFileToSend();
        void                    setNoFileToSend(bool state);
        bool                    headerHasBeenSent();
        void                    setHeaderBeenSent(bool state);
        bool                    thereIsAFileToSend();
        void                    setThereIsAFileToSend(bool state);


        //Response class
};
