/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebException.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 15:28:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/23 17:51:39 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <typeinfo>
# include <string>
# include <string.h>
# include <sstream>

#if defined(__unix__)
    #define _NOEXCEPT noexcept
#endif

# define GREEN  "\e[92m"
# define RED	"\033[31m"
# define BLUE	"\033[34m"
# define RESET  "\033[0m"

template< typename T >
class WebException : public std::exception
{
    private:
        std::string   _err;

    public:
        WebException(const char* colour, const char*   message, T var)  {   
                                                                std::string clr(colour);
                                                                std::string msg(message);
                                                                std::stringstream ss;
                                                                ss << var;
                                                                _err = clr + msg + ": " + ss.str() + RESET;
                                                            };
        virtual ~WebException() _NOEXCEPT                   {};
        virtual const char* what() const _NOEXCEPT          {   return _err.c_str();    };
};
