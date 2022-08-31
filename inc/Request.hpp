/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 16:04:46 by artmende          #+#    #+#             */
/*   Updated: 2022/08/31 16:41:12 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <map>

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

    std::map<std::string, std::string>  _header_map;

    void    _fill_header_map(std::string raw_request)
    {
        std::remove(raw_request.begin(), raw_request.end(), '\r'); // remove all \r occurences from the string
        raw_request.erase(0, 1 + raw_request.find('\n')); // delete the first line, as it's not part of the header
        raw_request.erase(raw_request.find("\n\n"), std::string::npos); // delete all that is after the header

        std::stringstream   temp_stream;
        temp_stream.str(raw_request);

        while (temp_stream.good()) // gonna be false if error or when EOF is reached
        { // each line has format KEY: VALUE | Same Key can be encountered more than once
            std::string temp_string;
            std::getline(temp_stream, temp_string);

            size_t  index_of_colon = temp_string.find(':');
            std::string key(temp_string, 0, index_of_colon);
            temp_string.erase(0, index_of_colon + 2); // value is all that's left in temp_string

            if ((this->_header_map[key]).size() == 0)
                this->_header_map[key] = temp_string;
            else
                (this->_header_map[key] += ';') += temp_string;
        }
    }

    Request();
    Request(Request const & x);
    Request &   operator=(Request const & x);
public:

    Request(std::string raw_request)
    {
        this->_fill_header_map(raw_request);

        std::remove(raw_request.begin(), raw_request.end(), '\r');
        this->_request_stream.str(raw_request);
        this->_request_stream >> this->_method; // GET / HTTP/1.1
        this->_request_stream >> this->_location;
        this->_request_stream >> this->_http_version;

  //      std::string str(this->_request_stream.str().c_str(), strstr(this->_request_stream.str().c_str(), "\n\n"))


        //std::cout << "nbr of char is : " << (this->_request_stream.str().begin() + this->_request_stream.str().find("\n")) - (this->_request_stream.str().begin() + this->_request_stream.str().find("\n\n")) << std::endl;
        //std::cout << "nbr of char is : " << (-1 + this->_request_stream.str().find("\n\n") - this->_request_stream.str().find("\n")) << std::endl;

        //this->_header.assign(this->_request_stream.str().c_str(), 1 + this->_request_stream.str().find("\n"), -1 + this->_request_stream.str().find("\n\n") - this->_request_stream.str().find("\n"));

  //      strstr(this->_request_stream.str().c_str(), "\n\n");

    }
    ~Request() {}


    std::map<std::string, std::string> &    get_header_map()
    {
        return (this->_header_map);
    }

};
