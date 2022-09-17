/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 15:08:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/17 18:44:33 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

extern bool keep_alive;

# include "../inc/Config.hpp"
# include "../inc/Exception.hpp"
#define GREEN "\e[92m"
#define RED	"\033[31m"
#define RESET "\033[0m"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

# include <csignal>

#define BACKLOG 255
#define NBPORTS 3
#define READ_BUFFER 1024
#define log(c, msg, x) std::cout << c << msg << x << "\n" << RESET;
# include "../inc/Cgi.hpp"

# include "Request.hpp"
# include "Client.hpp"

class Config;
class Cgi;

class Webserv//: public ServerInfo // Do we need to have it as inherited class ?
{
private:
    std::vector<ServerInfo>         _servers; // each one of them will contains port, listening socket and listening addrs
    //std::vector<int>                _ports;
    //std::vector<int>                _listening_sockets; // vector of sockets used with listen() // _ports, _sockets and _addrs should have the same size
    //std::vector<struct sockaddr_in> _listening_addrs; // those are the addr structs associated with listening sockets


    fd_set                          _current_set;
    fd_set                          _read_set;
    fd_set                          _write_set;
    int                             _fd_max; // biggest fd used so far
    //std::vector<int>                _clients_sockets; // vector of active sockets that were opened by accept()
    //std::vector<struct sockaddr_in> _clients_addrs;

    //std::vector<std::pair<int, struct sockaddr_in> >    _clients_pair;
    std::vector<Client *> _clients_list;

    Webserv();
    Webserv(Webserv const & x);
    Webserv &   operator=(Webserv const & x);

public:
    Webserv(std::vector<ServerInfo> &s);
    ~Webserv();

    std::vector<ServerInfo> &getServers();
    std::vector<int> &getWbsrvPorts();

    //bool isCGI_request(std::string html_content); // check if action and method fit for cgi
    // this function is part of class CGI
    void close_all();
    int set_server();
    void close_all(std::vector<int> &sockets);
    int set_server(std::vector<struct sockaddr_in> &addrs);
    void parse_request(std::string &request);
    int run_server();
    void accept_clients();
    void transmit_data();
    void checking_for_new_clients();
    void looping_through_read_set();
    void looping_through_write_set();

    int     get_fd_max() const;
    bool    is_listening_socket(int socket) const;
    sockaddr_in const & get_addrs_associated_with_listening_socket(int listening_socket) const;
    Client     *accept_new_client(int listening_socket); // this calls accept() and store socket and addrs of newly created connection. The client is allocated, needs to be deallocated
};

void signal_handler(int signum);
