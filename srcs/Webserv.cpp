/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/05 08:44:55 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"

Webserv::Webserv(std::vector<ServerInfo> &s)  : _servers(s) 
                                                {}
Webserv::~Webserv()                             {}

std::vector<ServerInfo>&     Webserv::getServers()       {   return _servers;    };


// probably to refactor
bool        Webserv::isCGI_request(std::string html_content)
{
    std::string root = "/home/user42/webserv/cgi-bin/";                 // hardcoded here; this should be retrieved from ServerInfo > page > root
    size_t pos = html_content.find("action");
    if (pos == std::string::npos)
        {   std::cout << "Invalid action for CGI\n"; return false;              };
    pos += 7;                                                           // action="........", we want to start from the first \", ie pos + 7
    std::string action;
    while ( html_content[++pos] != '\0'
        && html_content[pos] != '\"')
        action += html_content[pos];
    size_t extension = action.size() - 3;
    if (action.compare(extension, action.size(), ".py")                 // check if it's a pyhton or perl script [ is this necessary ? ]
        && action.compare(extension, action.size(), ".pl"))
        {   std::cout << "Invalid file extension for CGI\n"; return false;      };
    pos = html_content.find("method");
    if (pos == std::string::npos)
        {   std::cout << "Invalid method for CGI\n"; return false;              };
    pos += 7;                                                           // method="........", we want to start from the first \", ie pos + 7
    std::string method;
    while ( html_content[++pos] != '\0'
        && html_content[pos] != '\"')
        method += html_content[pos];
    if (method.compare("get") != 0                                      // only method get and post are accepted for cgi
        && method.compare("post") != 0)
        {   std::cout << "Invalid method for CGI\n"; return false;              };
    root += action;
    if (access(root.c_str(), X_OK) < 0)                                 // if executable exist and it's executable
        {   std::cout << "File not executable by CGI\n"; return false;          };
    return true;
}