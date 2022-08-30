/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 16:04:46 by artmende          #+#    #+#             */
/*   Updated: 2022/08/30 16:45:37 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>

class Request
{
private:
public:
    std::string         _method;
    std::string         _location;
    std::string         _http_version;
    std::string         _content_type;
    int                 _content_length;
    std::string         _header;
    std::string         _body;

    std::stringstream   _request_stream;
public:
    Request() {}
    Request(Request const & x) {(void)x;}
    Request(std::string raw_request)
    {
        std::remove(raw_request.begin(), raw_request.end(), '\r');
        this->_request_stream.str(raw_request);
        this->_request_stream >> this->_method; // GET / HTTP/1.1
        this->_request_stream >> this->_location;
        this->_request_stream >> this->_http_version;

  //      std::string str(this->_request_stream.str().c_str(), strstr(this->_request_stream.str().c_str(), "\n\n"))


        std::cout << "nbr of char is : " << (this->_request_stream.str().begin() + this->_request_stream.str().find("\n")) - (this->_request_stream.str().begin() + this->_request_stream.str().find("\n\n")) << std::endl;
        std::cout << "nbr of char is : " << (-1 + this->_request_stream.str().find("\n\n") - this->_request_stream.str().find("\n")) << std::endl;

        this->_header.assign(this->_request_stream.str().c_str(), 1 + this->_request_stream.str().find("\n"), -1 + this->_request_stream.str().find("\n\n") - this->_request_stream.str().find("\n"));

  //      strstr(this->_request_stream.str().c_str(), "\n\n");

    }
    ~Request() {}

    Request &   operator=(Request const & x) {(void)x;return *this;}
};
