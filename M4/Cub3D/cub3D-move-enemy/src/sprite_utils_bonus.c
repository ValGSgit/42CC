/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprite_utils_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:30:55 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 21:46:04 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/*
** Initialize sprite system: allocate memory for sprites and buffers
*/
void	init_sprites(t_game *game)
{
	game->sprites = (t_sprite *)malloc(sizeof(t_sprite) * MAX_SPRITES);
	if (!game->sprites)
		error_exit(game, ERR_MEMORY);
	game->sprite_order = (int *)malloc(sizeof(int) * MAX_SPRITES);
	if (!game->sprite_order)
		error_exit(game, ERR_MEMORY);
	game->sprite_distance = (double *)malloc(sizeof(double) * MAX_SPRITES);
	if (!game->sprite_distance)
		error_exit(game, ERR_MEMORY);
	game->zbuffer = (double *)malloc(sizeof(double) * WIN_WIDTH);
	if (!game->zbuffer)
		error_exit(game, ERR_MEMORY);
	game->sprite_count = 0;
	ft_memset(game->sprites, 0, sizeof(t_sprite) * MAX_SPRITES);
	ft_memset(game->sprite_order, 0, sizeof(int) * MAX_SPRITES);
	ft_memset(game->sprite_distance, 0, sizeof(double) * MAX_SPRITES);
}

/*
** Free sprite system resources
*/
void	free_sprites(t_game *game)
{
	if (game->sprites)
		free(game->sprites);
	if (game->sprite_order)
		free(game->sprite_order);
	if (game->sprite_distance)
		free(game->sprite_distance);
	if (game->zbuffer)
		free(game->zbuffer);
	game->sprites = NULL;
	game->sprite_order = NULL;
	game->sprite_distance = NULL;
	game->zbuffer = NULL;
	game->sprite_count = 0;
}

/*
** Add a sprite to the game world
*/
void	add_sprite(t_game *game, double x, double y, int texture)
{
	if (game->sprite_count >= MAX_SPRITES)
		return ;
	game->sprites[game->sprite_count].x = x;
	game->sprites[game->sprite_count].y = y;
	game->sprites[game->sprite_count].texture = texture;
	game->sprites[game->sprite_count].visible = 1;
	game->sprites[game->sprite_count].direction_x = 0;
	game->sprites[game->sprite_count].direction_y = 0;
	game->sprite_count++;
}

static void	swap_sprites(t_game *game, int j)
{
	int		temp_order;
	double	temp_dist;

	if (game->sprite_distance[j] < game->sprite_distance[j + 1])
	{
		temp_dist = game->sprite_distance[j];
		game->sprite_distance[j] = game->sprite_distance[j + 1];
		game->sprite_distance[j + 1] = temp_dist;
		temp_order = game->sprite_order[j];
		game->sprite_order[j] = game->sprite_order[j + 1];
		game->sprite_order[j + 1] = temp_order;
	}
}

/*
** Sort sprites from far to close for proper rendering order
** This needs to be modified to use the correct player's 
** perspective in multiplayer
*/
void	sort_sprites(t_game *game, int active_player)
{
	int		i;
	int		j;

	i = -1;
	while (++i < game->sprite_count)
	{
		game->sprite_order[i] = i;
		game->sprite_distance[i] = \
				((game->player[active_player].pos_x - game->sprites[i].x) \
				* (game->player[active_player].pos_x - game->sprites[i].x) \
				+ (game->player[active_player].pos_y - game->sprites[i].y) \
				* (game->player[active_player].pos_y - game->sprites[i].y));
	}
	i = -1;
	while (++i < game->sprite_count - 1)
	{
		j = -1;
		while (++j < game->sprite_count - i - 1)
		{
			swap_sprites(game, j);
		}
	}
}
