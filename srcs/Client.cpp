/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:30:29 by artmende          #+#    #+#             */
/*   Updated: 2022/09/26 16:26:21 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client( int client_socket, 
                struct sockaddr_in client_addrs,
                ServerInfo *associated_server)  :   client_socket(client_socket),
                                                    client_addrs(client_addrs),
                                                    associated_server(associated_server),
                                                    request_str("")                            {
                                                                                                        setReadAsComplete(false);
                                                                                                        setHeaderReadAsComplete(false); 
                                                                                                };
Client::~Client()                                                                               {};

// GETTERS
int                     Client::getClientSocket()                       const   {   return  client_socket;      };
struct sockaddr_in      Client::getClientAddress()                      const   {   return  client_addrs;       };
ServerInfo *            Client::getServerInfo()                         const   {   return  associated_server;  };
std::string             Client::getRequestString()                      const   {   return  request_str;        };
Request                 Client::getRequest()                            const   {   return  request;            };

// SETTERS
void                    Client::setRequest(std::string const &s)                {   request = Request(s); request.parse_raw_request();       };
void                    Client::setRequestString(std::string s)                 {   request_str.append(s);      };

// BOOLS
bool                    Client::isReadComplete()                        {   return  is_read_complete;            };
void                    Client::setReadAsComplete(bool state)           {   is_read_complete = state;            };
bool                    Client::headerIsReadComplete()                  {   return  header_is_read_complete;     };
void                    Client::setHeaderReadAsComplete(bool state)     {   header_is_read_complete = state;     };
