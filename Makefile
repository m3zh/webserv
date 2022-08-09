NAME 	 = webserv

CXX 	 = c++
CXXFLAGS = #-fsanitize=address#-Wall -Werror -Wextra -g -std=c++11

SRCS	=	srcs/main.cpp		\
			srcs/Token.cpp		\
			srcs/Lexer.cpp		\
			srcs/Config.cpp		\
			srcs/ServerInfo.cpp		\
			srcs/Webserv.cpp	\
			srcs/Cgi.cpp	

INC		=	inc/

OBJ = $(SRCS:%.cpp=%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -I$(INC) $(OBJ) -o $@

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: re clean fclean all
