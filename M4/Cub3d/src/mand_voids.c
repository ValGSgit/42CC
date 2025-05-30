/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mand_voids.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 10:47:43 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 10:48:43 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

#if BONUS == 0

void	setup_sprites(t_game *game)
{
	(void)game;
}

// Dummy implementations for bonus functions when not in 2-player mode
int	key_press_bonus_2p(int keycode, t_game *game)
{
	(void)keycode;
	(void)game;
	return (0);
}

int	key_release_bonus_2p(int keycode, t_game *game)
{
	(void)keycode;
	(void)game;
	return (0);
}

// Mandatory mode: check that exactly 1 player exists
int	check_players(t_map *map)
{
	if (map->players != 1)
		return (ft_dprintf(2, \
			"Error\nMap must have exactly 1 starting point\n"), 1);
	return (0);
}

#endif