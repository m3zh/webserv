/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 15:08:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/15 10:03:55 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
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
#define BACKLOG 255
#define NBPORTS 3
#define log(c, msg, x) std::cout << c << msg << x << "\n" << RESET;
# include "../inc/Cgi.hpp"

class Config;
class Cgi;

class Webserv: public ServerInfo
{
    private:
        int                             _max;
        int                             _connection;
        size_t                          _size;
        std::vector<ServerInfo>         _servers;
        std::vector<int>                _clients;
        std::vector<int>                _ports;
        std::vector<int>                _sockets;
        std::vector<struct sockaddr_in> _addrs;
        fd_set                          _current_set;
        fd_set                          _read_set;

    public:
        Webserv(std::vector<ServerInfo> &s);
        ~Webserv();

        std::vector<ServerInfo>&    getServers();
        std::vector<int>&           getWbsrvPorts();

        bool    isCGI_request(std::string html_content);            // check if action and method fit for cgi
        void    close_all();
        int     set_server();
        void    close_all(std::vector<int> &sockets);
        int     set_server(std::vector<struct sockaddr_in> &addrs);
        void    parse_request(std::string &request);
        int     run_server();
        void    accept_clients();
        void    transmit_data();
};