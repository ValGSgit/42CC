/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:06 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 21:21:10 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	check_hit(t_ray *ray, char **map)
{
	if (map[ray->map_y][ray->map_x] == 'O')
	{
		ray->bonus = 1;
		ray->bonus_cor[0] = ray->map_y;
		ray->bonus_cor[1] = ray->map_x;
	}
}

void	door(t_game *game, int i)
{
	t_ray	ray;
	int		x;

	x = WIN_WIDTH / 2;
	init_ray(&ray, x, game, i);
	calculate_step_and_side_dist(&ray, &game->player[i]);
	perform_dda(&ray, game->map.grid, game->map.height, game->map.width);
	if (ray.hit == 2)
		game->map.grid[ray.map_y][ray.map_x] = 'O';
	if (ray.bonus == 1)
		game->map.grid[ray.bonus_cor[0]][ray.bonus_cor[1]] = 'C';
}

void	change_map(t_game *game, t_ray *ray)
{
	int	x;
	int	y;

	x = ray->map_x;
	y = ray->map_y;
	if (game->map.grid[y][x + 1] == '0')
		game->map.grid[y][++x] = 'K';
	else if (game->map.grid[y][x - 1] == '0')
		game->map.grid[y][--x] = 'K';
	else if (game->map.grid[y + 1][x] == '0')
		game->map.grid[++y][x] = 'K';
	else if (game->map.grid[y - 1][x] == '0')
		game->map.grid[--y][x] = 'K';
	if (x != ray->map_x || y != ray->map_y)
	{
		add_sprite(game, x + 0.5, y + 0.5, SPRITE_COIN);
		game->coin_count++;
	}
}

// i = 2 for enermies
// j = -1 just a counter
// Remove the enemy sprite from the game
void	dead(t_game *game, t_ray *ray, int i, int j)
{
	change_map(game, ray);
	if (i < 2)
	{
		game->player[i].coin = 0;
		game->player[i].dead = 1;
		game->sprites[game->player[i].index].texture = SPRITE_PLAYER2;
	}
	else
	{
		while (++j < game->sprite_count)
		{
			if ((game->sprites[j].texture == SPRITE_ENEMY
					|| game->sprites[j].texture == SPRITE_ENEMY_FIAT)
				&& (int)game->sprites[j].x == ray->map_x
				&& (int)game->sprites[j].y == ray->map_y)
			{
				game->sprites[j].visible = 0;
				game->sprites[j].x = 0;
				game->sprites[j].y = 0;
				break ;
			}
		}
		game->map.grid[ray->map_y][ray->map_x] = '0';
		add_sprite(game, ray->map_x + 0.5, ray->map_y + 0.5, SPRITE_PLAYER2);
	}
}

void	fire(t_game *game, int i)
{
	t_ray	ray;
	int		x;
	int		hit;

	if (game->player[i].dead == 1)
		return ;
	x = WIN_WIDTH / 2;
	init_ray(&ray, x, game, i);
	calculate_step_and_side_dist(&ray, &game->player[i]);
	perform_dda_bonus(&ray, game->map.grid, game->map.height, game->map.width);
	calculate_wall_distance(&ray, &game->player[i]);
	hit = 0;
	if (game->camera_y[i] < 0)
		ray.perp_wall_dist *= -1;
	if (ray.perp_wall_dist * game->camera_y[i] < 450)
		hit = 1;
	if (BONUS == 2 && ray.hit == 3 && game->player[0].dead == 0 && hit)
		dead(game, &ray, 0, -1);
	if (BONUS == 2 && ray.hit == 4 && game->player[1].dead == 0 && hit)
		dead(game, &ray, 1, -1);
	if (BONUS == 1 && ray.hit == 3 && hit)
		dead(game, &ray, 2, -1);
	game->camera_y[i] += 30;
	render_frame(game);
	game->camera_y[i] -= 20;
}
