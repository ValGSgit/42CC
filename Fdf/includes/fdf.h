/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+
	+:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+
	+#+        */
/*                                                +#+#+#+#+#+
	+#+           */
/*   Created: 2025/01/15 12:12:01 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/15 12:12:01 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

# include "get_next_line.h"
# include <X11/keysym.h>
# include <fcntl.h>
# include <math.h>
# include <mlx.h>
# include <stdarg.h>
# include <stdlib.h>
# include <unistd.h>

# define INPUT_ERR "Valid fdf input looks like: ./fdf [map.42]\n"
# define MALLOC_ERR "Malloc failed somewhere somehow :(\n"
# define FILE_ERR "File not found :(\n"
# define EXTENSION_ERR "Invalid file extension [.fdf]\n"
# define INVALID_MAP_ERR "Invalid map :(\n"
# define END_MSG "The program has been closed properly!\n"
# define MLX_ERR "Minilibx failed to initialize :(\n"
# define WIN_ERR "Error opening window :(\n"
# define TITLE "FDF"

# define MAX_COLOR_HEX 16777215
# define VIOLET_HEX 0x8F00FF
# define YELLOW_HEX 0xFFFF00
# define BLUE_HEX 0x177E89
# define L_MAX 9223372036854775807

typedef struct pixl
{
	float		x;
	float		y;
	float		z;
	int			is_last;
	int			color;
}	t_pixl;

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		order;
}	t_img;

typedef struct data
{
	double	scale;
	int		color;
	int		random_color;
	int		random_color2;
	int		map_scale;
	int		is_colors;
	double	z_scale;
	int		shift_x;
	int		shift_y;
	int		is_isometric;
	double	angle;
	int		win_x;
	int		win_y;
	void	*mlx_ptr;
	void	*win_ptr;
	t_pixl	**matrix;
	t_img	img;
}	t_data;

// Colors
int		color5(t_pixl a, t_pixl b);
int		color4(t_data *params, t_pixl a, t_pixl b);
int		color3(t_data *params, t_pixl a, t_pixl b);
int		get_color(t_data *params, t_pixl a, t_pixl b);
void	change_color(t_data *data);
int		color2(t_pixl a, t_pixl b);
int		color1(t_pixl a, t_pixl b);

// Error Handling
int		invalid(const char *s);
int		map_check(char *file_name);
void	put_err(const char *s);
void	*malloc_check(size_t size, size_t size_of);
void	ft_free(char **arr);
int		is_valid(char *s);
int		ft_exit(t_data *data);
char	if_cap(char c);

// Bunch of Meth
void	isometric(t_pixl *pixl, double angle);
float	abs_value(float x);
float	max_value(float x, float y);
int		limit_x(float x, t_data *data);
int		limit_y(float y, t_data *data);
double	get_z_scale(t_data *data);
double	get_scale(t_data *data);
void	init_coords(t_pixl *a, t_pixl *b, t_data *param);

// Mapping
int		count_rows(int fd, char *line);
t_pixl	**process_map(char *file_name);
int		open_file(char *file_name, int permissions);
void	draw(t_pixl **matrix, t_data *data);
void	my_mlx_pixel_put(t_img *data, int x, int y, int color);
int		line_color(char *s);

// Mlx Stuff
int		if_key(int key, t_data *data);
void	print_option(t_data *param);

// Struct
void	init_data(t_data *data, t_pixl **matrix);

// Utils
int		index_of(char *s, char c);
int		hex_to_int(char *s);
void	set_color_styles(t_data *data);
char	*get_next_line(int fd);
int		ft_srch(char *s, char c);
char	**ft_split(char const *s, char c);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_atoi(const char *str);
size_t	ft_strlen(const char *s);
void	ft_putstr_fd(char *s, int fd);
int		count_words(char const *str, char c);
void	free_matrix(t_pixl **matrix);
int		ft_skipspace(const char *str);

#endif