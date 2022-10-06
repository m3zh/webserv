/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/10/05 10:01:02 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"

bool keep_alive = true;

Webserv::Webserv(std::vector<ServerInfo> const &s) : _servers(s)    {};
Webserv::~Webserv()                     {};

std::vector<ServerInfo>         Webserv::getServers()           {   return _servers;  };
int                             Webserv::set_server()
{
    int on = 1;
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        int listening_socket;
        // check for creation of socket to bind with each server
        if ((listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)                       {   close_all();    return -1;      }
        // setting socket in non blocking mode, to allow it to connect to multiple clients ; instead of having a blocking socket restricted to one client
        if (fcntl(listening_socket, F_SETFL, O_NONBLOCK) < 0)                               {   close_all();    return -2;      }
        // setting socket options to allow it to reuse local address (even after a client disconnects)
        if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)    {   close_all();    return -3;      }
        struct sockaddr_in  listening_addrs;
        // we use memset to initialize a sockaddr_in structure
        memset(&listening_addrs, 0, sizeof(listening_addrs));
        listening_addrs.sin_family = AF_INET;
        // adding port
        listening_addrs.sin_addr.s_addr = inet_addr("127.0.0.1");
        // convert host address to network bytes address
        listening_addrs.sin_port = htons((*it).getPort());
        // we bind the socket to a "file" descriptor => we get a socket descriptor
        if ((bind(listening_socket, (struct sockaddr *)&listening_addrs, sizeof(listening_addrs))) < 0)     {   close_all();    return -4;  }
        // we launch the socket, with a maximum of 256 different sockets in the queue
        if ((listen(listening_socket, BACKLOG) < 0))                                        {   close_all();    return -5;      }
        (*it).setListeningSocket(listening_socket);
        (*it).setListeningAddrs(listening_addrs);
    }
    return 0;
}

int     Webserv::run_server()
{
    struct timeval timeout;
    int select_fd;
    
    if (set_server() < 0)
        return -1;
    FD_ZERO(&_current_set);
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
        FD_SET((*it).getListeningSocket(), &_current_set);
    signal(SIGINT, signal_handler);
    while (keep_alive)
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
        try {   select_fd = select(1 + get_fd_max(), &_read_set, &_write_set, NULL, &timeout);              }
        catch (...) {  throw WebException<int>(BLUE, "WebServ error: select failed on fd", select_fd);      }
            
        // looping through all listening socket
        checking_for_new_clients();
        looping_through_read_set();
        looping_through_write_set();
    }
    close_all();
    return 0;
}

// CLIENTS
void    Webserv::checking_for_new_clients()
{
    // looping through all listening sockets to see if some have been picked by select()
    for (std::vector<ServerInfo>::iterator it = _servers.begin();
        it != _servers.end(); it++)
    {
        if (FD_ISSET((*it).getListeningSocket(), &_read_set) && keep_alive)
        { // there is a new client to accept, we instantiate a Client class and add it to the clients list
            std::cout << "new client ! about to call accept. size of client list now is : " << _clients_list.size() << std::endl;
            _clients_list.push_back(accept_new_client((*it).getListeningSocket()));
        }
    }
}

Client     *Webserv::accept_new_client(int listening_socket)
{
    // the returned client is allocated in the heap. Do not forget to deallocate it
    struct sockaddr_in  addr_of_client;
    socklen_t   len_for_accept = sizeof(addr_of_client);
    int client_socket;
    try {   client_socket = accept(listening_socket, (struct sockaddr *)&addr_of_client, &len_for_accept);              } // if (client_socket < 0)
    catch (...) {  throw WebException<int>(RED, "WebServ error: Client not accepted on listening socket ", listening_socket); return nullptr;      }
    std::cout << "new client accepted ! socket is " << client_socket << std::endl;      
    FD_SET(client_socket, &_current_set);
    Client *ret = new Client(client_socket, addr_of_client, get_server_associated_with_listening_socket(listening_socket));
    return (ret);
}


// READING AND WRITING
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
            std::cout << "Reading...\n" << std::endl;
            char buffer[READ_WRITE_BUFFER];
            bzero(&buffer, sizeof(buffer));
            int bytes_recv;
            // we can call parse_request as soon as the header is read. 
            // The beginning of the body will be there at the first call to recv()
            if ((*it)->headerIsReadComplete() == false) 
            {
                if ((bytes_recv = recv(client_socket, buffer, sizeof(buffer), 0)) == -1)
                    throw WebException<int>(BLUE, "WebServ error: receiving failed on client socket ", client_socket);
                buffer[bytes_recv] = 0;
                std::string buf(buffer);
                (*it)->setRequestString(buf);
                parseHeader(*it);
                std::cout << bytes_recv << " bytes of the header have been read.\n";
                
                // // means we don't have a body
                if ((*it)->getRequest().get_index_beginning_body() == std::string::npos) 
                    (*it)->setReadAsComplete(true);
                else // there is a body. We have to compare content_length with what we received
                {
                    std::cout << "Reading the body.\n";
                    std::cout << "Body index " << (*it)->getRequest().get_index_beginning_body() << "\n";
                    std::map<std::string, std::string>  header_map = (*it)->getRequest().get_header_map();
                    std::map<std::string, std::string>::iterator    content_length_it = header_map.find("Content-Length");
                    if (content_length_it == (*it)->getRequest().get_header_map().end())
                    {    throw WebException<int>(BLUE, "WebServ error: no Content-Length on client socket ", client_socket);    return;     }
                    else if (( (*it)->getRequestString().size() - (*it)->getRequest().get_index_beginning_body()) 
                        >= (unsigned long)(atoi((content_length_it->second).c_str())) )
                        (*it)->setReadAsComplete(true);
                }
            }
            else // header has been read and parsed and there is more data to read (we have to check content-length)
            {
                std::map<std::string, std::string>  header_map = (*it)->getRequest().get_header_map();
                std::map<std::string, std::string>::const_iterator    content_length_it = header_map.find("Content-Length");
                if (content_length_it == (*it)->getRequest().get_header_map().end())
                {    throw WebException<int>(BLUE, "WebServ error: no Content-Length on client socket ", client_socket);    return;     }
                if ((bytes_recv = recv(client_socket, buffer, sizeof(buffer), 0)) == -1)
                {    throw WebException<int>(BLUE, "WebServ error: receiving failed on client socket ", client_socket);     return;     }
                (*it)->getRequestString().append(buffer, bytes_recv); // append() method will include \0 if some are present in the buffer
                if (((*it)->getRequestString().size() - (*it)->getRequest().get_index_beginning_body())
                        >= (unsigned long)(atoi((content_length_it->second).c_str())) ) // bytes received match content-length bytes of the body
                    (*it)->setReadAsComplete(true);
            }
            if ((*it)->isReadComplete())    {
                std::cout << "Reading complete.\nRequest is:\n" << (*it)->getRequestString();
                handleRequest(*it); // we handle the request and create a response to send
            }
        }
    }
}

void    Webserv::looping_through_write_set()
{
    // need to make the it++ in the code itself, to manage when deleting clients
    // looping to all clients to see which one is ready to receive data.
    // condition to send to a client is that recv has returned 0 (nothing more to read) and that select() has put it in the write set
    
    for ( std::list<Client*>::iterator it = _clients_list.begin(); it != _clients_list.end(); /* it++ */ ) // increment must be done in the code because we delete from the list
    {
        int     client_socket = (*it)->getClientSocket();
        ssize_t bytes_sent;

        if ( FD_ISSET(client_socket, &(_write_set)) && (*it)->isReadComplete() )
        {
            if ((*it)->headerHasBeenSent() == false) // first send the header
            {
                if ((*it)->getRemainingBufferToSend().size() > 0) // in case the whole header could not be sent in 1 time
                {
                    if ((bytes_sent = send(client_socket, (*it)->getRemainingBufferToSend().c_str(), (*it)->getRemainingBufferToSend().size(), 0)) < 0)
                        throw WebException<int>(BLUE, "WebServ error: sending failed on client socket ", client_socket);
                    (*it)->getRemainingBufferToSend().erase(0, bytes_sent); // this will either clear the string, or leave there what was not sent yet
                    if ((*it)->getRemainingBufferToSend().size() == 0) // means it was all sent successfully
                        (*it)->setHeaderBeenSent(true);
                    ++it;
                    continue; // not more than 1 send() per select()
                }
                if ((bytes_sent = send(client_socket, (*it)->getResponseString().c_str(), (*it)->getResponseString().size(), 0)) < 0)
                    throw WebException<int>(BLUE, "WebServ error: sending failed on client socket ", client_socket);
                if ((std::size_t)bytes_sent < (*it)->getResponseString().size())
                    (*it)->getRemainingBufferToSend().assign((*it)->getResponseString(), bytes_sent, std::string::npos);
                else
                    (*it)->setHeaderBeenSent(true);
            }
            else // header has been sent already
            {
                // if there is no file to send, close the socket here
                if ((*it)->thereIsAFileToSend() == false)
                {
                    std::cout << "FALSE\n";
                    close(client_socket);
                    FD_CLR(client_socket, &_current_set);
                    std::list<Client*>::iterator    to_delete = it;
                    ++it; // ready for next loop cycle
                    delete (*to_delete); // Client is allocated
                    _clients_list.erase(to_delete);
                    continue;
                }
                if ((*it)->getRemainingBufferToSend().size())
                {
                    std::cout << "TRUE\n";
                    if ((bytes_sent = send(client_socket, (*it)->getRemainingBufferToSend().c_str(), (*it)->getRemainingBufferToSend().size(), 0)) < 0)
                        throw WebException<int>(BLUE, "WebServ error: sending failed on client socket ", client_socket);
                    (*it)->getRemainingBufferToSend().erase(0, bytes_sent); // this will either clear the string, or leave there what was not sent yet
                    ++it;
                    continue; // not more than 1 send() per select()
                }
                char             buffer[READ_WRITE_BUFFER];
                std::streamsize  effective_size_of_buffer = 0;

                if ((*it)->getResponseFileStream().good()) // what if we have issue with the stream ?
                {
                    (*it)->getResponseFileStream().read(buffer, sizeof(buffer));
                    effective_size_of_buffer = (*it)->getResponseFileStream().gcount();
                }
                if (effective_size_of_buffer != 0) // this could be 0 if EOF is reached or if there is an issue with the stream
                {
                    if ((bytes_sent = send(client_socket, buffer, effective_size_of_buffer, 0)) < 0)
                        throw WebException<int>(BLUE, "WebServ error: sending failed on client socket ", client_socket);
                    if (bytes_sent < effective_size_of_buffer)
                        (*it)->getRemainingBufferToSend().assign(&buffer[bytes_sent], effective_size_of_buffer - bytes_sent);
                }
                if ((*it)->getResponseFileStream().eof() == true
                    && (*it)->getRemainingBufferToSend().size() == 0) // the file has been fully transmitted
                {
                    close(client_socket);
                    FD_CLR(client_socket, &_current_set);
                    std::list<Client*>::iterator    to_delete = it;
                    ++it; // ready for next loop cycle
                    delete (*to_delete); // Client is allocated
                    _clients_list.erase(to_delete);
                    continue;
                }
            }
        }
        ++it; // put it here instead of in the for loop declaration
    }
}

void    Webserv::close_all()
{
    for ( std::vector<ServerInfo>::iterator it = _servers.begin();
            it != _servers.end(); it++ )
        close((*it).getListeningSocket());
    for ( std::list<Client*>::iterator it = _clients_list.begin();
            it != _clients_list.end(); it++ )
            delete *it;
     _clients_list.clear();

}

// GETTERS
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

ServerInfo *Webserv::get_server_associated_with_listening_socket(int listening_socket)
{
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
        if ((*it).getListeningSocket() == listening_socket)
            return &(*it);
    return nullptr;
}

// EXECUTING REQUEST AND CREATE RESPONSE
void    Webserv::parseHeader(Client *c)         {
                                                    c->setRequest(c->getRequestString());
                                                    c->setHeaderReadAsComplete(true);
                                                };

void    Webserv::handleRequest(Client *c)   const   {
                                                        std::string method = c->getRequest().get_method();
                                                        std::string uri = c->getRequest().get_location();
                                                        std::string version = c->getRequest().get_http_version();
                                                        if ( !method.size() || !uri.size() || !version.size() )
                                                            c->setResponseString(BAD_REQUEST, "", "");
                                                        if ( uri.size() > MAX_URI )
                                                            c->setResponseString(REQUEST_URI_TOO_LONG, "", "");
                                                        if ( version != "HTTP/1.1" )
                                                            c->setResponseString(HTTP_VERSION_NOT_SUPPORTED, "", "");
                                                        if (method == "GET")            GETmethod(c);
                                                        else if (method == "POST")      POSTmethod(c);
                                                        else if (method == "DELETE")    DELETEmethod(c);
                                                        else
                                                            c->setResponseString(METHOD_NOT_ALLOWED,"", "");
                                                    };

void Webserv::GETmethod(Client *c)  const
{
    std::string         pwd(getenv("PWD"));
    std::string         file_path;
    Cgi                 cgi;
    ServerInfo*         _server = c->getServerInfo();
    Request             req = c->getRequest();
    std::vector<page>   pages = _server->getPages();
    std::vector<page>::iterator page_requested = pages.begin();
    int                 redirect = 0;
    int                 fileInFolder = -1;

    for ( ; page_requested != pages.end(); page_requested++ )                                   // check for location in config
    {
        if ((*page_requested).location_path.back() == '/')
            file_path = (*page_requested).location_path.substr(0, (*page_requested).location_path.find_last_of("\\/")) + req.get_location();
        if ( file_path.back() != '/')                                                           // if it is not a folder, check file path
            fileInFolder = access((pwd + _server->getServerRoot() + file_path).c_str(), F_OK | R_OK);
        if ( req.get_location().compare((*page_requested).location_path) == 0                   // if the page required is exactly as in config
                || fileInFolder == 0 )                                                          // or if it is found in a config folder                     
        {                                                                                                       
            if ( invalidMethod(*page_requested, "GET") )                                        // check for method
            {    c->setResponseString(METHOD_NOT_ALLOWED, "", ""); return  ;   }
            if ( fileInFolder == -1 && (*page_requested).redirect.size() )                      // check for redirection
                redirect = 1;
            break ;
        }
    }
    if ( fileInFolder < 0 && page_requested == pages.end() )                                    // if nothing is found 
    {   
        if (req.get_location().find("py") != std::string::npos )  {                                                                                  // we check if it is a CGI request   
            if (cgi.isCGI_request(c)) {   std::cout << "GET request for CGI!" << std::endl; return ;        }
        }
        c->setResponseString(NOT_FOUND, "", "");  return ;        
    }
    if ( redirect )     {	c->setResponseString(MOVED_PERMANENTLY, page_requested->redirect, "");    return  ;     }
    if ( fileInFolder < 0 )  {
        std::string     path2file = pwd + _server->getServerRoot() + page_requested->location_path;
        std::ifstream   file(path2file.c_str());
        if ( !file.good() )     {    c->setResponseString(UNAUTHORIZED, "", "");  return ;                        }
        if ( isDirectory(  path2file ) )                                                        // if it is a directory, check for autoindex              
        {    checkAutoindex( *page_requested, path2file, c, _server );  return;     }
        c->setResponseString(OK, page_requested->location_path, _server->getServerRoot()); return ;
    }
	c->setResponseString(OK, file_path, _server->getServerRoot());
};

void Webserv::POSTmethod(Client *c) const
{
    std::string         pwd(getenv("PWD"));
    std::string         file_path;
    ServerInfo*         _server = c->getServerInfo();
    Request             req = c->getRequest();
    Cgi                 cgi;
    std::vector<page>   pages = _server->getPages();
    std::vector<page>::iterator page_requested = pages.begin();

    for ( ; page_requested != pages.end(); page_requested++ )                                                   // check for location in config
    {
        if ((*page_requested).root.size())                                                                      // we look for the cgi root folder
        {
            file_path = pwd + (*page_requested).root;                                                             
            if ( access((file_path + req.get_location()).c_str(), F_OK) != -1 )                                 // if the location required exists
            {                                                                                                   
                if ( invalidMethod(*page_requested, "POST") ) 
                {    c->setResponseString(METHOD_NOT_ALLOWED, "", ""); return  ;   }
                break ;
            }
        }
    }
    if ( page_requested == pages.end() )
    {    c->setResponseString(NOT_FOUND, "", "");  return ;       }  
    if (cgi.isCGI_request(c))
	{   std::cout << "POST request for CGI!" << std::endl; c->setNoFileToSend(true); exit(1); return ;        }
    c->setResponseString(BAD_GATEWAY, "", "");
};

void Webserv::DELETEmethod(Client *c) const
{
    std::string         pwd(getenv("PWD"));
    std::string         file_path;
    ServerInfo*         _server = c->getServerInfo();
    Request             req = c->getRequest();
    std::vector<page>   pages = _server->getPages();
    std::vector<page>::iterator page_requested = pages.begin();
    int                 fileInFolder;

    for ( ; page_requested != pages.end(); page_requested++ )                               
    {
        if ((*page_requested).location_path.back() == '/')
            file_path += (*page_requested).location_path.substr(0, (*page_requested).location_path.find_last_of("\\/")) + req.get_location();
        fileInFolder = access((pwd + _server->getServerRoot() + file_path).c_str(), R_OK);
        if ( req.get_location().compare((*page_requested).location_path) == 0                   // if the page required is exactly as in config
                || fileInFolder > -1 )                                                          // or if it is found in a config folder                     
        {                                                                                                       
            if ( invalidMethod(*page_requested, "DELETE") )                                     // check for method
            {    c->setResponseString(METHOD_NOT_ALLOWED, "", ""); return  ;   }
            break ;
        }
    }
    if ( fileInFolder < 0 && page_requested == pages.end() )
    {    c->setResponseString(NOT_FOUND, "", "");  return ;           }
    if ( remove((pwd + _server->getServerRoot() + file_path).c_str()) != 0 )
    {   c->setResponseString(UNAUTHORIZED, "", ""); return  ;   }
    c->setResponseString(OK, "File successfully deleted\n", "");
    c->setNoFileToSend(true);
};

// SIGNALS
void signal_handler(int signum)
{
    if (signum == SIGINT)
        keep_alive = false;
    std::cout << "Received SIGINT signal\n";
    std::cout << "Shutting down gracefully ...\n";
}

// UTILS
int     Webserv::invalidMethod(page page, std::string method)   const
{
    std::vector<std::string>::iterator method_it = std::find(page.methods.begin(),
                                                            page.methods.end(), method);
    if ( method_it == page.methods.end() )
        return 1;
    return 0;
}

int     Webserv::isDirectory( std::string path2file )    const
{
    struct stat         check_file;

    if ( !stat(path2file.c_str(), &check_file)   // if path exists
                && (check_file.st_mode & S_IFDIR )      )
        return 1;    
    return 0;
}

void     Webserv::checkAutoindex( page page, std::string path2file, Client *c, ServerInfo* _server ) const
{
    if ( page.autoindex == "on" )
    {
        std::cout << "Autoindex is on for " << page.location_path << std::endl;
        struct dirent *dir_list;
        std::string response;

        response = "<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'> \
                    <title> Index of" + page.location_path + "</title></head><body>"; 
        response += "<h1>Index of " + page.location_path + "</h1><hr><div><ul style=\"list-style: none;padding: 0;\">";

        DIR *dir = opendir(path2file.c_str());
        if ( dir == NULL )
        {   c->setResponseString(UNAUTHORIZED, "", "");  return ;   }
        while ( (dir_list = readdir(dir)) )    {
            std::string item(dir_list->d_name);
            std::string href = item;
            if ( item == "." )          {   href = path2file.substr(path2file.find_last_of("\\/"), path2file.size());  }
            else if ( item == ".." )    {   href = path2file.substr(0, path2file.find_last_of("\\/"));  }
            else if ( item[0] != '/' )  {   href.insert(0, "/");     }
            response += HREF_BEGIN + href + "\">" + item + HREF_END;
        }
        response += "</ul></div></body></html>";
        c->setNoFileToSend(true);
        c->setResponseString(OK, response, ""); return ;
    }
    c->setResponseString(OK, _server->getServerIndex(), _server->getServerRoot());  return ;
}

