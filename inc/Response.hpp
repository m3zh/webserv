/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 16:13:54 by artmende          #+#    #+#             */
/*   Updated: 2022/09/20 16:18:30 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

// Should be an attribute of the Client class
// This class shall contain :
    // The header of the response
    // A pointer to the data to be sent
    // Some way to know what has been sent so far


class Response
{
private:
    /* data */
public:
    Response() {}
    Response(Response const & x) {}
    ~Response() {}

    Response &  operator=(Response const & x) {return *this;}
};
