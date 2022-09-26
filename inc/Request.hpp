/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 16:04:46 by artmende          #+#    #+#             */
/*   Updated: 2022/09/26 12:06:08 by mlazzare         ###   ########.fr       */
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
        void    _get_method_location_version(std::string raw_request);
        void    _fill_header_map(std::string raw_request);  

    public:
        Request();
        Request(Request const & x);
        Request &operator=(Request const & x);
        Request(std::string const &raw_request);
        ~Request();

        void    parse_raw_request();
        void    handleRequest();

        std::string                             get_raw_request() const;
        std::string                             get_method() const;
        std::string                             get_location() const;
        std::string                             get_http_version() const;
        std::map<std::string, std::string>      get_header_map() const;
        size_t                                  get_index_beginning_body() const;
        std::string                             get_body() const;
};
