/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/24 16:48:26 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"

bool keep_alive = true;

std::vector<ServerInfo>         Webserv::getServers()           {   return _servers;  };
//std::vector<int>&             Webserv::getWbsrvPorts()        {   return _ports;  };

Webserv::Webserv(std::vector<ServerInfo> const &s) : _servers(s)
{
    //std::vector<int> tmp(s.size());
    //_listening_sockets = tmp;
    // for (std::vector<ServerInfo *>::iterator it = _servers.begin(); it != _servers.end(); it++)
    // {
    //     log(GREEN, "SERVER NAME = ", it->getServerName());
    //     log(GREEN, "SERVER PORT = ", it->getPort());
    //     log(GREEN, "MAX CLIENT BODY = ", it->getClientMaxBodySize());
    //     log(GREEN, "ROOT = ", it->getServerRoot());
    //     log(GREEN, "INDEX = ", it->getServerIndex());
    //     //std::vector<page> current_pages = it->getPages();
    //     //_ports.push_back(it->getPort());
    //     log(RED, "----------------------------------------------------------", 0);
    // }
}

Webserv::~Webserv()                     {};

void    Webserv::close_all()
{
    for ( std::vector<ServerInfo>::iterator it = _servers.begin();
            it != _servers.end(); it++ )
        close((*it).getListeningSocket());
}

int     Webserv::set_server()
{
    int on = 1;
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        int listening_socket;
        if ((listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)                       {   close_all();    return -1;      }
        if (fcntl(listening_socket, F_SETFL, O_NONBLOCK) < 0)                               {   close_all();    return -2;      }
        if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)    {   close_all();    return -3;      }
        struct sockaddr_in  listening_addrs;
        memset(&listening_addrs, 0, sizeof(listening_addrs));
        listening_addrs.sin_family = AF_INET;
        listening_addrs.sin_addr.s_addr = inet_addr("127.0.0.1");
        listening_addrs.sin_port = htons((*it).getPort());
        if ((bind(listening_socket, (struct sockaddr *)&listening_addrs, sizeof(listening_addrs))) < 0)     {   close_all();    return -4;  }
        if ((listen(listening_socket, BACKLOG) < 0))                                        {   close_all();    return -5;      }
        (*it).setListeningSocket(listening_socket);
        (*it).setListeningAddrs(listening_addrs);
    }
    return 0;
}

int     Webserv::run_server()
{
    struct timeval timeout;

    if (set_server() < 0)
        return -1;
    FD_ZERO(&_current_set);
    //_fd_max = (_servers.rbegin())->getListeningSocket(); // the listening socket of the last server setup has the biggest fd so far
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
        FD_SET((*it).getListeningSocket(), &_current_set);
    signal(SIGINT, signal_handler);
    while (keep_alive)///////////////////////
    {
        std::cout << "Connecting ...\n";
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

        // select will test listening fd, and clients
        int select_fd = select(1 + get_fd_max(), &_read_set, &_write_set, NULL, &timeout);
        if (select_fd <= 0) // negative is select() error and 0 is select() timeout
            throw WebException<int>(BLUE, "WebServ error: select failed on fd", select_fd);
        // looping through all listening socket
        checking_for_new_clients();
        looping_through_read_set();
        looping_through_write_set();
    }
    close_all();
    return 0;
}

void    Webserv::checking_for_new_clients()
{
    // looping through all listening sockets to see if some have been picked by select()
    for (std::vector<ServerInfo>::iterator it = _servers.begin();
        it != _servers.end(); it++)
    {
        if (FD_ISSET((*it).getListeningSocket(), &_read_set))
        { // there is a new client to accept, we instantiate a Client class and add it to the clients list
            std::cout << "new client ! about to call accept. size of client list now is : " << _clients_list.size() << std::endl;
            _clients_list.push_back(accept_new_client((*it).getListeningSocket()));
        }
    }
}

void    Webserv::looping_through_read_set()
{
    // for each client(in the read set), check if header has been read yet.
        // if header has been read already (there is a body), read a buffer, if content length match, mark read as completed
        // if header has not been read yet, read a buffer, then find the first occurence of \r\n\r\n
        // and compare its index with the request size to mark the read as completed or not (and mark the header as read)
        // if \r\n\r\n cannot be found, we have a request error. can be header too long, can be something else

    for (std::list<Client*>::iterator it = _clients_list.begin(); it != _clients_list.end(); ++it)
    {
        int client_socket = (*it)->getClientSocket();
        if (FD_ISSET(client_socket, &_read_set))
        {
            std::cout << "just entered if (FD_ISSET) and client is read complete ? " << (*it)->isReadComplete() << std::endl;
            char buffer[READ_BUFFER];
            bzero(&buffer, sizeof(buffer));
            int return_of_recv;
            if ((*it)->headerIsReadComplete() == false) // we can call parse_request as soon as the header is read. The beginning of the body will be there at the first call to recv()
            {
                if ((return_of_recv = recv(client_socket, buffer, sizeof(buffer), 0)) == -1)
                    throw WebException<int>(BLUE, "WebServ error: receiving failed on client socket ", client_socket);
                (*it)->getRequestString().append(buffer, return_of_recv); // append() method will include \0 if some are present in the buffer
                (*it)->parseHeader();
                (*it)->setHeaderReadAsComplete(true);
                std::cout << return_of_recv << " bytes of the header have been read. We have an index for beginning of body of : " << (*it)->getRequest().get_index_beginning_body() << std::endl;
                // now we have to check if there is actually something more to read.
                // If we don't have a body, reading is done. If there is a body, we have to compare content_length with what we received
                if ((*it)->getRequest().get_index_beginning_body() == std::string::npos) // means we don't have a body
                    (*it)->setReadAsComplete(true);
                else // there is a body.
                {
                    std::cout << "there is a body and we have more to read\n";
                    std::map<std::string, std::string>::iterator    content_length_it = (*it)->getRequest().get_header_map().find("Content-Length");
                    if (content_length_it == (*it)->getRequest().get_header_map().end())
                        throw WebException<int>(BLUE, "WebServ error: no Content-Length on client socket ", client_socket);
                    if (((*it)->getRequestString().size() - (*it)->getRequest().get_index_beginning_body()) == (unsigned long)(atoi((content_length_it->second).c_str())) )
                    {
                        std::cout << "After the first reading of header, we see that there is a body, but we read everything already\n";
                        // that means we received at least content-length bytes of the body. Read should be complete. probably need to put ==
                        (*it)->setReadAsComplete(true);
                    }
                }
            }
            else // header has been read and parsed and there is more data to read (we have to check content-length)
            {
                std::map<std::string, std::string>::const_iterator    content_length_it = (*it)->getRequest().get_header_map().find("Content-Length");
                if (content_length_it == (*it)->getRequest().get_header_map().end())
                    throw WebException<int>(BLUE, "WebServ error: no Content-Length on client socket ", client_socket);
                if ((return_of_recv = recv(client_socket, buffer, sizeof(buffer), 0)) == -1)
                    throw WebException<int>(BLUE, "WebServ error: receiving failed on client socket ", client_socket);
                (*it)->getRequestString().append(buffer, return_of_recv); // append() method will include \0 if some are present in the buffer
                if (((*it)->getRequestString().size() - (*it)->getRequest().get_index_beginning_body())
                        == (unsigned long)(atoi((content_length_it->second).c_str())) ) // that means we received at least content-length bytes of the body. Read should be complete. probably need to put ==
                    (*it)->setReadAsComplete(true);
            }
            std::cout << "exiting if (FD_ISSET) and client is read complete ? " << (*it)->isReadComplete() << std::endl;
            if ((*it)->isReadComplete())
                std::cout << "Reading complete.\nMethod requested is\n" << (*it)->getRequestString();
        }
        
    }
}

void    Webserv::looping_through_write_set()
{
    // looping to all clients to see which one is ready to receive data.
    // condition to send to a client is that recv has returned 0 (nothing more to read) and that select() has put it in the write set
    
    for (std::list<Client*>::iterator it = _clients_list.begin(); it != _clients_list.end(); /*++it*/)
    {
        if ( FD_ISSET((*it)->getClientSocket(), &(_write_set)) && (*it)->isReadComplete() )
        {

            // down here is dummy response for developement purpose

            std::string ok = "HTTP/1.1 200\r\n\r\n";
            char temp_buffer[1024];
            std::ifstream   temp_stream("./pages/website1/index.html", std::ios_base::in | std::ios_base::binary);

            while (temp_stream.good()) // when EOF is reached, or in case of error, we break out of the loop
            {
                temp_stream.read(temp_buffer, sizeof(temp_buffer));
                ok.append(temp_buffer, temp_stream.gcount());
            }
            std::cout << "about to send a response\n";
            std::cout << "Full request has size " << (*it)->getRequestString().size() << " and str is :\n----------------\n" << (*it)->getRequestString() << "\n-------------" << std::endl;
            //if ((send((*it)->getClientSocket(), "HTTP/1.1 200 OK\r\n\r\nYOPPP", 24, 0)) < 0)
            if ((send((*it)->getClientSocket(), ok.c_str(), ok.size(), 0)) < 0)
                return ;
            close ((*it)->getClientSocket());
            FD_CLR((*it)->getClientSocket(), &_current_set);
            std::list<Client*>::iterator    to_delete = it;
            ++it;
            delete (*to_delete); // Client is allocated
            _clients_list.erase(to_delete);
        }
        else
            ++it;
    }
}

int     Webserv::get_fd_max() const
{
    // if there are clients, get the max fd from them. If no clients, get the max listening fd
    if (_clients_list.size() != 0)
    {
        int fd_max = (*(_clients_list).begin())->getClientSocket();
        for (std::list<Client*>::const_iterator it = _clients_list.begin(); it != _clients_list.end(); ++it)
            if ((*it)->getClientSocket() > fd_max)
                fd_max = (*it)->getClientSocket();
        return fd_max;
    }
    else
        return (_servers.back().getListeningSocket()); // last added server has the biggest listening socket
}

Client     *Webserv::accept_new_client(int listening_socket)
{
    // the returned client is allocated in the heap. Do not forget to deallocate it
    struct sockaddr_in  addr_of_client;
    socklen_t   len_for_accept = sizeof(addr_of_client);
    int client_socket = accept(listening_socket, (struct sockaddr *)&addr_of_client, &len_for_accept);
    std::cout << "new client accepted ! socket is " << client_socket << std::endl;
    if (client_socket < 0)
        throw WebException<int>(RED, "WebServ error: Client not accepted on listening socket ", listening_socket);
    FD_SET(client_socket, &_current_set);
    Client *ret = new Client(client_socket, addr_of_client, get_server_associated_with_listening_socket(listening_socket));
    return (ret);
}

ServerInfo *Webserv::get_server_associated_with_listening_socket(int listening_socket)
{
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
        if ((*it).getListeningSocket() == listening_socket)
            return &(*it);
    return nullptr;
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
        keep_alive = false;
    std::cout << "Received SIGINT signal\n";
    std::cout << "Shutting down gracefully ...\n";
}