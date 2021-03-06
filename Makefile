NAME = engine
RM = /bin/rm -f

FLAGS = -std=c++11# -Wall -Wextra -Werror

SRC_DIR := ./srcs/
OBJ_DIR := ./objs/
LIB_DIR := ./lib/
INC_DIR := ./includes/
HEADERS := ${INC_DIR}*.hpp
HEADERS_INC := -I ${INC_DIR}

# engine
FILES = engine chunk camera terrain FastNoise player lightEngine textureEngine structureEngine
CFILES = $(patsubst %, $(SRC_DIR)%.cpp, $(FILES))
OFILES = $(patsubst %, $(OBJ_DIR)%.o, $(FILES))

GL_FLAGS = -lglfw3 -framework AppKit -framework OpenGL -framework IOKit -framework CoreVideo
GL_DIR = $(LIB_DIR)glfw/src
GL_INC = -I $(LIB_DIR)glfw/include/
GL_LINK = -L$(GL_DIR)

ASSIMP_LINK = -lassimp

.PHONY: all clean fclean re

all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@echo [INFO] engine Object Files Directory Created

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HEADERS)
	clang++ $(FLAGS) ${HEADERS_INC} $(GL_INC) -o $@ -c $< 

$(NAME): $(OBJ_DIR) $(OFILES)
	@clang++ $(FLAGS) $(GL_LINK) $(OFILES) $(ASSIMP_LINK) $(GL_FLAGS) -o $(NAME)
	@echo [INFO] engine Binary Created

clean:
	@rm -rf $(OBJ_DIR)
	@echo [INFO] engine Object Files Directory Destroyed

fclean: clean
	@$(RM) $(NAME)
	@echo [INFO] engine Binary Destroyed

re: fclean all

