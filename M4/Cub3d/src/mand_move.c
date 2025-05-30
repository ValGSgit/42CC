/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mand_move.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 10:46:15 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 10:48:23 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

#if BONUS == 0
// Single player movement update for mandatory version
void	update_move(t_game *game)
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
}

// Dummy implementations for bonus functions when not in bonus mode
void	fire(t_game *game, int player)
{
	(void)game;
	(void)player;
}

void	door(t_game *game, int player)
{
	(void)game;
	(void)player;
}

void	bonus_hooks(t_game *game)
{
	(void)game;
}

#endif
