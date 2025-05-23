/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   weapon.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:26:45 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/22 12:05:17 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	draw_gun_pixel(t_game *game, int x, int y, int offset_y)
{
	int	tex_x;
	int	tex_y;
	int	color;
	int	weapon_width;
	int	weapon_height;

	weapon_width = WIN_WIDTH / 4.0;
	weapon_height = WIN_HEIGHT / 3.0;
	tex_x = (x * TEX_WIDTH) / weapon_width;
	tex_y = ((y * TEX_HEIGHT) / weapon_height);
	if (tex_x >= 0 && tex_x < TEX_WIDTH && tex_y >= 0 && tex_y < TEX_HEIGHT)
	{
		color = game->texture_data_bonus[SPRITE_GUN][TEX_WIDTH * tex_y + tex_x];
		if (color)
		{
			my_mlx_pixel_put(&game->img, WIN_WIDTH - weapon_width + x,
				WIN_HEIGHT - weapon_height + y + offset_y, color);
		}
	}
}

/*
** Draw a weapon/arm sprite with a bobbing effect based on movement
	- SIMPLIFIED VERSION
*/
// Early exit check
// Update bobbing effect based on player movement
// Create a cycle for the bobbing effect
// Calculate vertical offset for bobbing effect
// Reduced amplitude for subtler effect
// Set the weapon dimensions - smaller to stay on screen
// Position the weapon at the bottom right of the screen with bobbing effect
// Calculate the corresponding position in the texture
// Get the pixel color from the texture
// Only draw non-transparent pixels (not black or white)
// Draw the pixel with bobbing effect and better positioning
// offset_y += 4 for getting rid empty pixels at the bottom

void	draw_player_weapon_bobbing(t_game *game)
{
	int			x;
	int			y;
	int			weapon_width;
	int			weapon_height;
	static int	bobbing = 0;

	if (game == NULL || game->texture_data_bonus == NULL)
		return ;
	if (game->move[W] || game->move[S] || game->move[A] || game->move[D])
		bobbing = (bobbing + 1) % 40;
	weapon_width = WIN_WIDTH / 4.0;
	weapon_height = WIN_HEIGHT / 3.0;
	y = 0;
	while (y < weapon_height)
	{
		x = 0;
		while (x < weapon_width)
		{
			draw_gun_pixel(game, x, y, (int)(sin(bobbing * 0.2) * 5) + 4);
			x++;
		}
		y++;
	}
}
