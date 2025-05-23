/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:37:53 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/22 16:15:54 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	free_textures_paths(t_game *game)
{
	int	i;

	i = 0;
	while (game->textures[i].path && i < 4)
	{
		if (game->textures[i].path)
			free(game->textures[i].path);
		i++;
	}
}

// Zero out the entire game struct to avoid uninitialized memory
// Use	printall(&game.map); for debug
// rotate_left(&game.player[0]); to avoid crashing
int	main(int argc, char **argv)
{
	t_game	game;

	ft_memset(&game, 0, sizeof(t_game));
	if (argc != 2)
		return (print_example("Error\nPlease only enter path of .cub\n"), 1);
	if (parse(&game.map, argv[1], 0))
		return (free_all(&game.map), 1);
	init_game(&game);
	if (game.mlx && game.win)
	{
		rotate_left(&game.player[0]);
		load_textures(&game);
		if (BONUS)
			setup_sprites(&game);
		render_frame(&game);
		mlx_loop(game.mlx);
	}
	else
	{
		ft_dprintf(2, "Error\nFailed to initialize mlx or create window\n");
		exit_game(&game);
		return (1);
	}
	exit_game(&game);
	return (0);
}
