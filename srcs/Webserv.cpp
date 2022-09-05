/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/31 16:31:11 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */

# include "../inc/Webserv.hpp"

bool keep_alive = true;

std::vector<ServerInfo>&     Webserv::getServers()       {   return _servers;  };
std::vector<int>&            Webserv::getWbsrvPorts()        {   return _ports;  };

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
    for (size_t i = 0; i < _sockets.size() && (int)i < _max; i++)
    {
        if (FD_ISSET(_sockets[i], &_read_set))
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



        std::cout << "Request from " << *it << " : \n---------------------------\n" << buffer << "-----------------------" << std::endl;
        //std::string request(buffer);
        //log(RED, "request contains: ", request);
        //if (request.length() > 0)
        //    printf("\x1B[32m[[DATA RECEIVED]]\x1B[0m\n\n%s", request.c_str());
        //parse_request(request);
        //request.clear();
        if (rd <= 0)
            break;
        Request req(buffer); // instanciate a Request class with the raw request as a constructor parameter

        std::cout << "\nData recovered :\nMethod : " << req._method << "\nLocation : " << req._location << "\nVersion : " << req._http_version << std::endl;

        std::cout << "header is : \n+++++++\n";// << req._header << "\n++++++" << std::endl;

        for (std::map<std::string, std::string>::iterator   it = req._header_map.begin(); it != req._header_map.end(); ++it)
        {
            std::cout << (*it).first << ": " << (*it).second << std::endl;
        }
        std::cout << "++++++++++++\n";

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
    signal(SIGINT, signal_handler);
    while (/*end_server == false*/ keep_alive)
    {
        std::cout << "in while\n";
        FD_ZERO(&_read_set);
        timeout.tv_usec = 0;
        timeout.tv_sec = 3 * 60;
        _read_set = _current_set;
 //       accept_clients();
        rc = select(_max + 1, &_read_set, NULL, NULL, &timeout);
        if (rc <= 0) // negative is select() error and 0 is select() timeout
            break;
        transmit_data();
        accept_clients();
    }
    close_all();
    return 0;
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
        keep_alive = false;
    std::cout << "Received SIGINT signal\n";
    std::cout << "Shutting down gracefully ...\n";
}