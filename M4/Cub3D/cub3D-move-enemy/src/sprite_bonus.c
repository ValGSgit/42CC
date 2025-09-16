/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprite_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 19:31:07 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/28 21:56:24 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	get_texy(int d, t_sprite_draw *draw, t_game *game, int player_index)
{
	int	tex_y;

	tex_y = ((d * TEX_HEIGHT) / draw->sprite_height) / 256;
	tex_y -= (game->camera_y[player_index] * 256 / draw->sprite_height);
	if (tex_y < 0)
		tex_y = 0;
	if (tex_y >= TEX_HEIGHT)
		tex_y = TEX_HEIGHT - 1;
	return (tex_y);
}

/*
** Render a single vertical stripe of a sprite
*/
void	render_sprite_stripe(t_game *game, int stripe, int sprite_idx, \
	int player_idx)
{
	int		tex_x;
	int		tex_y;
	int		y;
	int		d;
	int		color;

	tex_x = (int)((256 * (stripe - \
		(-game->draw->sprite_width / 2 + game->draw->sprite_screen_x)) \
		* TEX_WIDTH / game->draw->sprite_width) / 256);
	if (game->draw->transform_y > 0 && stripe > 0 && stripe < WIN_WIDTH
		&& game->draw->transform_y < game->zbuffer[stripe])
	{
		y = game->draw->draw_start_y;
		while (y < game->draw->draw_end_y)
		{
			d = y * 256 - WIN_HEIGHT * 128 + game->draw->sprite_height * 128;
			tex_y = get_texy(d, game->draw, game, player_idx);
			color = game->texture_data_bonus[game->sprites[sprite_idx].texture] \
					[tex_y * TEX_WIDTH + tex_x];
			if ((color & 0x00FFFFFF) != 0 && color != 0x00000000)
				my_mlx_pixel_put(&game->img, stripe, y, color);
			y++;
		}
	}
}

/*
** Cast all sprites for a specific player (i)
// Before drawing, sort the sprites based on distance
// Log the number of sprites to be drawn
// Draw each sprite in order (from far to close)
*/
void	cast_sprites(t_game *game, int i)
{
	int		j;
	int		sprite_idx;

	sort_sprites(game, i);
	j = 0;
	sprite_idx = 0;
	while (j < game->sprite_count)
	{
		sprite_idx = game->sprite_order[j];
		if (game->sprites[sprite_idx].visible)
			draw_sprite(game, sprite_idx, i);
		j++;
	}
}

/*
** Update sprite positions (especially moving sprites like players)
*/
// Update sprite positions in both single and multiplayer modes
// Update door state (if doors have changed states)
// Round to get integer position for map coordinates
// Check if door has been opened/closed on the map and update visibility
// Door is closed - show sprite
// Door is open - still show sprite but could be modified for visual difference
// Door might have been removed from map - hide sprite
// Update player positions in multiplayer mode
// Update player position in single player mode with enemies

static void	update_bonus(t_game *game, int bonus_type)
{
	if (bonus_type == 2)
	{
		game->sprites[game->player[0].index].x = game->player[0].pos_x;
		game->sprites[game->player[0].index].y = game->player[0].pos_y;
		game->sprites[game->player[1].index].x = game->player[1].pos_x;
		game->sprites[game->player[1].index].y = game->player[1].pos_y;
	}
}

void	update_sprites(t_game *game)
{
	int		i;
	int		door_x;
	int		door_y;

	i = 0;
	while (i < game->sprite_count)
	{
		if (game->sprites[i].texture == SPRITE_DOOR)
		{
			door_x = (int)(game->sprites[i].x);
			door_y = (int)(game->sprites[i].y);
			if (game->map.grid[door_y][door_x] == 'C')
				game->sprites[i].visible = 1;
			else if (game->map.grid[door_y][door_x] == 'O')
				game->sprites[i].visible = 1;
			else
				game->sprites[i].visible = 0;
		}
		if (BONUS == 1 && game->sprites[i].texture == SPRITE_ENEMY)
			move_enemy_x(game, i);
		if (BONUS == 1 && game->sprites[i].texture == SPRITE_ENEMY_FIAT)
			move_enemy_y(game, i);
		i++;
	}
	update_bonus(game, BONUS);
}
