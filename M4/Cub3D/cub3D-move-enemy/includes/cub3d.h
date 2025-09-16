/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:22 by vagarcia          #+#    #+#             */
/*   Updated: 2025/06/26 11:08:32 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# ifndef BONUS
#  define BONUS 0
# endif

# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include "../libft/libft.h"
# include <fcntl.h>
# include <stdbool.h>
# include <string.h>
# include <mlx.h>
# include <sys/time.h>
# include <time.h>
//# include "../minilibx-linux/mlx.h"

// Constants for window dimensions
# define WIN_WIDTH 720
# define WIN_HEIGHT 680
# define TEX_WIDTH 256
# define TEX_HEIGHT 256
# define MAP_SCALE 5

// Error codes
# define ERR_MLX_INIT 1
# define ERR_MLX_WIN 2
# define ERR_TEX_LOAD 3
# define ERR_MEMORY 4

// Direction constants
# define NORTH 0
# define SOUTH 1
# define EAST 2
# define WEST 3

// Direction constants
# define COIN 0
# define DOOR 1
# define P1 2
# define P2 3

// Movements
# define W 0
# define S 1
# define A 2
# define D 3
# define LEFT 4
# define RIGHT 5
# define UP 12
# define DOWN 13

// Linux key codes
# define KEY_ESC 65307
# define KEY_W 119
# define KEY_A 97
# define KEY_S 115
# define KEY_D 100
# define KEY_Q 113
# define KEY_E 101
# define KEY_LEFT 65361
# define KEY_RIGHT 65363
# define KEY_UP 65362
# define KEY_DOWN 65364
# define KEY_ZERO 65438
# define KEY_ONE 65436
# define KEY_TWO 65433
# define KEY_THREE 65435
# define KEY_FIVE 65437
# define KEY_C 99
# define KEY_SPACE 32

// Movement speed and rotation speed
# define MOVE_SPEED 0.05
# define ROT_SPEED 0.03
# define SPRITE_COIN 0
# define SPRITE_PLAYER1 1
# define SPRITE_PLAYER2 2
//# define SPRITE_DEAD 3
# define SPRITE_ENEMY 3
# define SPRITE_GUN 4
# define SPRITE_DOOR 5
# define MAX_SPRITES 1024
# define SPRITE_ENEMY_FIAT 6
# define MAX_SPRITE_TYPES 10

// Structure for sprite drawing calculations
typedef struct s_sprite_draw
{
	double	sprite_x;
	double	sprite_y;
	double	inv_det;
	double	transform_x;
	double	transform_y;
	int		sprite_screen_x;
	int		sprite_height;
	int		sprite_width;
	int		draw_start_x;
	int		draw_start_y;
	int		draw_end_x;
	int		draw_end_y;
}	t_sprite_draw;

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
	int		width;
	int		height;
}	t_img;

typedef struct s_texture
{
	t_img	img;
	char	*path;
}	t_texture;

typedef struct s_player
{
	double	pos_x;
	double	pos_y;
	double	dir_x;
	double	dir_y;
	double	plane_x;
	double	plane_y;
	int		index; // index for sprites
	int		coin;
	int		dead;
	int		starting_x;
	int		starting_y;
}	t_player;

typedef struct s_ray
{
	double	camera_x;
	int		y; // look up and down
	double	ray_dir_x;
	double	ray_dir_y;
	int		map_x;
	int		map_y;
	double	side_dist_x;
	double	side_dist_y;
	double	delta_dist_x;
	double	delta_dist_y;
	double	perp_wall_dist;
	int		step_x;
	int		step_y;
	int		hit;
	int		side;
	int		line_height;
	int		draw_start;
	int		draw_end;
	int		tex_x;
	double	wall_x;
	int		bonus;
	int		bonus_cor[2];
}	t_ray;

typedef struct s_color
{
	int	r;
	int	g;
	int	b;
	int	color;
}	t_color;

/* typedef struct s_map
{
	char	**grid;
	int		width;
	int		height;
}	t_map; */

typedef struct s_map
{
	char	*texture_no; // paths to textures
	char	*texture_so;
	char	*texture_we;
	char	*texture_ea;
	char	*texture_door;
	char	*texture_coin;
	char	*texture_p1;
	char	*texture_p2;
	int		floor[3]; //rgb colours
	int		ceiling[3];
	int		identifier[7];
	// flags in order for NO, SO, WE, EA, F ,C;
	// identifier[6] == 1 if parsing map started to throw newline error
	char	**grid;
	int		width;
	int		height;
	int		flag;
	// to check enclosed map init as flag > 1 not enclosed, flag == 0 valid
	int		players;
	// for bonus, total number of players on map
}			t_map;

// Forward declaration
typedef struct s_sprite
{
	double	x;
	double	y;
	int		texture;
	double	distance; // Distance from player (for sorting)
	int		visible; // Whether the sprite is visible
	int		direction_x; // sprite moving direction
	int		direction_y; // sprite moving direction
}	t_sprite;

typedef struct s_game
{
	void					*mlx;
	void					*win;
	int						move[16]; // to make it smoother?
	int						action[4];
	int						frame;
	int						second;
	int						x; // previous x for mouse movement
	int						y; // previous y for mouse movement
	int						camera_y[2]; // look up and down
	t_map					map;
	t_player				player[2];
	t_img					img;
	t_texture				textures[4];
	// Increased to hold gun and other sprites
	t_texture				textures_bonus[10];
	t_color					floor;
	t_color					ceiling;
	int						**texture_data;
	int						**texture_data_bonus;
	int						coin_count;

	// Sprite-related data
	struct s_sprite			*sprites;
	struct s_sprite_draw	*draw;
	int						sprite_count;
	int						*sprite_order;
	double					*sprite_distance;
	double					*zbuffer;
}	t_game;

// Sprites
void	free_sprites(t_game *game);
void	setup_sprites(t_game *game);
void	init_sprites(t_game *game);
void	add_sprite(t_game *game, double x, double y, int texture);
void	sort_sprites(t_game *game, int active_player);
void	cast_sprites(t_game *game, int player_index);
void	update_sprites(t_game *game);
void	draw_sprite(t_game *game, int sprite_index, int player_index);
void	calc_sprite_transform(t_game *game,
			t_sprite_draw *draw, int sprite_index, int player_index);
void	calc_sprite_dimensions(t_game *game,
			t_sprite_draw *draw, int sprite_index);
void	calc_sprite_boundaries(t_sprite_draw *draw,
			t_game *game, int player_index);
void	render_sprite_stripe(t_game *game, int stripe, int sprite_index, \
		int player_index);
//parse
int		parse(t_map	*map, char *input, int x);
int		parse_line(t_map *map, char *temp, int line);
int		check_map(t_map *map, int gnl_flag, int line);
int		check_identifier(t_map *map, int error);
int		can_pass(char c);
//init
int		init_map(t_map	*map, char *input);
//error
void	print_example(char *temp);
void	identifier_error(int x, int line);
void	colour_error(int i, int x, char **temp, int line);
//utils
char	**add_array(char **input, char *entry, int y);
char	**copy_map(char **map);
void	ft_free_split(char **array);
void	free_all(t_map	*map);
void	free_parse(int fd, char *temp);
void	print_array(char **temp);

// Raycasting functions
void	cast_rays(t_game *game, int i);
void	init_ray(t_ray *ray, int x, t_game *game, int i);
void	calculate_step_and_side_dist(t_ray *ray, t_player *player);
void	perform_dda(t_ray *ray, char **map, int map_height, int map_width);
int		perform_dda1(t_ray *ray, int map_height, int map_width);
void	calculate_wall_distance(t_ray *ray, t_player *player);
void	calculate_wall_height(t_ray *ray);
void	draw_vertical_line(t_game *game, t_ray *ray, int x);

// Texture functions
void	load_textures(t_game *game);
void	free_textures(t_game *game);
int		get_texture_color(t_game *game, int tex_num, int tex_x, int tex_y);
void	calculate_texture_x(t_ray *ray, t_player *player);
int		determine_texture(t_ray *ray);

// Utils
int		create_rgb(int r, int g, int b);
void	my_mlx_pixel_put(t_img *img, int x, int y, int color);
int		exit_game(t_game *game);
void	error_exit(t_game *game, int error_code);
void	init_game(t_game *game);
void	init_player_direction(t_player *player, char direction);
int		render_frame(t_game *game);
void	free_map(t_map *map);
void	free_texture_paths(t_game *game);
void	free_textures_bonus(t_game *game);

// Movement
int		key_release_bonus_2p(int keycode, t_game *game);
int		key_press_bonus_2p(int keycode, t_game *game);
int		key_press(int keycode, t_game *game);
int		key_release(int keycode, t_game *game);
void	move_forward(t_game *game, t_player *player);
void	move_backward(t_game *game, t_player *player);
void	move_left(t_game *game, t_player *player);
void	move_right(t_game *game, t_player *player);
void	rotate_left(t_player *player);
void	rotate_right(t_player *player);
void	update_move(t_game *game);

// bonus
void	draw_bonus(t_game *game, int i);
int		key_press_bonus_2p(int keycode, t_game *game);
int		key_release_bonus_2p(int keycode, t_game *game);
void	bonus_hooks(t_game *game);
int		check_players(t_map *map);
void	check_bonus(t_map *map, int y, int x, char **temp);
void	get_coin(t_game *game, int i);
//void	draw_coin(t_game *game);
void	load_textures_bonus(t_game *game);
void	door(t_game *game, int i);
void	check_hit(t_ray *ray, char **map);
void	cast_rays_bonus(t_game *game, int i, int x);
void	calculate_obj_distance(t_ray *ray, t_player *player);
void	calculate_obj_height(t_ray *ray);
void	calculate_obj_x(t_ray *ray, t_player *player);
void	fire(t_game *game, int i);
void	perform_dda_bonus(t_ray *ray,
			char **map, int map_height, int map_width);
void	render_dead(t_game *game, int i);
void	draw_coin_info(t_game *game, int i);
void	draw_range(t_game *game, int i, char *dst);
void	draw_map(t_game *game, int x, int y, int pixel);
void	move_enemy_x(t_game *game, int i);
void	move_enemy_y(t_game *game, int i);

// Weapon/player arm
void	draw_player_weapon_bobbing(t_game *game);

#endif