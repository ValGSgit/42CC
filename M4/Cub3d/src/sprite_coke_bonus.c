/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprite_coke.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:34:16 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 12:11:54 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/*
** Scan the map for sprite items (K for coins, etc.)
** and create sprites for them
// ft_printf("Added coin sprite at position (%d,%d)\n", x, y);
** commented out the print statement since its not required
*/
void	scan_map_for_sprites(t_game *game)
{
	int		x;
	int		y;
	int		sprite_idx;

	y = 0;
	sprite_idx = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width && game->map.grid[y][x])
		{
			if (game->map.grid[y][x] == 'K')
			{
				sprite_idx = game->sprite_count;
				add_sprite(game, x + 0.5, y + 0.5, SPRITE_COIN);
				game->sprites[sprite_idx].visible = 1;
				game->coin_count++;
			}
			else if (game->map.grid[y][x] == 'C' || game->map.grid[y][x] == 'O')
				add_sprite(game, x + 0.5, y + 0.5, SPRITE_DOOR);
			x++;
		}
		y++;
	}
}

/*
** Setup sprites at the beginning of the game
*/

static int	choose_car_type(int x, int y)
{
	int	car_selector;

	car_selector = ((int)x + (int)y) % 2;
	if (car_selector == 0)
		return (SPRITE_ENEMY);
	else
		return (SPRITE_ENEMY_FIAT);
}

static void	put_sprites(t_game *game)
{
	int	x;
	int	y;
	int	car_type;

	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width && game->map.grid[y][x])
		{
			if (game->map.grid[y][x] == 'F')
			{
				car_type = choose_car_type(x, y);
				add_sprite(game, x + 0.5, y + 0.5, car_type);
			}
			x++;
		}
		y++;
	}
}

void	setup_sprites(t_game *game)
{
	srand(rand());
	init_sprites(game);
	scan_map_for_sprites(game);
	if (BONUS == 2)
	{
		game->player[0].index = game->sprite_count;
		add_sprite(game, game->player[0].pos_x, \
			game->player[0].pos_y, SPRITE_PLAYER1);
		game->player[1].index = game->sprite_count;
		add_sprite(game, game->player[1].pos_x, \
			game->player[1].pos_y, SPRITE_PLAYER1);
	}
	else if (BONUS == 1)
	{
		put_sprites(game);
	}
}
