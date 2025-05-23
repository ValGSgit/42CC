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
# include "mega_colors.h"
# include <X11/keysym.h>
# include <fcntl.h>
# include <math.h>
# include <mlx.h>
# define INVALID_MAP "Invalid map :(\n"
# define TITLE "FDF"

# define L_MAX 9223372036854775807

typedef struct s_matrix
{
	float	x;
	float	y;
	float	z;
	int		last;
	int		color;
}			t_matrix;

typedef struct s_mlx
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line_length;
	int		order;
}			t_mlx;

typedef struct s_data
{
	double		scale;
	int			color;
	int			dimensions;
	double		height;
	int			target_x;
	int			target_y;
	int			is_isometric;
	double		angle;
	int			win_x;
	int			win_y;
	void		*mlx_ptr;
	void		*win_ptr;
	t_matrix	**matrix;
	t_mlx		img;
}				t_data;

// Colors
int			get_color(void);

// Error Handling
int			invalid(const char *s);
int			map_check(char *file_name);
void		put_err(const char *s);
void		ft_free(char **arr);
int			is_valid(char *s);
int			clear(t_data *data);

// Bunch of Meth
float		abs_value(float x);
float		max_value(float x, float y);
int			limit_x(float x, t_data *data);
int			limit_y(float y, t_data *data);
double		get_heights(t_data *data);
double		get_scale(double dimensions);
void		init_coords(t_matrix *a, t_matrix *b, t_data *param);

// Mapping
int			item_count(char *line);
int			count_rows(int fd, char *line);
int			word_count(char const *str, char c);
t_matrix	**process_map(char *file_name);
int			open_file(char *file_name, int permissions);
void		put_matrix(t_matrix **matrix, t_data *data);
void		my_mlx_pixel_put(t_mlx *data, int x, int y, int color);

// Mlx Stuff
int			if_key(int key, t_data *data);
void		print_keys(t_data *param);

// Struct
void		init_data(t_data *data, t_matrix **matrix);

// Utils
int			ft_srch(char *s, char c);
char		*get_next_line(int fd);
char		**ft_split(char const *s, char c);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_atoi(const char *str);
size_t		ft_strlen(const char *s);
void		ft_putstr_fd(char *s, int fd);
void		free_matrix(t_matrix **matrix);

#endif