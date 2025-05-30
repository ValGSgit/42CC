/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_draw.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:06 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 16:20:27 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	my_mlx_pixel_put(t_img *img, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || y < 0 || x >= WIN_WIDTH || y >= WIN_HEIGHT)
		return ;
	dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

void	draw_ceiling(t_game *game, int x, int draw_start)
{
	int	y;

	y = 0;
	while (y < draw_start)
	{
		my_mlx_pixel_put(&game->img, x, y, game->ceiling.color);
		y++;
	}
}

void	draw_floor(t_game *game, int x, int draw_end)
{
	int	y;

	y = draw_end;
	if (y < 0)
		y = 0;
	while (y + 1 < WIN_HEIGHT)
	{
		my_mlx_pixel_put(&game->img, x, y, game->floor.color);
		y++;
	}
}

void	draw_texture(t_game *game, t_ray *ray, int x, int tex_num)
{
	int		y;
	int		tex_y;
	double	step;
	double	tex_pos;
	int		color;

	step = 1.0 * TEX_HEIGHT / ray->line_height;
	tex_pos = (ray->draw_start - WIN_HEIGHT / 2 - ray->y + ray->line_height / 2)
		* step;
	y = ray->draw_start;
	while (y < ray->draw_end)
	{
		tex_y = (int)tex_pos & (TEX_HEIGHT - 1);
		tex_pos += step;
		color = get_texture_color(game, tex_num, ray->tex_x, tex_y);
		my_mlx_pixel_put(&game->img, x, y, color);
		y++;
	}
}

void	draw_vertical_line(t_game *game, t_ray *ray, int x)
{
	int	tex_num;

	tex_num = determine_texture(ray);
	draw_ceiling(game, x, ray->draw_start);
	if (BONUS && (ray->hit == 2))
		tex_num = 5;
	draw_texture(game, ray, x, tex_num);
	draw_floor(game, x, ray->draw_end);
}
