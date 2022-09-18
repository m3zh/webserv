/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 16:04:46 by artmende          #+#    #+#             */
/*   Updated: 2022/09/18 12:07:47 by artmende         ###   ########.fr       */
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
    std::string const &                 _raw_request; // this points to the request untouched
    std::string                         _method;
    std::string                         _location;
    std::string                         _http_version;
    std::map<std::string, std::string>  _header_map;
    size_t                              _index_beginning_body; // = std::string::npos in case there is no body

    void    _remove_char_from_string(std::string & str, char c)
    {
        size_t  return_of_rfind = std::string::npos;
        while (std::string::npos != (return_of_rfind = str.rfind(c, return_of_rfind))) // delete char from the end to minimise copy operations
        {
            str.erase(return_of_rfind, 1);
        }
    }

    void    _get_method_location_version(std::string raw_request)
    {
        this->_remove_char_from_string(raw_request, '\r');
        std::stringstream   temp_stream;
        temp_stream.str(raw_request);
        temp_stream >> this->_method;
        temp_stream >> this->_location;
        temp_stream >> this->_http_version;
    }

    void    _fill_header_map(std::string raw_request)
    {
        this->_remove_char_from_string(raw_request, '\r');

        std::stringstream   temp_stream;
        temp_stream.str(raw_request);

        while (temp_stream.good()) // gonna be false if error or when EOF is reached
        { // each line has format KEY: VALUE | Same Key can be encountered more than once
            std::string temp_string;
            std::getline(temp_stream, temp_string);
            size_t  index_of_colon = temp_string.find(':');
            if (index_of_colon == std::string::npos) // means there is no : in the line retrieved. Typically it is an empty line
                continue;
            std::string key(temp_string, 0, index_of_colon);
            temp_string.erase(0, index_of_colon + 2); // value is all that's left in temp_string

            if ((this->_header_map[key]).size() == 0) // That key could have been already present in the map, if that's the case, we must not delete previous data
                this->_header_map[key] = temp_string;
            else
                (this->_header_map[key] += ';') += temp_string;
        }
    }

    Request();
    Request(Request const & x);
    Request &   operator=(Request const & x);
public:

    Request(std::string const & raw_request) : _raw_request(raw_request) {}
    ~Request() {}

    void    parse_raw_request() // That function is meant to be called once the request has been read in full. 
    {
        this->_get_method_location_version(this->_raw_request.substr(0, this->_raw_request.find('\n')));

        this->_index_beginning_body = (4 + this->_raw_request.find("\r\n\r\n") >= this->_raw_request.size() ? std::string::npos : 4 + this->_raw_request.find("\r\n\r\n"));

        size_t  index_beginning_header = 1 + this->_raw_request.find('\n');
        this->_fill_header_map(this->_raw_request.substr(index_beginning_header, this->_index_beginning_body - index_beginning_header));
    }

    std::string const & get_raw_request() const
    {
        return (this->_raw_request);
    }

    std::string const &   get_method() const
    {
        return (this->_method);
    }

    std::string const &   get_location() const
    {
        return (this->_location);
    }

    std::string const &   get_http_version() const
    {
        return (this->_http_version);
    }

    std::map<std::string, std::string> const &    get_header_map() const
    {
        return (this->_header_map);
    }
    size_t  get_index_beginning_body() const
    {
        return (this->_index_beginning_body);
    }
    const char    *get_body() const
    {
        if (this->_index_beginning_body != std::string::npos) // it means there is a body
            return (&(this->_raw_request.c_str()[this->_index_beginning_body]));
        else
            return (NULL);
    }
};
