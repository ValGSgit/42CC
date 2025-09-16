NAME = cub3D
BONUS_NAME = cub3D_bonus
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
#-I$(INC_DIR) -Iminilibx-linux -g
#MLX_PATH = minilibx-linux/
MLX_FLAGS = -lmlx -lXext -lX11 -lm #-L$(MLX_PATH) 
LIBFT = libft/libft.a

SRC_DIR = src
OBJ_DIR = obj
BONUS_OBJ_DIR = obj_bonus
INC_DIR = includes

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/raycasting.c \
       $(SRC_DIR)/raycasting_calculate.c \
       $(SRC_DIR)/render.c \
       $(SRC_DIR)/render_draw.c \
       $(SRC_DIR)/texture.c \
       $(SRC_DIR)/movement.c \
       $(SRC_DIR)/movement_wasd.c \
       $(SRC_DIR)/utils.c \
       $(SRC_DIR)/utils_texture.c \
       $(SRC_DIR)/parse.c \
       $(SRC_DIR)/parse_line.c \
       $(SRC_DIR)/parse_check.c \
       $(SRC_DIR)/init.c \
       $(SRC_DIR)/error.c \
       $(SRC_DIR)/utils_array.c \
       $(SRC_DIR)/utils_free.c \
	$(SRC_DIR)/mand_move.c \
	$(SRC_DIR)/mand_voids.c

BONUS_ONLY_SRCS = $(SRC_DIR)/key_actions_bonus.c \
                  $(SRC_DIR)/movement_players_bonus.c \
                  $(SRC_DIR)/render_bonus.c \
                  $(SRC_DIR)/render_map_bonus.c \
                  $(SRC_DIR)/movement_bonus.c \
                  $(SRC_DIR)/parse_check_bonus.c \
                  $(SRC_DIR)/coin_bonus.c \
                  $(SRC_DIR)/texture_bonus.c \
                  $(SRC_DIR)/door_bonus.c \
                  $(SRC_DIR)/raycasting_bonus.c \
                  $(SRC_DIR)/raycasting_calculate_bonus.c \
                  $(SRC_DIR)/sprite_bonus.c \
                  $(SRC_DIR)/sprite_draw_bonus.c \
                  $(SRC_DIR)/weapon_bonus.c \
                  $(SRC_DIR)/render_dead_bonus.c \
                  $(SRC_DIR)/sprite_utils_bonus.c \
                  $(SRC_DIR)/sprite_coke_bonus.c \
                  $(SRC_DIR)/sprite_move_enemy_bonus.c

BONUS_SRCS = $(SRCS) $(BONUS_ONLY_SRCS)

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

BONUS_OBJS = $(BONUS_SRCS:$(SRC_DIR)/%.c=$(BONUS_OBJ_DIR)/%.o)

all: $(NAME) #make_mlx

#make_mlx:
#	@make -C $(MLX_PATH)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(OBJS) $(MLX_FLAGS) $(LIBFT) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BONUS_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BONUS_OBJ_DIR)
	$(CC) $(CFLAGS) $(BONUS_FLAGS) -c $< -o $@

$(BONUS_NAME): $(BONUS_OBJS) $(LIBFT)
	$(CC) $(BONUS_OBJS) $(MLX_FLAGS) $(LIBFT) -o $(BONUS_NAME)

$(LIBFT):
	$(MAKE) -C ./libft

add_flag1:
	$(eval CFLAGS += -D BONUS=1)

add_flag2:
	$(eval CFLAGS += -D BONUS=2)

bonus: add_flag1 $(BONUS_NAME)

bonus2: clean add_flag2 $(BONUS_NAME)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BONUS_OBJ_DIR)
	$(MAKE) -C ./libft clean
# make -C $(MLX_PATH) clean

fclean: clean
	rm -f $(NAME) $(BONUS_NAME)
	$(MAKE) -C ./libft fclean

re: fclean all

.PHONY: all clean fclean re bonus bonus2 #make_mlx