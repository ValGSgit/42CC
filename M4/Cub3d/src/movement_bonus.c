/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:37:58 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/21 17:21:46 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	mouse_hook(int keycode, int x, int y, t_game *game)
{
	(void)x;
	(void)y;
	if (keycode == 1)
		fire(game, 0);
	return (0);
}

static int	mouse_move(int x, int y, t_game *game)
{
	if (game->player[0].dead == 1)
		return (0);
	if (x > game->x)
		rotate_right(&game->player[0]);
	else if (x < game->x)
		rotate_left(&game->player[0]);
	if (y < game->y && game->camera_y[0] <= 500)
		game->camera_y[0] += 10;
	if (y > game->y && game->camera_y[0] >= -500)
		game->camera_y[0] -= 10;
	if (x < WIN_WIDTH / 4 || x >= (3 * WIN_WIDTH) / 4 || y < WIN_HEIGHT / 4
		|| y >= (3 * WIN_HEIGHT) / 4)
		mlx_mouse_move(game->mlx, game->win, WIN_WIDTH / 2, WIN_HEIGHT / 2);
	game->x = x;
	game->y = y;
	return (0);
}

void	bonus_hooks(t_game *game)
{
	mlx_mouse_hide(game->mlx, game->win);
	mlx_hook(game->win, 6, 1L << 6, mouse_move, game);
	mlx_mouse_hook(game->win, mouse_hook, game);
}
