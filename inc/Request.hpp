/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 16:04:46 by artmende          #+#    #+#             */
/*   Updated: 2022/08/31 18:39:16 by artmende         ###   ########.fr       */
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
    std::stringstream                   _raw_request; // this contains the request untouched
    std::string                         _method;
    std::string                         _location;
    std::string                         _http_version;
    std::map<std::string, std::string>  _header_map;
    std::stringstream                   _body;

    void    _get_method_location_version(std::string raw_request)
    {
        std::stringstream   temp_stream;
        temp_stream.str(raw_request);
        temp_stream >> this->_method;
        temp_stream >> this->_location;
        temp_stream >> this->_http_version;
    }

    void    _fill_header_map(std::string raw_request)
    {
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

    void    _fill_body_stream(std::string raw_request)
    {
        std::string body(raw_request, raw_request.find("\n\n") + 2);
        this->_body.str(body);
    }


    Request();
    Request(Request const & x);
    Request &   operator=(Request const & x);
public:

    Request(std::string raw_request)
    {
        this->_raw_request.str(raw_request);
        std::remove(raw_request.begin(), raw_request.end(), '\r'); // remove all \r occurences from the string
        this->_get_method_location_version(raw_request);
        this->_fill_header_map(raw_request);
        this->_fill_body_stream(raw_request);
    }
    ~Request() {}

    std::stringstream & get_raw_request()
    {
        return (this->_raw_request);
    }

    std::string &   get_method()
    {
        return (this->_method);
    }

    std::string &   get_location()
    {
        return (this->_location);
    }

    std::string &   get_http_version()
    {
        return (this->_http_version);
    }

    std::map<std::string, std::string> &    get_header_map()
    {
        return (this->_header_map);
    }

    std::stringstream & get_body()
    {
        return (this->_body);
    }
};
