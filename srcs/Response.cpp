/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 12:37:09 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/27 18:26:02 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Response.hpp"

Response::Response(std::string  code, std::string response_file) :    
                                                    _code(code),
                                                    _response_file(response_file) {};
Response::Response(Response const & r)              {
                                                        _code = r.getCode();
                                                        _response = r.getResponse();
                                                    };
Response::~Response()                               {};

std::string     Response::getCode()     const       {   return _code;               };
std::string     Response::getResponse() const       {   return _response;           };
std::string     Response::getResponseFile() const   {   return _response_file;      };

void            Response::setCode(std::string code)             {   _code = code;               };
void            Response::setResponseFile(std::string file)     {   _response_file = file;      };