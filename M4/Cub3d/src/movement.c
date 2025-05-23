/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:37:58 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/21 14:31:08 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

// Rotate camera direction vector using a rotation matrix
// Rotate camera plane vector using the same rotation matrix
void	rotate_left(t_player *player)
{
	double	old_dir_x;
	double	old_plane_x;
	double	rot_speed;

	rot_speed = ROT_SPEED;
	old_dir_x = player->dir_x;
	player->dir_x = player->dir_x * cos(-rot_speed)
		- player->dir_y * sin(-rot_speed);
	player->dir_y = old_dir_x * sin(-rot_speed) + player->dir_y
		* cos(-rot_speed);
	old_plane_x = player->plane_x;
	player->plane_x = player->plane_x * cos(-rot_speed)
		- player->plane_y * sin(-rot_speed);
	player->plane_y = old_plane_x * sin(-rot_speed) + player->plane_y
		* cos(-rot_speed);
}

void	rotate_right(t_player *player)
{
	double	old_dir_x;
	double	old_plane_x;
	double	rot_speed;

	rot_speed = ROT_SPEED;
	old_dir_x = player->dir_x;
	player->dir_x = player->dir_x * cos(rot_speed)
		- player->dir_y * sin(rot_speed);
	player->dir_y = old_dir_x * sin(rot_speed) + player->dir_y
		* cos(rot_speed);
	old_plane_x = player->plane_x;
	player->plane_x = player->plane_x * cos(rot_speed)
		- player->plane_y * sin(rot_speed);
	player->plane_y = old_plane_x * sin(rot_speed) + player->plane_y
		* cos(rot_speed);
}

// Debug information to see which key was pressed
//printf("Key pressed: %d\n", keycode)
// Dont Re-render the frame after movement
// render_frame(game); < it will slow down the game
int	key_press(int keycode, t_game *game)
{
	if (keycode == KEY_ESC)
		exit_game(game);
	if (BONUS == 2)
		return (key_press_bonus_2p(keycode, game));
	if (keycode == KEY_W)
		game->move[W] = 1;
	else if (keycode == KEY_S)
		game->move[S] = 1;
	else if (keycode == KEY_A)
		game->move[A] = 1;
	else if (keycode == KEY_D)
		game->move[D] = 1;
	else if (keycode == KEY_LEFT)
		game->move[LEFT] = 1;
	else if (keycode == KEY_RIGHT)
		game->move[RIGHT] = 1;
	else if (BONUS && keycode == KEY_UP)
		game->move[UP] = 1;
	else if (BONUS && keycode == KEY_DOWN)
		game->move[DOWN] = 1;
	else if (BONUS && keycode == KEY_SPACE)
		fire(game, 0);
	else if (BONUS && keycode == KEY_C)
		door(game, 0);
	return (0);
}

int	key_release(int keycode, t_game *game)
{
	if (BONUS == 2)
		return (key_release_bonus_2p(keycode, game));
	if (keycode == KEY_W)
		game->move[W] = 0;
	else if (keycode == KEY_S)
		game->move[S] = 0;
	else if (keycode == KEY_A)
		game->move[A] = 0;
	else if (keycode == KEY_D)
		game->move[D] = 0;
	else if (keycode == KEY_LEFT)
		game->move[LEFT] = 0;
	else if (keycode == KEY_RIGHT)
		game->move[RIGHT] = 0;
	else if (BONUS && keycode == KEY_UP)
		game->move[UP] = 0;
	else if (BONUS && keycode == KEY_DOWN)
		game->move[DOWN] = 0;
	return (0);
}
