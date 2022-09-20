/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/20 17:26:48 by artmende         ###   ########.fr       */
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
    //_fd_max = (this->_servers.rbegin())->getListeningSocket(); // the listening socket of the last server setup has the biggest fd so far
    for (std::vector<ServerInfo>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
        FD_SET((*it).getListeningSocket(), &_current_set);
    signal(SIGINT, signal_handler);
    while (keep_alive)///////////////////////
    {
        std::cout << "main while loop (run server)\n";
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

        //rc = select(_fd_max + 1, &_read_set, &_write_set, NULL, &timeout); // select will test listening fd, and clients
        rc = select(1 + this->get_fd_max(), &_read_set, &_write_set, NULL, &timeout);
        if (rc <= 0) // negative is select() error and 0 is select() timeout
            break;
        // what about accepting clients here ? looping through all listening socket
        // then we can just loop through all clients after
        checking_for_new_clients();
        looping_through_read_set();
        looping_through_write_set();
        //transmit_data();
        //accept_clients();
    }
    close_all();
    return 0;
}

void    Webserv::checking_for_new_clients()
{
    // looping through all listening sockets to see if some have been picked by select()
    for (std::vector<ServerInfo>::const_iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
    {
        if (FD_ISSET((*it).getListeningSocket(), &_read_set))
        { // there is a new client to accept, we instantiate a Client class and add it to the clients list
            std::cout << "new client ! about to call accept. size of client list now is : " << this->_clients_list.size() << std::endl;
            this->_clients_list.push_back(this->accept_new_client((*it).getListeningSocket()));
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

    for (std::list<Client*>::iterator it = this->_clients_list.begin(); it != this->_clients_list.end(); ++it)
    {
        if (FD_ISSET((*it)->client_socket, &_read_set))
        {
            std::cout << "just entered if (FD_ISSET) and client is read complete ? " << (*it)->is_read_complete << std::endl;
            char buffer[READ_BUFFER];
            bzero(&buffer, sizeof(buffer));
            int return_of_recv;
            if ((*it)->has_header_been_read == false) // we can call parse_request as soon as the header is read. The beginning of the body will be there at the first call to recv()
            {
                if ((return_of_recv = recv((*it)->client_socket, buffer, sizeof(buffer), 0)) == -1)
                {/*ERROR*/}
                (*it)->request_str.append(buffer, return_of_recv); // append() method will include \0 if some are present in the buffer
                (*it)->parse_request();
                (*it)->has_header_been_read = true;
                std::cout << return_of_recv << " bytes of the header have been read. We have an index for beginning of body of : " << (*it)->request_class.get_index_beginning_body() << std::endl;
                // now we have to check if there is actually something more to read.
                // If we don't have a body, reading is done. If there is a body, we have to compare content_length with what we received
                if ((*it)->request_class.get_index_beginning_body() == std::string::npos) // means we don't have a body
                    (*it)->mark_read_as_complete();
                else // there is a body.
                {
                    std::cout << "there is a body and we have more to read\n";
                    std::map<std::string, std::string>::const_iterator    content_length_it = (*it)->request_class.get_header_map().find("Content-Length");
                    if (content_length_it == (*it)->request_class.get_header_map().end())
                    {/*ERROR : Content length not present in the map*/}
                    if (((*it)->request_str.size() - (*it)->request_class.get_index_beginning_body()) == (unsigned long)(atoi((content_length_it->second).c_str())) )
                    {
                        std::cout << "After the first reading of header, we see that there is a body, but we read everything already\n";
                        // that means we received at least content-length bytes of the body. Read should be complete. probably need to put ==
                        (*it)->mark_read_as_complete();
                    }
                }
            }
            else // header has been read and parsed and there is more data to read (we have to check content-length)
            {
                std::map<std::string, std::string>::const_iterator    content_length_it = (*it)->request_class.get_header_map().find("Content-Length");
                if (content_length_it == (*it)->request_class.get_header_map().end())
                {/*ERROR : Content length not present in the map*/}
                if ((return_of_recv = recv((*it)->client_socket, buffer, sizeof(buffer), 0)) == -1)
                {/*ERROR with reading*/}
                (*it)->request_str.append(buffer, return_of_recv); // append() method will include \0 if some are present in the buffer
                if (((*it)->request_str.size() - (*it)->request_class.get_index_beginning_body()) >= (unsigned long)(atoi((content_length_it->second).c_str())) )
                {
                    // that means we received at least content-length bytes of the body. Read should be complete. probably need to put ==
                    (*it)->mark_read_as_complete();
                }
            }
                std::cout << "exiting if (FD_ISSET) and client is read complete ? " << (*it)->is_read_complete << std::endl;
        }
    }
}

void    Webserv::looping_through_write_set()
{
    // looping to all clients to see which one is ready to receive data.
    // condition to send to a client is that recv has returned 0 (nothing more to read) and that select() has put it in the write set
    
    for (std::list<Client*>::iterator it = this->_clients_list.begin(); it != this->_clients_list.end(); /*++it*/)
    {
        if (FD_ISSET((*it)->client_socket, &(this->_write_set)) && (*it)->is_read_complete == true)
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
            std::cout << "Full request has size " << (*it)->request_str.size() << " and str is :\n----------------\n" << (*it)->request_str << "\n-------------" << std::endl;
            //if ((send((*it)->client_socket, "HTTP/1.1 200 OK\r\n\r\nYOPPP", 24, 0)) < 0)
            if ((send((*it)->client_socket, ok.c_str(), ok.size(), 0)) < 0)
            {/*ERROR*/}
            close ((*it)->client_socket);
            FD_CLR((*it)->client_socket, &_current_set);
            std::list<Client*>::iterator    to_delete = it;
            ++it;
            delete (*to_delete); // Client is allocated
            this->_clients_list.erase(to_delete);
        }
        else
            ++it;
    }
}

int     Webserv::get_fd_max() const
{
    // if there are clients, get the max fd from them. If no clients, get the max listening fd
    if (this->_clients_list.size() != 0)
    {
        int fd_max = (*(this->_clients_list).begin())->client_socket;
        for (std::list<Client*>::const_iterator it = this->_clients_list.begin(); it != this->_clients_list.end(); ++it)
        {
            if ((*it)->client_socket > fd_max)
                fd_max = (*it)->client_socket;
        }
        return (fd_max);
    }
    else
        return (this->_servers.back().getListeningSocket()); // last added server has the biggest listening socket
}

bool    Webserv::is_listening_socket(int socket) const // that function is not used anymore
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
    // the returned client is allocated in the heap. Do not forget to deallocate it
    struct sockaddr_in  addr_of_client;
    socklen_t   len_for_accept = sizeof(addr_of_client);
    int client_socket = accept(listening_socket, (struct sockaddr *)&addr_of_client, &len_for_accept);
    std::cout << "new client accepted ! socket is " << client_socket << std::endl;
    if (client_socket < 0)
    {/*PROBLEM*/}
    fcntl(client_socket, F_SETFL, O_NONBLOCK);///////////////////// Not sure if it is needed here
    FD_SET(client_socket, &_current_set);
    Client *ret = new Client(client_socket, addr_of_client, this->get_server_associated_with_listening_socket(listening_socket));
    return (ret);
}

ServerInfo const &  Webserv::get_server_associated_with_listening_socket(int listening_socket) const
{
    for (std::vector<ServerInfo>::const_iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
    {
        if ((*it).getListeningSocket() == listening_socket)
            return (*it);
    }
    return (this->_servers[0]); // to make it compile, but should never get out of the for loop
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
        keep_alive = false;
    std::cout << "Received SIGINT signal\n";
    std::cout << "Shutting down gracefully ...\n";
}