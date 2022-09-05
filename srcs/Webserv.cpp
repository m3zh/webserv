/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/05 15:21:33 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */

# include "../inc/Webserv.hpp"

std::vector<ServerInfo>&     Webserv::getServers()       {   return _servers;  };
std::vector<int>&            Webserv::getWbsrvPorts()        {   return _ports;  };

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
        log(RED, "----------------------------------------------------------", 0);
    }
    _size = _ports.size();
    _sockets.resize(_size);
    _addrs.resize(_size);
    //for (std::vector<page>::iterator it = current_pages.begin(); it != current_pages.end(); it++)
    //{
    //    log(GREEN, "location = ", it->location_path);
    //    for (std::vector<std::string>::iterator rit = it->methods.begin(); rit != it->methods.end(); rit++)
    //        log(GREEN, "ALLOWED METHOD = ", *rit);
    //    log(GREEN, "autoindex = ", it->autoindex);
    //    log(GREEN, "is CGI = ", it->is_cgi);
    //}
    return ;
}

Webserv::~Webserv()
{
    return ;
}

void    Webserv::close_all()
{
    for (std::vector<int>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
        close(*(it));
}

int     Webserv::set_server()
{
    int on = 1;
    for (size_t i = 0; i != _ports.size(); i++)
    {
        if ((_sockets[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            close_all();
            return -1;
        }
        if (fcntl(_sockets[i], F_SETFL, O_NONBLOCK) < 0)
        {
            close_all();
            return -2;
        }
        if (setsockopt(_sockets[i], SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        {
            close_all();
            return -3;
        }
        memset(&_addrs[i], 0, sizeof(_addrs[i]));
        _addrs[i].sin_family = AF_INET;
        _addrs[i].sin_addr.s_addr = inet_addr("127.0.0.1");
        _addrs[i].sin_port = htons(_ports[i]);
        if ((bind(_sockets[i], (struct sockaddr *)&_addrs[i], sizeof(_addrs[i]))) < 0)
        {
            close_all();
            return -4;
        }
        if ((listen(_sockets[i], BACKLOG) < 0))
        {
            close_all();
            return -5;
        }
    }
    return 0;
}



void    Webserv::accept_clients()
{
    //std::cout << "_sockets size is : " << _sockets.size() << " and _max is " << _max << std::endl;
    for (int i = 0; i < _max; i++)
    {
        if ((unsigned long)i < _sockets.size() && FD_ISSET(_sockets[i], &_read_set)) // this line creates a SEGFAULT in linux
        {
            socklen_t len = sizeof(sockaddr[i]);
            _connection = accept(_sockets[i], (struct sockaddr*)&_addrs[i], &len);
            if (_connection < 0)
                close_all();
            FD_SET(_connection, &_current_set);
            _clients.push_back(_connection);
            if (_connection >= _max)
                _max = _connection;
            break ; // as soon as a client is pushed back, we break out of the for loop
        }
    }
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

void    Webserv::transmit_data()
{
    int rd = 0;
    (void)rd; //why ?
    int rw = 0;
    char buffer[1024];
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
    for (std::vector<int>::iterator it = _clients.begin(); it != _clients.end(); it++) // Is it possible to have more than 1 inside of this vector ?
    {
        bzero(&buffer, sizeof(buffer)); /* Clear the buffer */
        rd = recv(*it, buffer, sizeof(buffer), 0);
        if (rd <= 0)
            break;

///////////////////////////////////////////////////////////////////////

        std::cout << "Request from " << *it << " : \n---------------------------\n" << buffer << "-----------------------" << std::endl;


        Request req(buffer); // instanciate a Request class with the raw request as a constructor parameter

        std::cout << "Data recovered from the request : \n";
        std::cout << "method : " << req.get_method() << std::endl;
        std::cout << "location : " << req.get_location() << std::endl;
        std::cout << "http version : " << req.get_http_version() << std::endl;
        std::cout << "\nDisplaying header map : \n";
        for (std::map<std::string, std::string>::const_iterator it = req.get_header_map().begin(); it != req.get_header_map().end(); ++it)
        {
            std::cout << (*it).first << ": " << (*it).second << std::endl;
        }
        if (req.get_index_beginning_body() != std::string::npos) // it means there is a body
        {
            std::cout << "\nDisplaying the request body :\n";
            std::cout << req.get_body() << std::endl;
        }
        std::cout << "\n\n";

//////////////////////////////////////////////////////////////////////////

        rw = send(*it, ok.c_str(), ok.size(), 0);
        if (rw <= 0)
            break;
        close(*it);
        FD_CLR(*it, &_current_set);
        //_clients.erase(it);
    }
    _clients.clear();
}

int     Webserv::run_server()
{
    struct timeval timeout;
    //int end_server;
    int rc;
    
    //end_server = false;
    rc = set_server();
    if (rc < 0)
        return -1;
    FD_ZERO(&_current_set);
    _max = _sockets.back();
    for (std::vector<int>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
        FD_SET(*it, &_current_set);
    while (/*end_server == false*/ true)
    {
        FD_ZERO(&_read_set);
        timeout.tv_usec = 0;
        timeout.tv_sec = 3 * 60;
        _read_set = _current_set;
 //       accept_clients(); doesnt work if placed here
        rc = select(_max + 1, &_read_set, NULL, NULL, &timeout);
        if (rc <= 0) // negative is select() error and 0 is select() timeout
            break;
        transmit_data();
        accept_clients();
    }
    close_all();
    return 0;
}