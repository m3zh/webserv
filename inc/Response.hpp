/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 16:13:54 by artmende          #+#    #+#             */
/*   Updated: 2022/09/26 16:04:21 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

# define OK	                200	
# define CREATED	        201	
# define ACCEPTED           202	
# define NO_CONTENT         204	
# define RESET_CONTENT 		205	
# define PARTIAL_CONTENT 	206	 
# define MOVED_PERMANENTLY	301			
# define BAD_REQUEST		400
# define UNAUTHORIZED		401	
# define FORBIDDEN			403	//GET: OK
# define NOT_FOUND			404	//GET: OK
# define METHOD_NOT_ALLOWED	405	//GET: OK
# define NOT_ACCEPTABLE		406	
# define PROXY_AUTHENTICATION_REQUIRED 407	
# define REQUEST_TIMEOUT    408	
# define CONFLICT			409	
# define GONE				410	
# define LENGTH_REQUIRED	411		
# define REQUEST_URI_TOO_LONG   414
# define UNSUPPORTED_MEDIA_TYPE 415	
# define EXPECTATION_FAILED	    417	
# define INTERNAL_SERVER_ERROR  500	
# define NOT_IMPLEMENTED		501	//OK
# define BAD_GATEWAY			502	
# define SERVICE_UNAVAILABLE	503	
# define GATEWAY_TIMEOUT		504	
# define HTTP_VERSION_NOT_SUPPORTED 505	//OK
# define METHOD_NOT_SUPPORTED   600	//OK


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
