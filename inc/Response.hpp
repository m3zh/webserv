/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 16:13:54 by artmende          #+#    #+#             */
/*   Updated: 2022/09/27 16:46:48 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

# define OK	                200	
# define CREATED	        201	
# define ACCEPTED           202	 
# define MOVED_PERMANENTLY	301			
# define BAD_REQUEST		400
# define UNAUTHORIZED		401	
# define FORBIDDEN			403
# define NOT_FOUND			404
# define METHOD_NOT_ALLOWED	405
# define REQUEST_URI_TOO_LONG   414
# define NOT_IMPLEMENTED		501
# define GATEWAY_TIMEOUT		504	
# define HTTP_VERSION_NOT_SUPPORTED 505


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
        std::string _response_file;
        
        Response &  operator=(Response const & r);
    public:
        Response(int code, std::string response_file);
        Response(Response const & r);
        ~Response();

        // GETTERS
        int             getCode() const;
        std::string     getResponse() const;
        std::string     getResponseFile() const;

        // SETTERS
        void            setCode();
        void            setResponse();
        void            setResponseFile();
};
