/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ablondel <ablondel@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/09 17:08:35 by ablondel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"

Webserv::Webserv()
{
    return ;
}

Webserv::Webserv(std::vector<ServerInfo> &s) : _servers(s)
{
    std::vector<int> tmp(s.size());
    _sockets = tmp;
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        log(GREEN, "SERVER NAME = ", it->getServerName());
        log(GREEN, "SERVER PORT = ", it->getPort());
        log(GREEN, "MAX CLIENT BODY = ", it->getClientMaxBodySize());
        log(GREEN, "ROOT = ", it->getServerRoot());
        log(GREEN, "INDEX = ", it->getServerIndex());
        std::vector<page> current_pages = it->getPages();
        _ports.push_back(it->getPort());
        //for (std::vector<page>::iterator it = current_pages.begin(); it != current_pages.end(); it++)
        //{
        //    log(GREEN, "location = ", it->location_path);
        //    for (std::vector<std::string>::iterator rit = it->methods.begin(); rit != it->methods.end(); rit++)
        //        log(GREEN, "ALLOWED METHOD = ", *rit);
        //    log(GREEN, "autoindex = ", it->autoindex);
        //    log(GREEN, "is CGI = ", it->is_cgi);
        //}
        log(RED, "----------------------------------------------------------", 0);
    }
    return ;
}

Webserv::~Webserv()
{
    return ;
}


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

void    Webserv::close_all(std::vector<int> &sockets)
{
    for (std::vector<int>::iterator it = sockets.begin(); it != sockets.end(); it++)
        close(*(it));    
}

int     Webserv::set_server(std::vector<struct sockaddr_in> &addrs)
{
    int on = 1;
    addrs.reserve(_ports.size());
    for (size_t i = 0; i != _ports.size(); i++)
    {
        if ((_sockets[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            close_all(_sockets);
            return -1;
        }
        if (fcntl(_sockets[i], F_SETFL, O_NONBLOCK) < 0)
        {
            close_all(_sockets);
            return -2;
        }
        if (setsockopt(_sockets[i], SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        {
            close_all(_sockets);
            return -3;
        }
        memset(&addrs[i], 0, sizeof(addrs[i]));
        addrs[i].sin_family = AF_INET;
        addrs[i].sin_addr.s_addr = inet_addr("127.0.0.1");
        addrs[i].sin_port = htons(_ports[i]);
        if ((bind(_sockets[i], (struct sockaddr *)&addrs[i], sizeof(addrs[i]))) < 0)
        {
            close_all(_sockets);
            return -4;
        }
        if ((listen(_sockets[i], BACKLOG) < 0))
        {
            close_all(_sockets);
            return -5;
        }
    }
    return 0;
}

void    Webserv::parse_request(std::string &request)
{
    size_t pos = 0;
    std::string res;
    if (!request.length())
        return ;
    while ((pos = request.find('\n')) != std::string::npos)
    {
        res = request.substr(0, pos);
        request.erase(0, pos + 1);
        std::cout << "|" << res << "|" << std::endl;
    }
}

int     Webserv::run_server(std::vector<struct sockaddr_in> &addrs)
{
    std::vector<int> clients;
    struct timeval timeout;
    char buffer[1024];
    int end_server;
    int close_conn;
    int rc;
    int max;
    
    /////////////////////////////////////DATA TESTS
    std::string ok = "HTTP/1.1 200\r\n\r\n";
    std::string index;
    std::ifstream ifs;
    ifs.open("./pages/website1/index.html", std::fstream::in);
    while(ifs.read(buffer, sizeof(buffer)))
        index.append(buffer, sizeof(buffer));
    index.append(buffer, ifs.gcount());
    ok.append(index);
    /////////////////////////////////////////
    
    end_server = false;
    rc = set_server(addrs);
    if (rc < 0)
        return -1;
    fd_set current_set;
    fd_set read_set;
    FD_ZERO(&current_set);
    max = _sockets.back();
    for (std::vector<int>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
        FD_SET(*it, &current_set);
    while (end_server == false)
    {
        FD_ZERO(&read_set);
        timeout.tv_usec = 0;
        timeout.tv_sec = 3 * 60;
        read_set = current_set;
        rc = select(max + 1, &read_set, NULL, NULL, &timeout);
        if (rc < 0)
        {
            close_all(_sockets);
            end_server = true;
            break ;
        }
        if (rc == 0)
        {
            close_all(_sockets);
            end_server = true;
            break ;
        }
        int rd = 0;
        int rw = 0;
        close_conn = false;
        for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); it++)
        {
            bzero(&buffer, sizeof(buffer)); /* Clear the buffer */
            rd = recv(*it, buffer, sizeof(buffer), 0);
            //std::string request(buffer);
            //log(RED, "request contains: ", request);
            //if (request.length() > 0)
            //    printf("\x1B[32m[[DATA RECEIVED]]\x1B[0m\n\n%s", request.c_str());
            //parse_request(request);
            //request.clear();
            if (rd < 0)
            {
                break ;
            }
            if (rd == 0)
            {
                break ;
            }
            rw = send(*it, ok.c_str(), ok.size(), 0);
            //log(GREEN, "Bytes sent: ", rw);
            if (rw < 0)
            {
                break ;
            }
            if (rw == 0)
            {
                break ;
            }
            close(*it);
            FD_CLR(*it, &current_set);
            clients.erase(it);
        }
        for (int i = 0; i < max; i++)
        {
            if (FD_ISSET(_sockets[i], &read_set))
            {
                socklen_t len = sizeof(sockaddr[i]);
                int connection = accept(_sockets[i], (struct sockaddr*)&addrs[i], &len);
                if (connection < 0)
                {
                    close_all(_sockets);
                    end_server = true;
                }
                FD_SET(connection, &current_set);
                clients.push_back(connection);
                if (connection > max)
                    max = connection;
                break ;
            }
        }
    }
    close_all(_sockets);
    (void)close_conn;
    return 0;
}
std::vector<ServerInfo>&     Webserv::getServers()       {   return _servers;    };
std::vector<int>&            Webserv::getPorts()        {   return _ports;  };
