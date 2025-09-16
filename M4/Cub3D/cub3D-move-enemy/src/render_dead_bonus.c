/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_dead_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:06 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 19:42:49 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

int	create_rgb(int r, int g, int b)
{
	return (r << 16 | g << 8 | b);
}

void	draw_dead(t_game *game)
{
	int				y;
	int				x;
	char			*dst;
	unsigned int	src;

	src = 0;
	y = 0;
	while (y < 256)
	{
		x = 0;
		while (x < 256)
		{
			dst = game->img.addr + ((y - 128 + WIN_HEIGHT / 2)
					* game->img.line_length + (x - 128 + WIN_WIDTH / 2)
					* (game->img.bits_per_pixel / 8));
			if (game->textures_bonus[SPRITE_PLAYER2].path)
				src = game->texture_data_bonus[SPRITE_PLAYER2][y * TEX_WIDTH
					+ x];
			if (src)
				*(unsigned int *)dst = src;
			x++;
		}
		y++;
	}
}

// added player touching enemy will die
// later we can print framerate on the screen for debugging?
void	render_dead(t_game *game, int i)
{
	if (BONUS == 1 && game->map.grid[(int)game->player[0].pos_y] \
		[(int)game->player[0].pos_x] == 'F')
	{
		game->player[0].dead = 1;
		game->player[0].coin = 0;
	}
	if (game->player[i].dead == 1)
	{
		game->camera_y[i] -= 10;
		draw_dead(game);
	}
	if (game->player[i].dead == 1 && game->camera_y[i] < -600)
	{
		game->player[i].dead = 0;
		if (BONUS == 2)
			game->sprites[game->player[i].index].texture = SPRITE_PLAYER1;
		game->camera_y[i] = 0;
		game->player[i].pos_x = game->player[i].starting_x + 0.5;
		game->player[i].pos_y = game->player[i].starting_y + 0.5;
	}
}
