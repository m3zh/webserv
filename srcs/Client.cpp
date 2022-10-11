/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 19:30:29 by artmende          #+#    #+#             */
/*   Updated: 2022/10/11 15:58:58 by artmende         ###   ########.fr       */
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
    // CALL PARSE REQUEST FCT FROM REQUEST CLASS
    request.parse_raw_request();
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
        response_str += "Content-Type: text/html; charset=utf-8;\r\n";
        response_str += "Content-Length: " + std::to_string(content.size()) + "\r\n";
        response_str += "\r\n\r\n";
        response_str += content;                               // we append the message we got from the python script
        file = "";                                              // we set the file to "" ( there is no file to send )
        std::cout << "STR RES: " << response_str; 
        setResponseFile(file); 
        setThereIsAFileToSend(false);  return ;                                           
    }
    else if( code == "200" )
        file = file_path;
    response_str += "Content-Type: " + getContentType(file) + "; charset=utf-8;\r\n";
    response_str += "Content-Length: " + getFileSize(file) + ";\r\n\r\n";             // file size !!!
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
    struct stat st;
    stat(file.c_str(), &st);
    off_t size = st.st_size; 
    return std::to_string(size);
}