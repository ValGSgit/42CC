/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_actions.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 17:17:24 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/21 19:44:26 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	more_key_press(int keycode, t_game *game)
{
	if (keycode == KEY_UP)
		game->move[6] = 1;
	else if (keycode == KEY_DOWN)
		game->move[7] = 1;
	else if (keycode == KEY_LEFT)
		game->move[8] = 1;
	else if (keycode == KEY_RIGHT)
		game->move[9] = 1;
	else if (keycode == KEY_ONE)
		game->move[10] = 1;
	else if (keycode == KEY_THREE)
		game->move[11] = 1;
	else if (keycode == KEY_FIVE)
		game->move[14] = 1;
	else if (keycode == KEY_TWO)
		game->move[15] = 1;
}

int	key_press_bonus_2p(int keycode, t_game *game)
{
	if (keycode == KEY_W)
		game->move[W] = 1;
	else if (keycode == KEY_S)
		game->move[S] = 1;
	else if (keycode == KEY_A)
		game->move[A] = 1;
	else if (keycode == KEY_D)
		game->move[D] = 1;
	else if (keycode == KEY_Q)
		game->move[LEFT] = 1;
	else if (keycode == KEY_E)
		game->move[RIGHT] = 1;
	else if (keycode == KEY_SPACE)
		fire(game, 0);
	else if (keycode == KEY_ZERO)
		fire(game, 1);
	more_key_press(keycode, game);
	return (0);
}

static void	more_key_release(int keycode, t_game *game)
{
	if (keycode == KEY_UP)
		game->move[6] = 0;
	else if (keycode == KEY_DOWN)
		game->move[7] = 0;
	else if (keycode == KEY_LEFT)
		game->move[8] = 0;
	else if (keycode == KEY_RIGHT)
		game->move[9] = 0;
	else if (keycode == KEY_ONE)
		game->move[10] = 0;
	else if (keycode == KEY_THREE)
		game->move[11] = 0;
	else if (keycode == KEY_FIVE)
		game->move[14] = 0;
	else if (keycode == KEY_TWO)
		game->move[15] = 0;
}

int	key_release_bonus_2p(int keycode, t_game *game)
{
	if (keycode == KEY_W)
		game->move[W] = 0;
	else if (keycode == KEY_S)
		game->move[S] = 0;
	else if (keycode == KEY_A)
		game->move[A] = 0;
	else if (keycode == KEY_D)
		game->move[D] = 0;
	else if (keycode == KEY_Q)
		game->move[LEFT] = 0;
	else if (keycode == KEY_E)
		game->move[RIGHT] = 0;
	more_key_release(keycode, game);
	return (0);
}
