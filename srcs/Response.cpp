/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 12:37:09 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/27 16:47:54 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Response.hpp"

Response::Response(int code, std::string response_file) :    
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
void            Response::setResponse(int code)                 
{   
    _response = res;            
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