# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aahlyel <aahlyel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/03 15:57:19 by aelbrahm          #+#    #+#              #
#    Updated: 2024/08/03 01:12:24 by aahlyel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#================================#
#         COLORS                 #
#================================#
BLU = \033[34;1m
CYN = \033[36;1m
PRP = \033[35;1m
GRN = \033[32;1m 
RED = \033[31;1m
YLW = \033[33;1m
C_CLS = \033[0m
#================================#
#         GLOBAL_VARIABLE        #
#================================#
CC = c++
FLAGS =  -Wall -Wextra -Werror
SRCS = $(shell ls | grep -E ".+\.cpp")
OBJS_O = $(SRCS:.cpp=.o)
NAME = bin/ircbot
OBJS = $(addprefix obj/, $(OBJS_O))
DIR = obj
RM = rm -rf
HEADRES = Bot.hpp

#================================#
#            RULES               #
#================================#
all : $(NAME)

$(NAME) : $(OBJS)
		mkdir -p bin
		$(CC) $(FLAGS)  $(OBJS) -o $(NAME) -lcurl

obj/%.o : ../bot/%.cpp $(HEADRES)
		mkdir -p $(dir $@)
		printf "|[ $(GRN)%-37s$(C_CLS)$(YLW) ]| \r" "Compailing... $(notdir $@)"
		$(CC) $(FLAGS) -c  $< -o $@

clean :
		$(RM) obj
		printf "[ $(RED)%-30s$(C_CLS) ]\n" "Objects file got cleaned successfuly ❌"
		sleep 1

fclean : clean
		$(RM) bin obj
		printf "[ $(RED)%-30s$(C_CLS) ]\n" "Executable & Archives got removed successfuly ❌"

help:
	@echo "Available targets:"
	@echo "  all        - Build the project (default target)"
	@echo "  clean      - Remove object files and the $(OBJ_DIR) directory"
	@echo "  fclean     - Remove the executable and object files"
	@echo "  re         - Clean and rebuild the project"
	@echo "  help       - Display this message"

re : fclean all
.SILENT : $(DIR) $(NAME) clean fclean $(OBJS)
