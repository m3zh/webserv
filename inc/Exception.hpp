/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 15:28:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/19 15:21:08 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <typeinfo>
# include <string>
# include <string.h>
# include <sstream>

# define GREEN  "\e[92m"
# define RED	"\033[31m"
# define RESET  "\033[0m"

template< typename T >
class Exception : public std::exception
{
    private:
        std::string   _err;

    public:
        Exception(const char* colour, const char*   message, T var)  {   
                                                                std::string clr(colour);
                                                                std::string msg(message);
                                                                std::stringstream ss;
                                                                ss << var;
                                                                _err = clr + msg + ": " + ss.str();
                                                            };
        ~Exception() {};

    const char* what() const noexcept override
    {
        return _err.c_str();
    }
};
