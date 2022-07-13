NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -g #-std=c++98 -g

SRCS	=	srcs/main.cpp		\
			srcs/AToken.cpp		\
			srcs/Lexer.cpp									

INC		=	inc/AToken.hpp		\
			inc/Lexer.hpp

all: $(NAME)

$(NAME): $(SRCS) $(INC)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

clean: fclean

fclean:
	@rm -f $(NAME)

re: fclean all

.PHONY: re clean fclean all
