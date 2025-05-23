/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement_wasd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:37:58 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/21 14:12:23 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	can_enter(char c)
{
	if (c == '1')
		return (0);
	if (BONUS && c == 'C')
		return (0);
	return (1);
}

// below: Collision detection - only move if the new position is not a wall
void	move_forward(t_game *game, t_player *player)
{
	double	new_x;
	double	new_y;
	double	move_speed;

	move_speed = MOVE_SPEED;
	new_x = player->pos_x + player->dir_x * move_speed;
	new_y = player->pos_y + player->dir_y * move_speed;
	if (can_enter(game->map.grid[(int)player->pos_y][(int)new_x]))
		player->pos_x = new_x;
	if (can_enter(game->map.grid[(int)new_y][(int)player->pos_x]))
		player->pos_y = new_y;
}

void	move_backward(t_game *game, t_player *player)
{
	double	new_x;
	double	new_y;
	double	move_speed;

	move_speed = MOVE_SPEED;
	new_x = player->pos_x - player->dir_x * move_speed;
	new_y = player->pos_y - player->dir_y * move_speed;
	if (can_enter(game->map.grid[(int)player->pos_y][(int)new_x]))
		player->pos_x = new_x;
	if (can_enter(game->map.grid[(int)new_y][(int)player->pos_x]))
		player->pos_y = new_y;
}

// Strafe left - perpendicular to the direction vector
void	move_left(t_game *game, t_player *player)
{
	double	new_x;
	double	new_y;
	double	move_speed;

	move_speed = MOVE_SPEED;
	new_x = player->pos_x - player->plane_x * move_speed;
	new_y = player->pos_y - player->plane_y * move_speed;
	if (can_enter(game->map.grid[(int)player->pos_y][(int)new_x]))
		player->pos_x = new_x;
	if (can_enter(game->map.grid[(int)new_y][(int)player->pos_x]))
		player->pos_y = new_y;
}

// Strafe right - perpendicular to the direction vector
void	move_right(t_game *game, t_player *player)
{
	double	new_x;
	double	new_y;
	double	move_speed;

	move_speed = MOVE_SPEED;
	new_x = player->pos_x + player->plane_x * move_speed;
	new_y = player->pos_y + player->plane_y * move_speed;
	if (can_enter(game->map.grid[(int)player->pos_y][(int)new_x]))
		player->pos_x = new_x;
	if (can_enter(game->map.grid[(int)new_y][(int)player->pos_x]))
		player->pos_y = new_y;
}
