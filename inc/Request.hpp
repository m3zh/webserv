/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 16:04:46 by artmende          #+#    #+#             */
/*   Updated: 2022/09/20 14:06:53 by artmende         ###   ########.fr       */
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
        std::string                         _raw_request; // this points to the request untouched
        std::string                         _method;
        std::string                         _location;
        std::string                         _http_version;
        std::map<std::string, std::string>  _header_map;
        size_t                              _index_beginning_body; // = std::string::npos in case there is no body

        void    _remove_char_from_string(std::string & str, char c);
        void    _get_method_location_version(std::string raw_request) const;
        void    _fill_header_map(std::string raw_request) const;

        Request();
        Request(Request const & x);
        Request &   operator=(Request const & x);

    public:
        Request(std::string & raw_request) : _raw_request(raw_request) {}
        ~Request() {}

        void    parse_raw_request() const;

        std::string const & get_raw_request() const;
        std::string const &   get_method() const;
        std::string const &   get_location() const;
        std::string const &   get_http_version() const;
        std::map<std::string, std::string> const &    get_header_map() const;
        size_t  get_index_beginning_body() const;
        const char    *get_body() const;
};
