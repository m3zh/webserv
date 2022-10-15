/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/10/15 16:41:08 by artmende         ###   ########.fr       */
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
        if ((listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {   throw WebException<int>(BLUE, "WebServ error: socket failed on listening socket ", listening_socket); close_all();    return -1;      }
        // setting socket in non blocking mode, to allow it to connect to multiple clients ; instead of having a blocking socket restricted to one client
        if (fcntl(listening_socket, F_SETFL, O_NONBLOCK) < 0)
        {   throw WebException<int>(BLUE, "WebServ error: fcntl failed on listening socket ", listening_socket); close_all();    return -1;      }
        // setting socket options to allow it to reuse local address (even after a client disconnects)
        if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0)
        //if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        {   throw WebException<int>(BLUE, "WebServ error: setsockopt failed on listening socket ", listening_socket); close_all();    return -1;      }
        struct sockaddr_in  listening_addrs;
        // we use memset to initialize a sockaddr_in structure
        memset(&listening_addrs, 0, sizeof(listening_addrs));
        listening_addrs.sin_family = AF_INET;
        // adding port
        listening_addrs.sin_addr.s_addr = inet_addr("127.0.0.1");
        // convert host address to network bytes address
        listening_addrs.sin_port = htons((*it).getPort());
        // we bind the socket to a "file" descriptor => we get a socket descriptor
        if ((bind(listening_socket, (struct sockaddr *)&listening_addrs, sizeof(listening_addrs))) < 0)
        {   throw WebException<int>(BLUE, "WebServ error: bind failed on listening socket ", listening_socket); close_all();    return -1;      }
        // we launch the socket, with a maximum of 256 different sockets in the queue
        if ((listen(listening_socket, BACKLOG) < 0))
        {   throw WebException<int>(BLUE, "WebServ error: listening failed on listening socket ", listening_socket); close_all();    return -1;      }
        (*it).setListeningSocket(listening_socket);
        (*it).setListeningAddrs(listening_addrs);
    }
    return 0;
}

int     Webserv::run_server()
{
    struct timeval timeout;
    int return_of_select; // -1 means error, 0 means timeout
    
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
        FD_ZERO(&_write_set);
        _read_set = _current_set;
        _write_set = _current_set;

        // select will test listening fd, and clients
        return_of_select = select(1 + get_fd_max(), &_read_set, &_write_set, NULL, &timeout);
        std::cout << "just after select() with return " << return_of_select << std::endl;
        if (return_of_select == 0)
            throw WebException<int>(BLUE, "WebServ timeout: no activity for the last", timeout.tv_sec);
        if (return_of_select == -1)
        {
            if (keep_alive == false)
                throw WebException<int>(BLUE, "CTRL+C has been pressed. Shutting down with exit code", 1);
            throw WebException<int>(BLUE, "WebServ error: select failed with return", return_of_select);
        }
        // looping through all listening socket
        try
        {
            checking_for_new_clients(); // possible issue with accept()
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        looping_through_read_set(); // possible issue with recv() but no exception
        looping_through_write_set(); // possible issue with send() but no exception
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
            Client  *to_add = accept_new_client((*it).getListeningSocket());
            // in case an exception is thrown, that is where it will stop execution. No Client will be allocated, and no push_back in the client list
            _clients_list.push_back(to_add);
        }
    }
}

Client     *Webserv::accept_new_client(int listening_socket)
{
    // the returned client is allocated in the heap. Do not forget to deallocate it
    struct sockaddr_in  addr_of_client;
    socklen_t   len_for_accept = sizeof(addr_of_client);
    int client_socket;

    client_socket = accept(listening_socket, (struct sockaddr *)&addr_of_client, &len_for_accept);
    if (client_socket < 0)
        throw WebException<int>(RED, "WebServ error with function accept(): Client not accepted on listening socket ", listening_socket);
    std::cout << "new client accepted ! socket is " << client_socket << std::endl;      
    FD_SET(client_socket, &_current_set);

    Client *ret;// = new Client(client_socket, addr_of_client, get_server_associated_with_listening_socket(listening_socket));

    try
    {
        ret = new Client(client_socket, addr_of_client, get_server_associated_with_listening_socket(listening_socket));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        close(client_socket); // in case the memory allocation fails, we close the newly created socket.
        throw WebException<int>(RED, "WebServ memory allocation error : Client could not be added to Client list. Listening socket was ", listening_socket);
        // that second exception thrown will be caught in run_server()
    }
    return (ret);
}


// READING AND WRITING
void    Webserv::looping_through_read_set()
{
    // what if client still send something after request has been marked as fully read ?

    // This function loop through all clients that have been selected by select() for reading. There is only one call to recv() per call to select().
    // At each loop, we call recv() and append the data received to a request std::string that is inside of the Client class.
    // First we have to read the header, we know that the header has been fully read once there is "\r\n\r\n" in the request string.
    // Once the header has been fully read, we have to parse it. Specifically here we look for "Content-Length" in the header.
    // If it is there, it means the request fromt that client has a body . We then know how long that body is and we can keep calling recv() until we get all of it.
    // If there is no "Content-Length" in the header, the whole request consists of just the header.

    for (std::list<Client*>::iterator it = _clients_list.begin(); it != _clients_list.end(); ++it)
    {
        int client_socket = (*it)->getClientSocket();
        if (FD_ISSET(client_socket, &_read_set))
        {
            std::cout << "Reading...\n" << std::endl;
            char buffer[READ_WRITE_BUFFER];
            bzero(&buffer, sizeof(buffer));
            ssize_t bytes_recv;
            if ((*it)->headerIsReadComplete() == false) 
            {
                if ((bytes_recv = recv(client_socket, buffer, sizeof(buffer), 0)) <= 0)
                {
                    std::list<Client*>::iterator    to_delete = it;
                    ++it; // ready for next loop cycle
                    remove_client(client_socket, to_delete);
                    std::cout << "recv failed for client socket " << client_socket << std::endl;
                    continue;
                }
                (*it)->appendToRequestString(buffer, bytes_recv);
                if ((*it)->getRequestString().find("\r\n\r\n") != std::string::npos)
                    parseHeader(*it); // only do this if \r\n\r\n is found in request string

                std::cout << bytes_recv << " bytes of the header have been read.\n";

            }
            else if ((*it)->headerIsReadComplete()) // header has been read and parsed and there is more data to read (we have to check content-length)
            {
                std::map<std::string, std::string>  header_map = (*it)->getRequest().get_header_map();
                std::map<std::string, std::string>::const_iterator    content_length_it = header_map.find("Content-Length");
                if ((bytes_recv = recv(client_socket, buffer, sizeof(buffer), 0)) <= 0)
                {
                    std::list<Client*>::iterator    to_delete = it;
                    ++it; // ready for next loop cycle
                    remove_client(client_socket, to_delete);
                    std::cout << "recv failed for client socket " << client_socket << std::endl;
                    continue;     
                }
                (*it)->appendToRequestString(buffer, bytes_recv);
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
                    if ((bytes_sent = send(client_socket, (*it)->getRemainingBufferToSend().data(), (*it)->getRemainingBufferToSend().size(), 0)) <= 0)
                    {
                        std::list<Client*>::iterator    to_delete = it;
                        ++it; // ready for next loop cycle
                        remove_client(client_socket, to_delete);
                        std::cout << "send failed for client socket " << client_socket << std::endl;
                        continue;    
                    }
                    (*it)->getRemainingBufferToSend().erase(0, bytes_sent); // this will either clear the string, or leave there what was not sent yet
                    if ((*it)->getRemainingBufferToSend().size() == 0) // means it was all sent successfully
                        (*it)->setHeaderBeenSent(true);
                    ++it;
                    continue; // not more than 1 send() per select()
                }
                if ((bytes_sent = send(client_socket, (*it)->getResponseString().c_str(), (*it)->getResponseString().size(), 0)) <= 0)
                {
                    std::list<Client*>::iterator    to_delete = it;
                    ++it; // ready for next loop cycle
                    remove_client(client_socket, to_delete);
                    std::cout << "send failed for client socket " << client_socket << std::endl;
                    continue;
                }
                    
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
                    std::list<Client*>::iterator    to_delete = it;
                    ++it; // ready for next loop cycle
                    remove_client(client_socket, to_delete);
                    continue;
                }
                if ((*it)->getRemainingBufferToSend().size())
                {
                    if ((bytes_sent = send(client_socket, (*it)->getRemainingBufferToSend().data(), (*it)->getRemainingBufferToSend().size(), 0)) <= 0)
                    {
                        std::list<Client*>::iterator    to_delete = it;
                        ++it; // ready for next loop cycle
                        remove_client(client_socket, to_delete);
                        std::cout << "send failed for client socket " << client_socket << std::endl;
                        continue;
                    }
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
                    if ((bytes_sent = send(client_socket, buffer, effective_size_of_buffer, 0)) <= 0)
                    {
                        std::list<Client*>::iterator    to_delete = it;
                        ++it; // ready for next loop cycle
                        remove_client(client_socket, to_delete);
                        std::cout << "send failed for client socket " << client_socket << std::endl;
                        continue;
                    }
                    if (bytes_sent < effective_size_of_buffer)
                        (*it)->getRemainingBufferToSend().assign(&buffer[bytes_sent], effective_size_of_buffer - bytes_sent);
                }
                if ((*it)->getResponseFileStream().eof() == true
                    && (*it)->getRemainingBufferToSend().size() == 0) // the file has been fully transmitted
                {
                    std::list<Client*>::iterator    to_delete = it;
                    ++it; // ready for next loop cycle
                    remove_client(client_socket, to_delete);
                    continue;
                }
            }
        }
        ++it; // put it here instead of in the for loop declaration
    }
}

void    Webserv::remove_client(int client_socket, std::list<Client*>::iterator to_delete)
{
    close(client_socket);
    FD_CLR(client_socket, &_current_set);
    delete (*to_delete); // Client is allocated
    _clients_list.erase(to_delete);
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
    if (_clients_list.size() != 0)                                                              // if there are clients, get the max fd from them. If no clients, get the max listening fd
    {
        int fd_max = (*(_clients_list).begin())->getClientSocket();
        for (std::list<Client*>::const_iterator it = _clients_list.begin(); it != _clients_list.end(); ++it)
            if ((*it)->getClientSocket() > fd_max)
                fd_max = (*it)->getClientSocket();
        return fd_max;
    }
    else
        return (_servers.back().getListeningSocket());                                          // last added server has the biggest listening socket
        // fix this
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
    // CALL PARSE REQUEST FCT FROM CLIENT
    c->parseHeader();
    //c->setRequest(c->getRequestString());
    c->setHeaderReadAsComplete(true);
};

void    Webserv::handleRequest(Client *c)   const   {
    std::string method = c->getRequest().get_method();
    std::string uri = c->getRequest().get_location();
    std::string version = c->getRequest().get_http_version();
    if ( !method.size() || !uri.size() || !version.size() )
    {    c->setResponseString(BAD_REQUEST, "", "");                 return;     }
    if ( uri.size() > MAX_URI )
    {    c->setResponseString(REQUEST_URI_TOO_LONG, "", "");        return;     }
    if ( version != "HTTP/1.1" )
    {     c->setResponseString(HTTP_VERSION_NOT_SUPPORTED, "", ""); return ;    }
    std::map<std::string, std::string> header = c->getRequest().get_header_map(); 
    if (header.find("Content-Length") != header.end()
        && std::stoi((header.find("Content-Length")->second)) > c->getServerInfo()->getClientMaxBodySize())
        {  c->setResponseString(PAYLOAD_TOO_LARGE,"","");    return ;              };
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
    ServerInfo*         _server = c->getServerInfo();
    Request const &             req = c->getRequest();
    std::vector<page>   pages = _server->getPages();
    std::vector<page>::iterator page_requested = pages.begin();
    int                 redirect = 0;
    int                 type = 0;
    Cgi                 cgi;

    for ( ; page_requested != pages.end(); page_requested++ )                                   // check for location in config
    {
        if ((*page_requested).root.size())
            file_path = (*page_requested).root + req.get_location();
        else
            file_path = (*page_requested).location_path + req.get_location();
        if ( file_path.find("//") != std::string::npos )
            file_path.replace(file_path.find("//"),2,"/");
        type = isItFileLocationOrSubfolder(req, *page_requested, pwd + _server->getServerRoot() + file_path);
        if ( type )
        {
            if ( invalidMethod(*page_requested, "GET") )                                        // check for method
            {    c->setResponseString(METHOD_NOT_ALLOWED, "", ""); return  ;   }
            if ( type == LOCATION )
            {
                if ((*page_requested).redirect.size())                                          // check for redirection
                    redirect = 1;
                else if ((*page_requested).root.size())
                    file_path = pwd + _server->getServerRoot() + (*page_requested).root;
                else
                    file_path = pwd + _server->getServerRoot() + page_requested->location_path;
            }
            else
                file_path = pwd + _server->getServerRoot() + file_path;
            break ;
        }
    }
    if ( !type && page_requested == pages.end() )                                                   // if nothing is found 
    {   
        if (req.get_location().find("py") != std::string::npos 
            || req.get_location().find(".rb") != std::string::npos )  {                             // we check if it is a CGI request   
            if (cgi.isCGI_request(c)) {   std::cout << "GET request for CGI!" << std::endl;   return ;        }
            else
                return ;
        }
        c->setResponseString(NOT_FOUND, "", "");  return ;        
    }
    if ( redirect )         {	c->setResponseString(MOVED_PERMANENTLY, page_requested->redirect, "");      return  ;       }
    if ( type == FILE_IN_FOLDER )    {
        if (open((file_path).c_str(), O_RDONLY) < 0)            {    c->setResponseString(UNAUTHORIZED, "", "");  return ;  }
        c->setResponseString(OK, "", file_path); return ;
    }
    if ( type == LOCATION || type == SUBFOLDER )                                                        // if it is a directory, check for autoindex              
    {    checkAutoindex( *page_requested, file_path, c, _server, type );  return;                     }
};

void Webserv::POSTmethod(Client *c) const
{

    std::string         pwd(getenv("PWD"));
    std::string         file_path;
    std::string         req_location;
    ServerInfo*         _server = c->getServerInfo();
    Request const &             req = c->getRequest();
    std::vector<page>   pages = _server->getPages();
    std::vector<page>::iterator page_requested = pages.begin();
    Cgi                 cgi;

    req_location = req.get_location().substr(req.get_location().find_last_of("/"), req.get_location().size());
    for ( ; page_requested != pages.end(); page_requested++ )                                                   // check for location in config
    {
                
        if ((*page_requested).root.size())                                                                      // we look for the cgi root folder
        {
            file_path = pwd + (*page_requested).root;
            std::cout << file_path << std::endl;
            std::cout << req_location << std::endl;                                                             
            if ( access((file_path + req_location ).c_str(), F_OK) != -1 )                                      // if the location required exists
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
	{   std::cout << "POST request for CGI!" << std::endl; c->setNoFileToSend(true); return ;        }
};

void Webserv::DELETEmethod(Client *c) const
{
    std::string         pwd(getenv("PWD"));
    std::string         file_path;
    ServerInfo*         _server = c->getServerInfo();
    Request const &             req = c->getRequest();
    std::vector<page>   pages = _server->getPages();
    std::vector<page>::iterator page_requested = pages.begin();
    int                 type = 0;

    for ( ; page_requested != pages.end(); page_requested++ )                                   // check for location in config
    {
        if ((*page_requested).root.size())
            file_path = (*page_requested).root + req.get_location();
        else
            file_path = (*page_requested).location_path + req.get_location();
        if ( file_path.find("//") != std::string::npos )
            file_path.replace(file_path.find("//"),2,"/");
        type = isItFileLocationOrSubfolder(req, *page_requested, pwd + _server->getServerRoot() + file_path);
        if ( type )
        {
            if ( invalidMethod(*page_requested, "DELETE") )                                        // check for method
            {    c->setResponseString(METHOD_NOT_ALLOWED, "", ""); return  ;   }
            // if ( type == LOCATION )
            // {
            //     if ((*page_requested).root.size())
            //         file_path = pwd + _server->getServerRoot() + (*page_requested).root;
            //     else
            //         file_path = pwd + _server->getServerRoot() + page_requested->location_path;
            // }
            // else
                file_path = pwd + _server->getServerRoot() + file_path;
            break ;
        }
    }
    if ( !type && page_requested == pages.end() )                                                   // if nothing is found 
    {    c->setResponseString(NOT_FOUND, "", "");  return ;     }   
    if ( remove((file_path).c_str()) != 0 )
    {   c->setResponseString(UNAUTHORIZED, "", ""); return  ;   }
    c->setNoFileToSend(true);
    c->setResponseString(OK, "File successfully deleted\n", "");    
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

int     Webserv::isItFileLocationOrSubfolder( Request const& req, page page, std::string file )       const
{
    if ( req.get_location().compare(page.location_path) == 0 )                     // if it matches a location in config 
        return 1;
    if (isFileinFolder( file ))   return 2;                                        // if it's a file in folder             
    if (isDirectory( file ))      return 3;                                        // if it's a subfolder of a location   
    return 0;
}

int     Webserv::isDirectory( std::string path2file )    const
{
    struct stat         check_file;

    stat(path2file.c_str(), &check_file);                 // if path exists
    if (S_ISDIR(check_file.st_mode)      )
        return 1;    
    return 0;
}

int     Webserv::isFileinFolder( std::string path2file ) const
{
    struct stat         check_file;

    stat(path2file.c_str(), &check_file);                  // if path exists
    if( S_ISREG(check_file.st_mode)   )
        return 1;    
    return 0;
}

void     Webserv::checkAutoindex( page page, std::string file, Client *c, ServerInfo* _server, int type ) const
{
    if ( type == SUBFOLDER || page.autoindex == "on" )
    {
        std::cout << "Autoindex is on for " << page.location_path << std::endl;
        struct dirent *dir_list;
        std::string response;

        response = "<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'> \
                    <title> Index of" + c->getRequest().get_location() + "</title></head><body>"; 
        response += "<h1>Index of " + c->getRequest().get_location() + "</h1><hr><div><ul style=\"list-style: none;padding: 0;\">";
        
        DIR *dir = opendir(file.c_str());
        if ( dir == NULL )
        {   c->setResponseString(UNAUTHORIZED, "", "");  return ;   }
        while ( (dir_list = readdir(dir)) )    {
            std::string item(dir_list->d_name);
            std::string href = item;
            if ( item == "." )          {   href = c->getRequest().get_location();  }
            else if ( item == ".." )    {   
                if ( page.location_path == "/" )
                    href = page.location_path;
                else    {
                    href = file.erase(file.find(c->getRequest().get_location()), file.size()); 
                    href = file.substr(file.find_last_of("\\/", file.size()));
                } 
            }
            else if ( item[0] != '/' )  {   href.insert(0, "/");     }
            // check file or dir
            response += HREF_BEGIN + href + "\">" + item + HREF_END;
        }
        response += "</ul></div></body></html>;\n";
        closedir(dir);
        c->setNoFileToSend(true);
        c->setResponseString(OK, response, ""); std::cout << file << std::endl; return ;
    }
    else
        c->setResponseString(OK, "", getenv("PWD") + _server->getServerRoot() + "/" + _server->getServerIndex());
}

