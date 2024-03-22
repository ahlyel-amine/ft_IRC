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
SRC = $(shell ls src/ | awk '{print "src/"$$1}')
CONTAINER = irs_server
OBJ_DIR = obj

OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o,  $(SRC))
DEPS = $(patsubst %.cpp, $(OBJ_DIR)/%.d,  $(SRC))

INCLUDES = -Iinc

FLGS = -Wall -Wextra -Werror -std=c++98
DEPSFLAGS = -MMD -MP
NAME = ircserv
#================================#
#            RULES               #
#================================#

all : $(NAME)

$(NAME) : $(OBJ)
	mkdir -p bin
	c++ $(FLGS) $^ -o bin/$@

$(OBJ_DIR)/%.o : %.cpp
	mkdir -p $(dir $@)
	printf "|[ $(GRN)%-37s$(C_CLS)$(YLW) ]| \r" "Compailing... $(notdir $@)"
	c++ $(FLGS) $(DEPSFLAGS) $(INCLUDES) -c $< -o $@

clean :
	printf "[ $(RED)%-30s$(C_CLS) ]\n" "Objects file got cleaned successfuly ❌"
	rm -rf $(OBJ_DIR)
	sleep 1

bot :
	make -C bot

fclean_bot :
	make -C bot fclean

clean_bot :
	make -C bot clean

fclean : clean
	rm -rf bin
	printf "[ $(RED)%-30s$(C_CLS) ]\n" "Executable & Archives got removed successfuly ❌"

re  : clean fclean all

build :
	docker build -t $(CONTAINER) .

start :
	docker run --privileged -it --rm -p 127.0.0.1:6666:6666 $(CONTAINER)
.PHONY : all clean fclean re
-include : $(DEPS)
.SILENT : $(NAME) $(OBJ) clean fclean re all bot fclean_bot clean_bot
