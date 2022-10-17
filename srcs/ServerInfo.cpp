/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/03 09:29:52 by mlazzare          #+#    #+#             */
/*   Updated: 2022/10/16 21:10:48 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/ServerInfo.hpp"

ServerInfo::ServerInfo()    :   _server_name(""),                               
                                _port(-1),                                      // port MUST be set in config
                                _client_max_body_size(1048576)                   // by default set to 1M
                                {};
ServerInfo::~ServerInfo()       {};

std::string         ServerInfo::getServerName()                 {   return _server_name;          };
void                ServerInfo::setServerName(std::string s)    {   _server_name = s;             };
std::string         ServerInfo::getServerRoot()                 {   return _server_root;          };
void                ServerInfo::setServerRoot(std::string s)    {   _server_root = s;             };
std::string         ServerInfo::getServerIndex()                {   return _server_index;         };             
void                ServerInfo::setServerIndex(std::string s)   {   _server_index = s;            };
int                 ServerInfo::getPort()                       {   return _port;                 };
void                ServerInfo::setPort(int p)                  {   _port = p;                    };
int                 ServerInfo::getClientMaxBodySize()          {   return _client_max_body_size; };
void                ServerInfo::setClientMaxBodySize(int c)     {   _client_max_body_size = c;    };
std::vector<page>&  ServerInfo::getPages()                      {   return _pages;                };
void                ServerInfo::setPages(page p)                {   _pages.push_back(p);          };
int                 ServerInfo::getListeningSocket() const      {   return _listening_socket;     };
void                ServerInfo::setListeningSocket(int socket)  {   _listening_socket = socket;   };
struct sockaddr_in const &  ServerInfo::getListeningAddrs() const                   {   return _listening_addrs;        };
void                        ServerInfo::setListeningAddrs(struct sockaddr_in addrs) {   _listening_addrs = addrs;       };




