/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/03 09:29:52 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/03 11:31:02 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/ServerInfo.hpp"

ServerInfo::ServerInfo()    :   _server_name("localhost"),                      // by default set to localhost
                                _port(-1),                                      // port MUST be set in config
                                _client_max_body_size(500)                      // by default set to 500
                                {};
ServerInfo::~ServerInfo()       {};

std::string         ServerInfo::getServerName()                 {   return _server_name;          };
void                ServerInfo::setServerName(std::string s)    {   _server_name = s;             };
int                 ServerInfo::getPort()                       {   return _port;                 };
void                ServerInfo::setPort(int p)                  {   _port = p;                    };
int                 ServerInfo::getClientMaxBodySize()          {   return _client_max_body_size; };
void                ServerInfo::setClientMaxBodySize(int c)     {   _client_max_body_size = c;    };
std::vector<page>&  ServerInfo::getPages()                      {   return _pages;                };
void                ServerInfo::setPages(page p)                {   _pages.push_back(p);          };