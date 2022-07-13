
NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -g #-std=c++98 -g

SRCS	=	srcs/main.cpp		\
			srcs/Parser.cpp		\										
			srcs/Lexer.cpp		\									
			srcs/AToken.cpp	\

INC		=	inc/Parser.hpp		\
			inc/Lexer.hpp		\
			inc/AToken.hpp		\

all: $(NAME)

$(NAME): $(SRCS) $(INC)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

clean: fclean

fclean:
	@rm -f $(NAME)

re: fclean all

.PHONY: re clean fclean all