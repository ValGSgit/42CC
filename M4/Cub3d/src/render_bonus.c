/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:06 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 12:17:14 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	draw_cross_y(t_game *game)
{
	int				y;
	int				x;
	char			*dst;
	unsigned int	src;

	y = (WIN_HEIGHT / 2) - 20;
	while (y < (WIN_HEIGHT / 2) + 20)
	{
		x = (WIN_WIDTH / 2) - 2;
		while (x < (WIN_WIDTH / 2) + 2)
		{
			dst = game->img.addr + (y * game->img.line_length + x
					* (game->img.bits_per_pixel / 8));
			src = create_rgb(255, 255, 255);
			if (y < (WIN_HEIGHT / 2) - 7 || y > (WIN_HEIGHT / 2) + 7)
				*(unsigned int *)dst = src;
			x++;
		}
		y++;
	}
}

void	draw_cross(t_game *game)
{
	int				y;
	int				x;
	char			*dst;
	unsigned int	src;

	y = (WIN_HEIGHT / 2) - 2;
	while (y < (WIN_HEIGHT / 2) + 2)
	{
		x = (WIN_WIDTH / 2) - 20;
		while (x < (WIN_WIDTH / 2) + 20)
		{
			dst = game->img.addr + (y * game->img.line_length + x
					* (game->img.bits_per_pixel / 8));
			src = create_rgb(255, 255, 255);
			if (x < (WIN_WIDTH / 2) - 7 || x > (WIN_WIDTH / 2) + 7)
				*(unsigned int *)dst = src;
			x++;
		}
		y++;
	}
	draw_cross_y(game);
}

/* void	print_fps(t_game *game)
{
	struct timeval	time;

	game->frame++;
	gettimeofday(&time, NULL);
	if (game->second != time.tv_sec)
	{
		ft_printf("framerate == %d\n", game->frame);
		game->frame = 0;
		game->second = time.tv_sec;
	}
} */
// just for debugger, also open the function above:
// print_fps(game);
// Render sprites for players
void	draw_bonus(t_game *game, int i)
{
	cast_sprites(game, i);
	draw_cross(game);
	draw_map(game, 0, 0, 0);
	draw_range(game, i, NULL);
	draw_coin_info(game, i);
	render_dead(game, i);
	draw_player_weapon_bobbing(game);
}
