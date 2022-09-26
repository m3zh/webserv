/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 12:37:09 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/26 15:52:37 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Response.hpp"

Response::Response(int code, std::string response) :    _code(code),
                                                        _response(response) {};
Response::Response(Response const & r)              {
                                                        _code = r.getCode();
                                                        _response = r.getResponse();
                                                    };
Response::~Response()                               {};

int             Response::getCode()     const       {   return _code;         };
std::string     Response::getResponse() const       {   return _response;     };