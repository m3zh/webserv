/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/15 18:32:27 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */

# include "../inc/Webserv.hpp"

bool keep_alive = true; // should we put that as a public attribute of webserv class ?

std::vector<ServerInfo>&     Webserv::getServers()       {   return _servers;  };
//std::vector<int>&            Webserv::getWbsrvPorts()        {   return _ports;  };

Webserv::Webserv(std::vector<ServerInfo> &s) : _servers(s)
{
    //std::vector<int> tmp(s.size());
    //_listening_sockets = tmp;
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        log(GREEN, "SERVER NAME = ", it->getServerName());
        log(GREEN, "SERVER PORT = ", it->getPort());
        log(GREEN, "MAX CLIENT BODY = ", it->getClientMaxBodySize());
        log(GREEN, "ROOT = ", it->getServerRoot());
        log(GREEN, "INDEX = ", it->getServerIndex());
        //std::vector<page> current_pages = it->getPages();
        //_ports.push_back(it->getPort());
        log(RED, "----------------------------------------------------------", 0);
    }
    //_size = _ports.size(); // no need
    //_listening_sockets.resize(_ports.size());
    //_listening_addrs.resize(_ports.size());
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
    for (std::vector<ServerInfo>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
        close((*it).getListeningSocket());


    //for (std::vector<int>::iterator it = _listening_sockets.begin(); it != _listening_sockets.end(); it++)
    //    close(*(it));
}

int     Webserv::set_server()
{
    int on = 1;
    //for (size_t i = 0; i != _ports.size(); i++)
    for (std::vector<ServerInfo>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
    {
        int listening_socket;
        if ((listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            close_all();
            return -1;
        }
        if (fcntl(listening_socket, F_SETFL, O_NONBLOCK) < 0)
        {
            close_all();
            return -2;
        }
        if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        {
            close_all();
            return -3;
        }
        struct sockaddr_in  listening_addrs;
        memset(&listening_addrs, 0, sizeof(listening_addrs));
        listening_addrs.sin_family = AF_INET;
        listening_addrs.sin_addr.s_addr = inet_addr("127.0.0.1");
        listening_addrs.sin_port = htons((*it).getPort());
        if ((bind(listening_socket, (struct sockaddr *)&listening_addrs, sizeof(listening_addrs))) < 0)
        {
            close_all();
            return -4;
        }
        if ((listen(listening_socket, BACKLOG) < 0))
        {
            close_all();
            return -5;
        }
        (*it).setListeningSocket(listening_socket);
        (*it).setListeningAddrs(listening_addrs);
    }
    return 0;
}



//void    Webserv::accept_clients()
//{
//    for (size_t i = 0; i < _listening_sockets.size() && (int)i < _fd_max; i++)
//    {
//        if (FD_ISSET(_listening_sockets[i], &_read_set))
//        {
//            socklen_t len = sizeof(sockaddr[i]); // what is that ?
//            int client_socket = accept(_listening_sockets[i], (struct sockaddr*)&_listening_addrs[i], &len);
//            if (client_socket < 0)
//                close_all();
//            FD_SET(client_socket, &_current_set);
//            _clients_sockets.push_back(client_socket);
//            if (client_socket >= _fd_max)
//                _fd_max = client_socket;
//            break ; // as soon as a client is pushed back, we break out of the for loop
//        }
//    }
//}

//void    Webserv::parse_request(std::string &request)
//{
//    size_t pos = 0;
//    std::string res;
//    if (!request.length())
//        return ;
//    while ((pos = request.find('\n')) != std::string::npos)
//    {
//        res = request.substr(0, pos);
//        request.erase(0, pos + 1);
//        std::cout << "|" << res << "|" << std::endl;
//    }
//}

//void    Webserv::transmit_data()
//{
//    int rd = 0;
//    int rw = 0;
//    char buffer[1024];
//    /////////////////////////////////////DATA TESTS
//    std::string ok = "HTTP/1.1 200\r\n\r\n";
//    std::string index;
//    std::ifstream ifs;
//    ifs.open("./pages/website1/index.html", std::fstream::in);
//    while(ifs.read(buffer, sizeof(buffer)))
//        index.append(buffer, sizeof(buffer));
//    index.append(buffer, ifs.gcount());
//    ok.append(index);
//    /////////////////////////////////////////
//    for (std::vector<int>::iterator it = _clients_sockets.begin(); it != _clients_sockets.end(); it++) // Is it possible to have more than 1 inside of this vector ?
//    {
//        bzero(&buffer, sizeof(buffer)); /* Clear the buffer */
//        rd = recv(*it, buffer, sizeof(buffer), 0);
//        if (rd <= 0)
//            break;

/////////////////////////////////////////////////////////////////////////

//        std::cout << "Request from " << *it << " : \n---------------------------\n" << buffer << "-----------------------" << std::endl;


//        Request req(buffer); // instanciate a Request class with the raw request as a constructor parameter

//        std::cout << "Data recovered from the request : \n";
//        std::cout << "method : " << req.get_method() << std::endl;
//        std::cout << "location : " << req.get_location() << std::endl;
//        std::cout << "http version : " << req.get_http_version() << std::endl;
//        std::cout << "\nDisplaying header map : \n";
//        for (std::map<std::string, std::string>::const_iterator it = req.get_header_map().begin(); it != req.get_header_map().end(); ++it)
//        {
//            std::cout << (*it).first << ": " << (*it).second << std::endl;
//        }
//        if (req.get_index_beginning_body() != std::string::npos) // it means there is a body
//        {
//            std::cout << "\nDisplaying the request body :\n";
//            std::cout << req.get_body() << std::endl;
//        }
//        std::cout << "\n\n";

////////////////////////////////////////////////////////////////////////////

//        rw = send(*it, ok.c_str(), ok.size(), 0);
//        if (rw <= 0)
//            break;
//        close(*it);
//        FD_CLR(*it, &_current_set);
//        //_clients.erase(it);
//    }
//    _clients_sockets.clear();
//}

int     Webserv::run_server()
{
    struct timeval timeout;
    int rc;

    rc = set_server();
    if (rc < 0)
        return -1;
    FD_ZERO(&_current_set);
    _fd_max = (this->_servers.rbegin())->getListeningSocket(); // the listening socket of the last server setup has the biggest fd so far
    for (std::vector<ServerInfo>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
        FD_SET((*it).getListeningSocket(), &_current_set);
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

        timeout.tv_usec = 0;
        timeout.tv_sec = 3 * 60;
        FD_ZERO(&_read_set);
        FD_ZERO(&_write_set); // do we need to zero them before assigning ?
        _read_set = _current_set;
        _write_set = _current_set;

        rc = select(_fd_max + 1, &_read_set, &_write_set, NULL, &timeout);
        if (rc <= 0) // negative is select() error and 0 is select() timeout
            break;
        looping_through_read_set();
        looping_through_write_set();
        //transmit_data();
        //accept_clients();
    }
    close_all();
    return 0;
}

void    Webserv::looping_through_read_set()
{
    for (int i = 0; i < this->_fd_max; i++)
    {
        if (FD_ISSET(i, &(this->_read_set)))
        { // here are only fd that are ready to read. If its a listening socket, we have to accept, if its a data socket, we have to read the actual data
            if (this->is_listening_socket(i))
            {
                this->_clients_list.push_back(this->accept_new_client(i));
                FD_SET(this->_clients_list.back()->client_socket, &_current_set);
            }
            else
            {
                std::cout << "yoo\n";
                char buffer[1024];
                bzero(&buffer, sizeof(buffer));
                recv(i, buffer, sizeof(buffer), 0);
                std::cout << "Request from " << i << " : \n---------------------------\n" << buffer << "-----------------------" << std::endl;
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

                send(i, "HTTP/1.1 200 OK\r\n\r\nYOPPP", 24, 0);
                close(i);
                FD_CLR(i, &_current_set);
            }
        }
    }
    
}

void    Webserv::looping_through_write_set()
{
    
}

bool    Webserv::is_listening_socket(int socket) const
{
    for (std::vector<ServerInfo>::const_iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
    {
        if (socket == (*it).getListeningSocket())
            return (true);
    }
    return (false);
}

Client     *Webserv::accept_new_client(int listening_socket)
{
    // the returner client is allocated in the heap. Do not forget to deallocate it
    struct sockaddr_in  addr_of_client;
    socklen_t   len_for_accept = sizeof(addr_of_client);
    int client_socket = accept(listening_socket, (struct sockaddr *)&addr_of_client, &len_for_accept);
    if (client_socket < 0)
    {/*PROBLEM*/}
    if (client_socket > this->_fd_max)
        this->_fd_max = client_socket;
    Client *ret = new Client(client_socket, addr_of_client, listening_socket, this->get_addrs_associated_with_listening_socket(listening_socket));
    return (ret);
}

sockaddr_in const & Webserv::get_addrs_associated_with_listening_socket(int listening_socket) const
{
    for (std::vector<ServerInfo>::const_iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
    {
        if ((*it).getListeningSocket() == listening_socket)
            return ((*it).getListeningAddrs());
    }
    return (this->_servers[0].getListeningAddrs()); // to make it compile, but should never get out of the for loop
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
        keep_alive = false;
    std::cout << "Received SIGINT signal\n";
    std::cout << "Shutting down gracefully ...\n";
}