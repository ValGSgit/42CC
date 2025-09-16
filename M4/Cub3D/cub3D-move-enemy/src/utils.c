/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:14 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 10:33:09 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	error_exit(t_game *game, int error_code)
{
	if (error_code == ERR_MLX_INIT)
		ft_dprintf(2, "Error\nFailed to initialize MLX\n");
	else if (error_code == ERR_MLX_WIN)
		ft_dprintf(2, "Error\nFailed to create MLX window\n");
	else if (error_code == ERR_TEX_LOAD)
		ft_dprintf(2, "Error\nFailed to load texture\n");
	else if (error_code == ERR_MEMORY)
		ft_dprintf(2, "Error\nMemory allocation failed\n");
	else
		ft_dprintf(2, "Error\nUnknown error\n");
	exit_game(game);
	exit(EXIT_FAILURE);
}

// In Linux with minilibx-linux
// we need to call mlx_destroy_display to clean up X11 resources
// #ifdef __linux__
// mlx_destroy_display(game->mlx);
// #endif
int	exit_game(t_game *game)
{
	if (!game)
		exit(EXIT_SUCCESS);
	free_texture_paths(game);
	if (game->img.img)
		mlx_destroy_image(game->mlx, game->img.img);
	free_textures(game);
	if (BONUS)
		free_textures_bonus(game);
	if (BONUS && game->sprites)
		free_sprites(game);
	free_map(&game->map);
	if (game->win)
	{
		mlx_destroy_window(game->mlx, game->win);
		game->win = NULL;
	}
	if (game->mlx)
	{
		mlx_destroy_display(game->mlx);
		free(game->mlx);
		game->mlx = NULL;
	}
	exit(EXIT_SUCCESS);
	return (0);
}

static void	init_player_direction1(t_player *player, char direction)
{
	if (direction == 'E')
	{
		player->dir_x = 1;
		player->dir_y = 0;
		player->plane_x = 0;
		player->plane_y = 0.66;
	}
	else if (direction == 'W')
	{
		player->dir_x = -1;
		player->dir_y = 0;
		player->plane_x = 0;
		player->plane_y = -0.66;
	}
}

void	init_player_direction(t_player *player, char direction)
{
	if (direction == 'N')
	{
		player->dir_x = 0;
		player->dir_y = -1;
		player->plane_x = 0.66;
		player->plane_y = 0;
	}
	else if (direction == 'S')
	{
		player->dir_x = 0;
		player->dir_y = 1;
		player->plane_x = -0.66;
		player->plane_y = 0;
	}
	else if (direction == 'E' || direction == 'W')
		init_player_direction1(player, direction);
}

#if BONUS == 0
// Create RGB color value from individual components
int	create_rgb(int r, int g, int b)
{
	return (r << 16 | g << 8 | b);
}
#endif
