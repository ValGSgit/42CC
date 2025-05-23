/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 12:02:25 by kfan              #+#    #+#             */
/*   Updated: 2025/05/21 15:29:35 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

// change i < 5 to not take ".cub"
int	init_map(t_map *map, char *input)
{
	int	i;

	map->texture_no = NULL;
	map->texture_so = NULL;
	map->texture_we = NULL;
	map->texture_ea = NULL;
	map->texture_door = NULL;
	map->texture_coin = NULL;
	map->texture_p1 = NULL;
	map->texture_p2 = NULL;
	map->grid = NULL;
	map->flag = 0;
	map->players = 0;
	i = 0;
	while (i < 7)
	{
		map->identifier[i] = 0;
		i++;
	}
	i = 0;
	while (input[i])
		i++;
	if (i < 4 || ft_strncmp(&input[i - 4], ".cub", 4))
		return (identifier_error(-1, 0), 1);
	return (0);
}

// note for Valentino:
// while (game->map.grid[y][x] && x < game->map.width)
// I added another check in the loop to avoid invalid read
// as some of the lines could be shorter:
// int i and y moved just to get through 25 lines, both = 0
static void	find_player_pos(t_game *game, int y, int i)
{
	int		x;
	char	c;

	while (y < game->map.height)
	{
		x = 0;
		while (game->map.grid[y][x] && x < game->map.width)
		{
			c = game->map.grid[y][x];
			if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
			{
				if (i > 1)
					return ;
				game->player[i].pos_x = x + 0.5;
				game->player[i].pos_y = y + 0.5;
				game->player[i].starting_x = x;
				game->player[i].starting_y = y;
				init_player_direction(&game->player[i], c);
				game->map.grid[y][x] = '0';
				i++;
			}
			x++;
		}
		y++;
	}
}

// translation from Ka Hou's parsing to Valentino's game
static void	init_bridge(t_game *game)
{
	game->textures[NORTH].path = game->map.texture_no;
	game->textures[SOUTH].path = game->map.texture_so;
	game->textures[EAST].path = game->map.texture_we;
	game->textures[WEST].path = game->map.texture_ea;
	game->textures_bonus[0].path = game->map.texture_coin;
	game->textures_bonus[1].path = game->map.texture_door;
	game->textures_bonus[2].path = game->map.texture_p1;
	game->textures_bonus[3].path = game->map.texture_p2;
	game->floor.r = game->map.floor[0];
	game->floor.g = game->map.floor[1];
	game->floor.b = game->map.floor[2];
	game->ceiling.r = game->map.ceiling[0];
	game->ceiling.g = game->map.ceiling[1];
	game->ceiling.b = game->map.ceiling[2];
}

// Loop hook to handle the frame rendering
static int	loop_hook(t_game *game)
{
	render_frame(game);
	return (0);
}

// Example of how to initialize the game structure
// This would be called from your main.c
// Set up event hooks for proper X11 resource management
// mlx_hook -> KeyPress event
// mlx_hook -> Window close event
// mlx_loop_hook -> Regular frame updates
void	init_game(t_game *game)
{
	init_bridge(game);
	find_player_pos(game, 0, 0);
	game->win = NULL;
	game->mlx = NULL;
	game->mlx = mlx_init();
	if (!game->mlx)
		error_exit(game, ERR_MLX_INIT);
	if (BONUS == 2)
		game->win = mlx_new_window(game->mlx, WIN_WIDTH * 2, WIN_HEIGHT,
				"cub3D");
	else
		game->win = mlx_new_window(game->mlx, WIN_WIDTH, WIN_HEIGHT, "cub3D");
	if (!game->win)
		error_exit(game, ERR_MLX_WIN);
	game->img.img = NULL;
	game->texture_data = NULL;
	game->floor.color = create_rgb(game->floor.r, game->floor.g, game->floor.b);
	game->ceiling.color = create_rgb(game->ceiling.r, game->ceiling.g,
			game->ceiling.b);
	if (BONUS)
		bonus_hooks(game);
	mlx_hook(game->win, 2, 1L << 0, key_press, game);
	mlx_hook(game->win, 3, 1L << 1, key_release, game);
	mlx_hook(game->win, 17, 0, exit_game, game);
	mlx_loop_hook(game->mlx, loop_hook, game);
}
