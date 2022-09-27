/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:30:29 by artmende          #+#    #+#             */
/*   Updated: 2022/09/27 18:18:59 by mlazzare         ###   ########.fr       */
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
std::string             Client::getResponseString()                     const   {   return  response_str;       };
std::string             Client::getResponseFile()                       const   {   return  response_file;      };

// SETTERS
void                    Client::setRequest(std::string const &s)                {   request = Request(s); request.parse_raw_request();       };
void                    Client::setRequestString(std::string s)                 {   request_str.append(s);      };
// sets the right header for the response and set the right file to open ( as requested by Client )
void                    Client::setResponseString(std::string code, std::string msg)
{
    std::string file = getenv("PWD");
    std::map<std::string, std::string> status;
    status["200"] = "OK";
    status["301"] = "Moved Permanently";
    status["400"] = "Bad Request";
    status["401"] = "Unauthorized";
    status["403"] = "Forbidden";
    status["404"] = "Not Found";
    status["405"] = "Method Not Allowed";
    status["414"] = "Request URI Too Long";
    status["501"] = "Not Implemented";
    status["504"] = "Gateway Timeout";
    status["505"] = "HTTP Version Not Supported";
    
    response_str = "HTTP/1.1 " + code + " " + status[code];
    if ( code == "301" )
        response_str += "\nLocation: " + msg;
    else if ( code > "301" )
        file += ERROR_FILE_PATH + "HTTP" + code + ".html";
    else if ( code == "200" )
        file += msg;
    response_str += "\r\n\r\n";
    std::cout << "FILE:@@@ " << file << std::endl;
    setResponseFile(file);    
        
};
void                    Client::setResponseFile(std::string file)       {   response_file = file;       };

// BOOLS
bool                    Client::isReadComplete()                        {   return  is_read_complete;            };
void                    Client::setReadAsComplete(bool state)           {   is_read_complete = state;            };
bool                    Client::headerIsReadComplete()                  {   return  header_is_read_complete;     };
void                    Client::setHeaderReadAsComplete(bool state)     {   header_is_read_complete = state;     };
