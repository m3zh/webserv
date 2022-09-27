/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 12:37:09 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/27 17:59:22 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Response.hpp"

Response::Response(std::string  code, std::string response_file) :    
                                                    _code(code),
                                                    _response_file(response_file) {};
Response::Response(Response const & r)              {
                                                        _code = r.getCode();
                                                        _response = r.getResponse();
                                                    };
Response::~Response()                               {};

int             Response::getCode()     const       {   return _code;               };
std::string     Response::getResponse() const       {   return _response;           };
std::string     Response::getResponseFile() const   {   return _response_file;      };

void            Response::setCode(int code)                     {   _code = code;               };
void            Response::setResponse(std::string code)                 
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
    status["414"] = "Request URI Too Long";
    status["501"] = "Not Implemented";
    status["504"] = "Gateway Timeout";
    status["505"] = "HTTP Version Not Supported";
    
    _response = "HTTP/1.1 " + code + " " + status[code];
    if ( code == "301" )
        _response
    
    if ( code > "301" )     {
        file += ERROR_PAGES_PATH + "HTTP" + code + ".html";
        setResponseFile(file)
    }
    _response += "\r\n\r\n";           
};
void            Response::setResponseFile(std::string file)     {   _response_file = file;      };

// ************
// HTTP HEADERS functions
// ************

// information are sent to the HTTP response header
// https://developer.mozilla.org/en-US/docs/Glossary/Response_header
void    Response::http_header()
{
    std::cout << "200 OK\n";
    std::cout << "Content-Type: text/html; charset=utf-8;";
    std::cout << "Set-Cookie: Cookies are set\r\n\r\n";
}

void    Response::redirect_http_header(std::string loc)
{
    std::cout << "Status:308 Permanent redirect\nLocation: ";
    std::cout << loc;
    std::cout << "\r\n\r\n";
}