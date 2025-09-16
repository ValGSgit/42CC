/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprite_draw_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:12:44 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 21:48:29 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/*
** Calculate sprite position and transformation values
*/
void	calc_sprite_transform(t_game *game, t_sprite_draw *draw, \
	int sprite_idx, int player_idx)
{
	if (!game->textures_bonus[game->sprites[sprite_idx].texture].path)
		return ;
	draw->sprite_x = game->sprites[sprite_idx].x \
		- game->player[player_idx].pos_x;
	draw->sprite_y = game->sprites[sprite_idx].y \
		- game->player[player_idx].pos_y;
	draw->inv_det = 1.0 / (game->player[player_idx].plane_x \
		* game->player[player_idx].dir_y - game->player[player_idx].dir_x \
		* game->player[player_idx].plane_y);
	draw->transform_x = draw->inv_det * (game->player[player_idx].dir_y \
		* draw->sprite_x - game->player[player_idx].dir_x * draw->sprite_y);
	draw->transform_y = draw->inv_det * (-game->player[player_idx].plane_y \
		* draw->sprite_x + game->player[player_idx].plane_x * draw->sprite_y);
}

/*
** Calculate sprite dimensions and screen position
// Calculate sprite screen position
// Calculate sprite dimensions on screen
// Adjust the size of different sprite types for better visibility
// Check if sprite is an enemy car (any of the three types)
// Make enemy car sprites larger
// Make player sprites slightly larger
// Make coins slightly smaller but still visible
// Make doors slightly larger
*/
static void	line_saver(t_game *game, t_sprite_draw *draw, \
		int sprite_index)
{
	if (game->sprites[sprite_index].texture == SPRITE_PLAYER1
		|| game->sprites[sprite_index].texture == SPRITE_PLAYER2)
	{
		draw->sprite_height = (int)(draw->sprite_height * 1.3);
		draw->sprite_width = (int)(draw->sprite_width * 1.3);
	}
	else if (game->sprites[sprite_index].texture == SPRITE_COIN)
	{
		draw->sprite_height = (int)(draw->sprite_height * 0.7);
		draw->sprite_width = (int)(draw->sprite_width * 0.7);
	}
	else if (game->sprites[sprite_index].texture == SPRITE_DOOR)
	{
		draw->sprite_height = (int)(draw->sprite_height * 1.2);
		draw->sprite_width = (int)(draw->sprite_width * 1.2);
	}
}

void	calc_sprite_dimensions(t_game *game, t_sprite_draw *draw, \
		int sprite_index)
{
	draw->sprite_screen_x = (int)((WIN_WIDTH / 2) \
		* (1 + draw->transform_x / draw->transform_y));
	draw->sprite_height = abs((int)(WIN_HEIGHT / draw->transform_y));
	draw->sprite_width = abs((int)(WIN_HEIGHT / draw->transform_y));
	if (game->sprites[sprite_index].texture == SPRITE_ENEMY
		||game->sprites[sprite_index].texture == SPRITE_ENEMY_FIAT)
	{
		draw->sprite_height = (int)(draw->sprite_height * 1.5);
		draw->sprite_width = (int)(draw->sprite_width * 1.5);
	}
	line_saver(game, draw, sprite_index);
}

/*
** Calculate sprite drawing boundaries
*/
// Calculate drawing boundaries - fixed height regardless of camera tilt
// For all sprites, we want them to remain at a fixed vertical position
// This way coins and other sprites don't float up/down when looking up/down
void	calc_sprite_boundaries(t_sprite_draw *draw, t_game *game, \
		int player_index)
{
	draw->draw_start_y = -draw->sprite_height / 2 + WIN_HEIGHT / 2 \
		+ game->camera_y[player_index];
	if (draw->draw_start_y < 0)
		draw->draw_start_y = 0;
	draw->draw_end_y = draw->sprite_height / 2 + WIN_HEIGHT / 2 \
		+ game->camera_y[player_index];
	if (draw->draw_end_y >= WIN_HEIGHT)
		draw->draw_end_y = WIN_HEIGHT - 1;
	draw->draw_start_x = -draw->sprite_width / 2 + draw->sprite_screen_x;
	if (draw->draw_start_x < 0)
		draw->draw_start_x = 0;
	draw->draw_end_x = draw->sprite_width / 2 + draw->sprite_screen_x;
	if (draw->draw_end_x >= WIN_WIDTH)
		draw->draw_end_x = WIN_WIDTH - 1;
}

/*
* Draw a single sprite by coordinating all the sprite rendering steps
* Step 1: Calculate the sprite transformation 
* Step 2: Calculate sprite dimensions based on distance and type
* Step 3: Calculate drawing boundaries on screen
* Step 4: Render each vertical stripe of the sprite
* Step 5: Draw the sprite on the screen
*/
void	draw_sprite(t_game *game, int sprite_index, int player_index)
{
	int	stripe;

	game->draw = ft_calloc(1, sizeof(t_sprite_draw));
	if (!game->draw)
		error_exit(game, ERR_MEMORY);
	calc_sprite_transform(game, game->draw, sprite_index, player_index);
	calc_sprite_dimensions(game, game->draw, sprite_index);
	calc_sprite_boundaries(game->draw, game, player_index);
	stripe = game->draw->draw_start_x;
	while (stripe < game->draw->draw_end_x)
	{
		render_sprite_stripe(game, stripe, sprite_index, player_index);
		stripe++;
	}
	free(game->draw);
}
