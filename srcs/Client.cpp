/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:30:29 by artmende          #+#    #+#             */
/*   Updated: 2022/10/14 14:28:23 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client( int client_socket, 
                struct sockaddr_in client_addrs,
                ServerInfo *associated_server)  :   client_socket(client_socket),
                                                    client_addrs(client_addrs),
                                                    associated_server(associated_server),
                                                    //request_str(""),
                                                    request(request_str),
                                                    FileToSend(false)           {
                                                                                    setReadAsComplete(false);
                                                                                    setHeaderReadAsComplete(false);
                                                                                    setThereIsAFileToSend(false);
                                                                                    setHeaderBeenSent(false);
                                                                                    setIsNotCgi(true);
                                                                                };
Client::~Client()                                                               {};


void                    Client::parseHeader()
{
    request.parse_raw_request();
    // if ((*it)->headerIsReadComplete() == true && (*it)->getRequest().get_header_map().find("Content-Length") == (*it)->getRequest().get_header_map().end())

    std::map<std::string, std::string>::const_iterator    content_length_it = request.get_header_map().find("Content-Length");

    if (content_length_it == request.get_header_map().end()) // there is no body in the request
    {
        setReadAsComplete(true);
        request.set_index_beginning_body(std::string::npos);
    }
    else // there is a body in the request. Need to see if we got all of it yet
    {
        if (request_str.size() - request.get_index_beginning_body() >= (unsigned long)(atoi((content_length_it->second).c_str()))) // means all the body is there already
            setReadAsComplete(true);
    }
                // issue : We might have both header and full body in the first call to recv(). In that case, we need to mark the reading as finished here directly, because select() won't give us that same client again
                // need a function to check if there is a content length and to check if all of it has been read yet
                // condition to mark as read complete here : 
                //  - There is no body (we can know that by the absence of Content-Length in the header map)
                //  - There is a body that has been fully read in the first call to recv() (There is a Content-Length in the header map, and it matches what we actually received)
                // important : If there is no body, we need to set the index beginning body to npos
}

// GETTERS
int                     Client::getClientSocket()                       const   {   return  client_socket;          };
struct sockaddr_in      Client::getClientAddress()                      const   {   return  client_addrs;           };
ServerInfo *            Client::getServerInfo()                         const   {   return  associated_server;      };
std::string const &             Client::getRequestString()                      const   {   return  request_str;            };
Request const &                 Client::getRequest()                            const   {   return  request;                };
std::string             Client::getResponseString()                     const   {   return  response_str;           };
std::string             Client::getResponseFile()                       const   {   return  response_file;          };

std::ifstream &         Client::getResponseFileStream()                         {    return response_file_stream;   };
std::string &           Client::getRemainingBufferToSend()                      {    return remaining_buffer_to_send;  };



// SETTERS
//void                    Client::setRequest(std::string const &s)                {   request = Request(s); request.parse_raw_request();       };
void                    Client::setIndexBeginningBodyInRequest(size_t index)    {this->request.set_index_beginning_body(index);}
void                    Client::appendToRequestString(char *str, ssize_t size)                 {   request_str.append(str, size);      };
// sets the right header for the response and set the right file to open ( as requested by Client )
void                    Client::setResponseString(std::string code, std::string content, std::string file_path)
{
    std::string file = getenv("PWD");
    std::map<std::string, std::string> status;
    status["200"] = "OK";
    status["301"] = "Moved Permanently";
    status["400"] = "Bad Request";
    status["401"] = "Unauthorized";
    status["403"] = "Forbidden";
    status["404"] = "Not Found";
    status["405"] = "Method Not Allowed";
    status["411"] = "Length Required";
    status["413"] = "Pay Load Too Large";
    status["414"] = "Request URI Too Long";
    status["501"] = "Not Implemented";
    status["504"] = "Gateway Timeout";
    status["505"] = "HTTP Version Not Supported";
    
    std::cout << "CODE: ";
    std::cout << code;
    std::cout << "\n";
    std::cout << content;
    std::string error_file(ERROR_FILE_PATH);
    response_str = "HTTP/1.1 " + code + " " + status[code] + "\r\n";        // start of header
    if ( code == "301" )
    {                                                    // if it is a redirection
        response_str += "Location: " + content + "\r\n\r\n";
        file = "";                                              // we set the file to "" ( there is no file to send )
        std::cout << "STR RES: " << response_str; 
        setResponseFile(file); 
        setThereIsAFileToSend(false);  return ;     
    }
    else if ( code > "301" )
        file += error_file + "/HTTP" + code + ".html";
    else if ( noFileToSend() )
    {
        std::stringstream ss;
        ss << content.size();
        response_str += "Content-Type: text/html; charset=utf-8;\r\n";
        response_str += "Content-Length: " + ss.str() + "\r\n";       // NEWLY ADDED, TO CHECK !!
        response_str += "\r\n\r\n";
        response_str += content;                               // we append the message we got from the python script
        file = "";                                              // we set the file to "" ( there is no file to send )
        std::cout << "STR RES: " << response_str; 
        setResponseFile(file); 
        setThereIsAFileToSend(false); return ;                                           
    }
    else if( code == "200" )
        file = file_path;
    response_str += "Content-Type: " + getContentType(file) + "; charset=utf-8;\r\n";
    response_str += "Content-Length: " + getFileSize(file) + ";\r\n\r\n";             // NEWLY ADDED, TO CHECK !!
    std::cout << "\nSTR RES: " << response_str;
    std::cout << "FILE: " << file << std::endl;
    setResponseFile(file);
    getResponseFileStream().open(file, std::ios_base::in | std::ios_base::binary);          // convert file to fstream
    setThereIsAFileToSend(true); 
};

void                    Client::setResponseFile(std::string file)       {   response_file = file;               };

// BOOLS
bool                    Client::isReadComplete()                        {   return  is_read_complete;            };
void                    Client::setReadAsComplete(bool state)           {   is_read_complete = state;            };
bool                    Client::headerIsReadComplete()                  {   return  header_is_read_complete;     };
void                    Client::setHeaderReadAsComplete(bool state)     {   header_is_read_complete = state;     };

bool                    Client::noFileToSend()                          {   return FileToSend;                   };
void                    Client::setNoFileToSend(bool state)             {   FileToSend = state;                  };

bool                    Client::headerHasBeenSent()                     {   return header_has_been_sent;         };
void                    Client::setHeaderBeenSent(bool state)           {   header_has_been_sent = state;        };
bool                    Client::thereIsAFileToSend()                    {   return there_is_a_file_to_send;      };
void                    Client::setThereIsAFileToSend(bool state)       {   there_is_a_file_to_send = state;     };
bool                    Client::isNotCgi()                              {   return is_not_cgi;      };
void                    Client::setIsNotCgi(bool state)                 {   is_not_cgi = state;     };

// UTILS


std::string           Client::getContentType(std::string file)
{
    std::string ret;
    std::string extension = file.substr(file.find_last_of(".") + 1, file.size());
    if ( extension == "html" || extension == "css" 
        || extension == "xml" || extension == "csv" )
    {    ret = "text/" + extension; return ret;     }
    if ( extension == "jpeg" || extension == "png" || extension == "gif" 
        || extension == "svg" || extension == "webp" || extension == "jpg" )
    {    ret = "image/" + extension; return ret;     }
    ret = "text/plain";
    return ret;
};

std::string           Client::getFileSize(std::string file)
{
    long size;
    FILE *f;
    std::stringstream ss;
 
    f = fopen(file.c_str(), "rb");
    if (f == NULL) return "0";
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    ss << size;
    fclose(f);
 
    return ss.str();
}