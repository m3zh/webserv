/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 16:13:54 by artmende          #+#    #+#             */
/*   Updated: 2022/10/05 10:43:49 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <stdio.h>
#include <map>

# define OK	                    "200"
# define MOVED_PERMANENTLY	    "301"			
# define BAD_REQUEST		    "400"
# define UNAUTHORIZED		    "401"	
# define FORBIDDEN			    "403"
# define NOT_FOUND			    "404"
# define METHOD_NOT_ALLOWED	    "405"
# define LENGTH_REQUIRED	    "411"
# define REQUEST_URI_TOO_LONG   "414"
# define NOT_IMPLEMENTED		"501"
# define BAD_GATEWAY    		"502"	
# define HTTP_VERSION_NOT_SUPPORTED "505"

# define ERROR_FILE_PATH    "/www/error-pages"
# define HREF_BEGIN         "<li><a href=\""
# define HREF_END           "</a></li>\n"


// ************
// HTTP HEADERS 
// ************

// information are sent to the HTTP response header
// https://developer.mozilla.org/en-US/docs/Glossary/Response_header


class Response
{
    private:
        std::string     _code;
        std::string     _response;
        std::string     _response_file;
        
        Response &  operator=(Response const & r);
    public:
        Response(std::string code, std::string response_file);
        Response(Response const & r);
        ~Response();

        // GETTERS
        std::string     getCode() const;
        std::string     getResponse() const;
        std::string     getResponseFile() const;

        // SETTERS
        void            setCode(std::string code);
        void            setResponse(std::string code);
        void            setResponseFile(std::string files);
};
