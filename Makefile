SRC = src/Authentication.cpp src/Channel.cpp src/Client.cpp src/join.cpp src/main.cpp src/PrivMsg.cpp src/Server.cpp src/Kick.cpp src/topic.cpp src/invite.cpp src/mode.cpp src/part.cpp

OBJ_DIR = obj

OBJ = $(patsubst %, $(OBJ_DIR)/%, $(notdir $(SRC:.cpp=.o)))

INC_DIR = inc

FLGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address

NAME = ircserv

all : $(NAME)

$(NAME) : $(OBJ)
	c++ $(FLGS) $^ -o $@

$(OBJ_DIR)/%.o : src/%.cpp $(INC_DIR)
	mkdir -p $(OBJ_DIR)
	c++ $(FLGS) -I$(INC_DIR) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -rf $(NAME)

re  : clean fclean all

.PHONY : all clean fclean re

.SILENT : $(NAME) $(OBJ) clean fclean re all
