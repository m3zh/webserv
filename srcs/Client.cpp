/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:30:29 by artmende          #+#    #+#             */
/*   Updated: 2022/09/13 19:30:58 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client( int client_socket, 
                struct sockaddr_in client_addrs,
                ServerInfo *associated_server)  :   client_socket(client_socket),
                                                    client_addrs(client_addrs),
                                                    associated_server(associated_server),
                                                    request(request_str)                        {
                                                                                                        setReadAsComplete(false);
                                                                                                        setHeaderReadAsComplete(false); 
                                                                                                };
Client::~Client()                                                                               {};

void    Client::parseHeader() const             {   getRequest().parse_raw_request();  };
void    Client::handleRequest()                 {   // performing some action here ! Now we have the full request and we should execute it and generate a response
                                                is_read_complete = true;
                                                };

// GETTERS
int                     Client::getClientSocket()       const   {   return  client_socket;      };
struct sockaddr_in      Client::getClientAddress()      const   {   return  client_addrs;       };
ServerInfo *            Client::getServerInfo()         const   {   return  associated_server;  };
std::string             Client::getRequestString()      const   {   return  request_str;        };
Request const&          Client::getRequest()            const   {   return  request;            };

// BOOLS
bool                    Client::isReadComplete()                        {   return  is_read_complete;            };
void                    Client::setReadAsComplete(bool state)           {   is_read_complete = state;            };
bool                    Client::headerIsReadComplete()                  {   return  header_is_read_complete;     };
void                    Client::setHeaderReadAsComplete(bool state)     {   header_is_read_complete = state;     };
