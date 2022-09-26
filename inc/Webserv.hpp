/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 15:08:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/26 15:11:25 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

extern bool keep_alive;

# define GREEN "\e[92m"
# define RED	"\033[31m"
# define RESET "\033[0m"

# define BACKLOG 255
# define NBPORTS 3
# define READ_BUFFER 16384 // 2^14
# define MAX_URI 1024 // 2^14
# define log(c, msg, x) std::cout << c << msg << x << "\n" << RESET;

# include <stdio.h>
# include <stdlib.h>
# include <sys/ioctl.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <errno.h>
# include <unistd.h>
# include <string.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <list>
# include <csignal>

# include "Config.hpp"
# include "Exception.hpp"
# include "Request.hpp"
# include "Client.hpp"
# include "Response.hpp"

class Config;
class Cgi;
class Response;

class Webserv
{
    private:
        std::vector<ServerInfo>           _servers; // each one of them will contains port, listening socket and listening addrs
        //std::vector<int>                _ports;
        //std::vector<int>                _listening_sockets; // vector of sockets used with listen() // _ports, _sockets and _addrs should have the same size
        //std::vector<struct sockaddr_in> _listening_addrs; // those are the addr structs associated with listening sockets


        fd_set                          _current_set;
        fd_set                          _read_set;
        fd_set                          _write_set;
        //int                             _fd_max; // biggest fd used so far
        //std::vector<int>                _clients_sockets; // vector of active sockets that were opened by accept()
        //std::vector<struct sockaddr_in> _clients_addrs;

        //std::vector<std::pair<int, struct sockaddr_in> >    _clients_pair;
        std::list<Client *> _clients_list;

        Webserv();
        Webserv(Webserv const & x);
        Webserv &   operator=(Webserv const & x);

    public:
        Webserv(std::vector<ServerInfo> const &s);
        ~Webserv();

        std::vector<ServerInfo>     getServers();
        std::vector<int>            &getWbsrvPorts();

        void    close_all();
        int     set_server();
        void    close_all(std::vector<int> &sockets);
        int     set_server(std::vector<struct sockaddr_in> &addrs);
        void    parse_request(std::string &request);
        int     run_server();
        void    accept_clients();
        void    transmit_data();
        void    checking_for_new_clients();
        void    looping_through_read_set();
        void    looping_through_write_set();

        int     get_fd_max() const;
        ServerInfo *get_server_associated_with_listening_socket(int listening_socket);
        // this calls accept() and store socket and addrs of newly created connection. 
        // The client is allocated, needs to be deallocated
        Client     *accept_new_client(int listening_socket);
        void        parseHeader(Client *c);
        Response    handleRequest(Client const &c) const;

};
        
void signal_handler(int signum);
