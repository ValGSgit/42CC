/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprite_move_enemy_bonus.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 19:31:07 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 21:03:23 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	can_move(char c)
{
	if (c == '1' || c == 'C' || c == 'K' || c == 'O')
		return (0);
	return (1);
}

static void	change_map_enemy_y(t_game *game, int i)
{
	game->map.grid[(int)game->sprites[i].y][(int)(game->sprites[i].x)] = 'F';
	if (game->sprites[i].direction_y == 0)
		game->map.grid[(int)(game->sprites[i].y - 1)] \
		[(int)(game->sprites[i].x)] = '0';
	else if (game->sprites[i].direction_y == 1)
		game->map.grid[(int)(game->sprites[i].y + 1)] \
		[(int)game->sprites[i].x] = '0';
}

static void	change_map_enemy_x(t_game *game, int i)
{
	game->map.grid[(int)game->sprites[i].y][(int)(game->sprites[i].x)] = 'F';
	if (game->sprites[i].direction_x == 0)
		game->map.grid[(int)game->sprites[i].y] \
		[(int)(game->sprites[i].x - 1)] = '0';
	else if (game->sprites[i].direction_x == 1)
		game->map.grid[(int)game->sprites[i].y] \
		[(int)(game->sprites[i].x + 1)] = '0';
}

void	move_enemy_y(t_game *game, int i)
{
	if (game->sprites[i].visible == 0)
		return ;
	if (game->sprites[i].direction_y == 0 && can_move(game->map.grid \
		[(int)(game->sprites[i].y + 0.5)][(int)game->sprites[i].x]))
	{
		game->sprites[i].y += 0.01;
		if (game->map.grid[(int)game->sprites[i].y] \
		[(int)(game->sprites[i].x)] == '0')
			change_map_enemy_y(game, i);
	}
	else if (game->sprites[i].direction_y == 0)
		game->sprites[i].direction_y = 1;
	else if (game->sprites[i].direction_y == 1 && can_move(game->map.grid \
		[(int)(game->sprites[i].y - 0.5)][(int)game->sprites[i].x]))
	{
		game->sprites[i].y -= 0.01;
		if (game->map.grid[(int)game->sprites[i].y] \
		[(int)(game->sprites[i].x)] == '0')
			change_map_enemy_y(game, i);
	}
	else if (game->sprites[i].direction_y == 1)
		game->sprites[i].direction_y = 0;
}

void	move_enemy_x(t_game *game, int i)
{
	if (game->sprites[i].visible == 0)
		return ;
	if (game->sprites[i].direction_x == 0 && can_move(game->map.grid \
		[(int)game->sprites[i].y][(int)(game->sprites[i].x + 0.5)]))
	{
		game->sprites[i].x += 0.01;
		if (game->map.grid[(int)game->sprites[i].y] \
		[(int)(game->sprites[i].x)] == '0')
			change_map_enemy_x(game, i);
	}
	else if (game->sprites[i].direction_x == 0)
		game->sprites[i].direction_x = 1;
	else if (game->sprites[i].direction_x == 1 && can_move(game->map.grid \
		[(int)game->sprites[i].y][(int)(game->sprites[i].x - 0.5)]))
	{
		game->sprites[i].x -= 0.01;
		if (game->map.grid[(int)game->sprites[i].y] \
		[(int)(game->sprites[i].x)] == '0')
			change_map_enemy_x(game, i);
	}
	else if (game->sprites[i].direction_x == 1)
		game->sprites[i].direction_x = 0;
}
