/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 15:08:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/27 17:21:54 by mlazzare         ###   ########.fr       */
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
# define MAX_URI 1024
# define MAX_UPLOAD_FILE 2048
//# define log(c, msg, x) std::cout << c << msg << x << "\n" << RESET;

# include <stdio.h>
# include <stdlib.h>
# include <sys/ioctl.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/stat.h>
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
# include "Cgi.hpp"

class Webserv
{
    private:
        std::vector<ServerInfo>     _servers; // each one of them will contains port, listening socket and listening addrs

        fd_set                      _current_set;
        fd_set                      _read_set;
        fd_set                      _write_set;

        std::list<Client *>         _clients_list;

        Webserv();
        Webserv(Webserv const & x);
        Webserv &   operator=(Webserv const & x);

    public:
        Webserv(std::vector<ServerInfo> const &s);
        ~Webserv();


        // GETTERS
        std::vector<ServerInfo>     getServers();
        ServerInfo                  *get_server_associated_with_listening_socket(int listening_socket);
        int                         get_fd_max() const;

        // SETTERS
        int         set_server();

        // SERVER FUNCTION
        int         run_server();
        void        checking_for_new_clients();
        Client      *accept_new_client(int listening_socket);       // this calls accept() and store socket and addrs of newly created connection.
        void        looping_through_read_set();
        void        looping_through_write_set();
        void        close_all();

        // REQUEST HANDLERS
        void        parseHeader(Client *c);
        void        handleRequest(Client *c)    const;
        void        GETmethod(Client *c)        const;
        void        POSTmethod(Client *c)       const;
        void        DELETEmethod(Client *c)     const;
};
        
void signal_handler(int signum);
