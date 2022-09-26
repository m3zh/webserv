/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 16:13:54 by artmende          #+#    #+#             */
/*   Updated: 2022/09/26 12:49:05 by mlazzare         ###   ########.fr       */
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
        int         _code;
        std::string _response;
        
        Response &  operator=(Response const & r);
    public:
        Response(int code, std::string response);
        Response(Response const & r);
        ~Response();

        int             getCode() const;
        std::string     getResponse() const;
};
