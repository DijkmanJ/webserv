# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: jdijkman <jdijkman@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2024/11/13 14:14:19 by jdijkman      #+#    #+#                  #
#    Updated: 2024/12/13 16:34:09 by jdijkman      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++20

NAME = webserv
SRC_DIR = ./src
BIN_DIR = ./bin

SRC = 	\
$(SRC_DIR)/cgi.cpp \
$(SRC_DIR)/delete.cpp \
$(SRC_DIR)/error_page.cpp \
$(SRC_DIR)/get.cpp \
$(SRC_DIR)/handle_request.cpp \
$(SRC_DIR)/launch_server.cpp \
$(SRC_DIR)/main.cpp \
$(SRC_DIR)/parse_server.cpp \
$(SRC_DIR)/post.cpp \
$(SRC_DIR)/response_content.cpp \
$(SRC_DIR)/run_server.cpp \
$(SRC_DIR)/server_utils.cpp \
$(SRC_DIR)/serverManager.cpp \
$(SRC_DIR)/utils.cpp \

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

all: $(NAME)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(NAME): $(BIN_DIR) $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean: logs
	rm -rf $(BIN_DIR)

logs:
	rm -rf ./log_*

fclean: clean logs
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re logs