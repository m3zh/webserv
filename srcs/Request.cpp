/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 16:10:50 by artmende          #+#    #+#             */
/*   Updated: 2022/09/26 12:18:16 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Request.hpp"

Request::Request()                                  {};
Request::Request(std::string const& raw_request)    :   _raw_request(raw_request) {};
Request &Request::operator=(Request const & r)      {
                                                        _raw_request = r._raw_request; // this points to the request untouched
                                                        _method = r._method;
                                                        _location = r._location;
                                                        _http_version = r._http_version;
                                                        _header_map.insert(r._header_map.begin(), r._header_map.end());
                                                        _index_beginning_body = r._index_beginning_body;
                                                        return *this;
                                                    };
Request::Request(Request const & r)                 {
                                                        _raw_request = r._raw_request; // this points to the request untouched
                                                        _method = r._method;
                                                        _location = r._location;
                                                        _http_version = r._http_version;
                                                        _header_map.insert(r._header_map.begin(), r._header_map.end());
                                                        _index_beginning_body = r._index_beginning_body;
                                                    };
Request::~Request()                                 {};


// PARSING
void    Request::_remove_char_from_string(std::string & str, char c)
{
    size_t  return_of_rfind = std::string::npos;
    while (std::string::npos != (return_of_rfind = str.rfind(c, return_of_rfind))) // delete char from the end to minimise copy operations
        str.erase(return_of_rfind, 1);
}

void    Request::_get_method_location_version(std::string raw_request)
{
    _remove_char_from_string(raw_request, '\r');
    std::stringstream   temp_stream;
    temp_stream.str(raw_request);
    temp_stream >> _method;
    temp_stream >> _location;
    temp_stream >> _http_version;
}

void    Request::_fill_header_map(std::string raw_request)
{
    _remove_char_from_string(raw_request, '\r');

    std::stringstream   temp_stream;
    temp_stream.str(raw_request);

    while (temp_stream.good())
    { // each line has format KEY: VALUE | Same Key can be encountered more than once
        std::string temp_string;
        std::getline(temp_stream, temp_string);
        size_t  index_of_colon = temp_string.find(':');
        if (index_of_colon == std::string::npos) // means there is no : in the line retrieved. Typically it is an empty line
            continue;
        
        std::string key(temp_string, 0, index_of_colon);
        std::string val(temp_string, index_of_colon + 2);
        _header_map.insert(std::make_pair(key, val));
    }
}

void    Request::parse_raw_request() // That function is meant to be called once the header of the request has been read.
{
    _get_method_location_version(_raw_request.substr(0, _raw_request.find('\n')));

    _index_beginning_body = std::string::npos;
    if (4 + _raw_request.find("\r\n\r\n") < _raw_request.size())
        _index_beginning_body =  4 + _raw_request.find("\r\n\r\n");

    size_t  index_beginning_header = 1 + _raw_request.find('\n');
    _fill_header_map(_raw_request.substr(index_beginning_header, _index_beginning_body));
}

// GETTERS
std::string     Request::get_raw_request() const                            {    return _raw_request;           };
std::string     Request::get_method() const                                 {    return _method;                };
std::string     Request::get_location() const                               {    return _location;              };
std::string     Request::get_http_version() const                           {    return _http_version;          };
std::map<std::string, std::string>     Request::get_header_map() const      {    return _header_map;            };
size_t          Request::get_index_beginning_body() const                   {    return _index_beginning_body;  };
std::string     Request::get_body() const
{    
    if (_index_beginning_body != std::string::npos) // it means there is a body
    {   std::string char2string(&_raw_request.c_str()[_index_beginning_body]); return char2string;   }
    return NULL;
}