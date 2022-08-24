/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 16:04:46 by artmende          #+#    #+#             */
/*   Updated: 2022/08/24 16:18:11 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

class Request
{
private:
    std::string _header;
    std::string _body;
    std::string _method;
    std::string _location;
    int         _content_length;
public:
    Request();
    Request(Request const & x);
    Request(std::string raw_request);
    ~Request();

    Request &   operator=(Request const & x);
};
