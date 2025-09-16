/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement_players.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:37:58 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/15 18:41:04 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	update_move_p2(t_game *game)
{
	if (game->move[6] == 1)
		move_forward(game, &game->player[1]);
	else if (game->move[7] == 1)
		move_backward(game, &game->player[1]);
	if (game->move[8] == 1)
		move_left(game, &game->player[1]);
	else if (game->move[9] == 1)
		move_right(game, &game->player[1]);
	if (game->move[10] == 1)
		rotate_left(&game->player[1]);
	else if (game->move[11] == 1)
		rotate_right(&game->player[1]);
	if (game->move[14] == 1 && game->camera_y[1] <= 500)
		game->camera_y[1] += 10;
	else if (game->move[15] == 1 && game->camera_y[1] >= -500)
		game->camera_y[1] -= 10;
}

// to make game smoother
// instead of in keycode event
// can move and turn at the same time
void	update_move(t_game *game)
{
	if (game->player[0].dead == 0)
	{
		if (game->move[W] == 1)
			move_forward(game, &game->player[0]);
		else if (game->move[S] == 1)
			move_backward(game, &game->player[0]);
		if (game->move[A] == 1)
			move_left(game, &game->player[0]);
		else if (game->move[D] == 1)
			move_right(game, &game->player[0]);
		if (game->move[LEFT] == 1)
			rotate_left(&game->player[0]);
		else if (game->move[RIGHT] == 1)
			rotate_right(&game->player[0]);
		if (game->move[12] == 1 && game->camera_y[0] <= 500)
			game->camera_y[0] += 10;
		else if (game->move[13] == 1 && game->camera_y[0] >= -500)
			game->camera_y[0] -= 10;
	}
	if (BONUS == 2 && game->player[1].dead == 0)
		update_move_p2(game);
}
