/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/12 17:15:05 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */

# include "../inc/Webserv.hpp"

bool keep_alive = true; // should we put that as a public attribute of webserv class ?

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
    for (size_t i = 0; i < _sockets.size() && (int)i < _fd_max; i++)
    {
        if (FD_ISSET(_sockets[i], &_read_set))
        {
            socklen_t len = sizeof(sockaddr[i]); // what is that ?
            int client_socket = accept(_sockets[i], (struct sockaddr*)&_addrs[i], &len);
            if (client_socket < 0)
                close_all();
            FD_SET(client_socket, &_current_set);
            _clients.push_back(client_socket);
            if (client_socket >= _fd_max)
                _fd_max = client_socket;
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
    int rc;
    
    //end_server = false;
    rc = set_server();
    if (rc < 0)
        return -1;
    FD_ZERO(&_current_set);
    _fd_max = _sockets.back(); // better to call std::max or something
    for (std::vector<int>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
        FD_SET(*it, &_current_set);
    signal(SIGINT, signal_handler);
    while (keep_alive)///////////////////////
    {

/*
        call select with read set and write set
        loop first over read set and check if its the listening fd
            if yes, accept and add it to the current set
            if no, read a chunk of data from it // need to append data at each loop until EOF is reached
                if EOF is reached, flag that socket as ready to respond (but it still has to be selected by select() for writing)
                at this stage we generate the response to be sent
        loop then over the write set
            for each writable fd that has been fully read from (EOF has been reached, and maybe should not be readable from select() ?)
                write a chunk of the response to it (if we just wrote the lask chunk, do we just close the socket and remove it from current list ?)

*/
        FD_ZERO(&_read_set);
        timeout.tv_usec = 0;
        timeout.tv_sec = 3 * 60;
        _read_set = _current_set;

        rc = select(_fd_max + 1, &_read_set, NULL, NULL, &timeout);
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