/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coin_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:06 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/21 19:41:56 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	draw_coins(t_game *game, int index, int row)
{
	int				y;
	int				x;
	char			*dst;
	unsigned int	src;

	y = 0;
	while (y < 128)
	{
		x = 0;
		while (x < 128)
		{
			if (index / row >= WIN_WIDTH / 128)
				row++;
			dst = game->img.addr + ((y + (WIN_HEIGHT - (row * 128)))
					* game->img.line_length + (x + (index * 128))
					* (game->img.bits_per_pixel / 8));
			src = game->texture_data_bonus[0][y * 2 * TEX_WIDTH + x * 2];
			if (src)
				*(unsigned int *)dst = src;
			x++;
		}
		y++;
	}
}

void	draw_coin_info(t_game *game, int i)
{
	int	index;
	int	row;

	row = 1;
	index = 0;
	while (index < game->player[i].coin)
	{
		draw_coins(game, index, row);
		index++;
	}
	if (game->player[i].coin == -1)
		render_dead(game, i);
}

// end the game
static void	end_game(t_game *game, int i)
{
	if (game->coin_count == 0 && game->player[i].coin > -1)
	{
		if (BONUS == 2 && game->player[1].coin > game->player[0].coin)
		{
			game->player[0].dead = 1;
			game->player[0].coin = -1;
		}
		else if (BONUS == 2 && game->player[1].coin < game->player[0].coin)
		{
			game->player[1].dead = 1;
			game->player[1].coin = -1;
		}
		ft_printf("Player %d Win!\n", i + 1);
		game->player[i].coin++;
	}
	if (game->player[i].coin > (WIN_HEIGHT / 128) * (WIN_WIDTH / 128))
		exit_game(game);
}

// i == index of player
void	get_coin(t_game *game, int i)
{
	int	x;
	int	y;
	int	j;

	x = (int)game->player[i].pos_x;
	y = (int)game->player[i].pos_y;
	if (game->map.grid[y][x] == 'K')
	{
		game->map.grid[y][x] = '0';
		game->player[i].coin++;
		j = -1;
		while (++j < game->sprite_count)
		{
			if (game->sprites[j].texture == SPRITE_COIN
				&& (int)game->sprites[j].x == x && (int)game->sprites[j].y == y)
			{
				game->sprites[j].visible = 0;
				game->sprites[j].x = 0;
				game->sprites[j].y = 0;
				game->coin_count--;
				break ;
			}
		}
	}
	end_game(game, i);
}
