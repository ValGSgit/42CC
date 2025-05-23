/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 16:42:46 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/23 17:44:35 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	allocate_texture_data_bonus(t_game *game)
{
	int	i;

	game->texture_data_bonus = (int **)malloc(sizeof(int *) * 11);
	if (!game->texture_data_bonus)
		error_exit(game, ERR_MEMORY);
	i = 0;
	while (i < 10)
	{
		game->texture_data_bonus[i] = (int *)malloc(sizeof(int) * (TEX_WIDTH
					* TEX_HEIGHT));
		if (!game->texture_data_bonus[i])
			error_exit(game, ERR_MEMORY);
		i++;
	}
	game->texture_data_bonus[i] = NULL;
}

// Use nearest neighbor scaling to sample from the source texture
// Clamp source coordinates to valid range
static void	scaling_pixel_bonus(t_game *game, int tex_num, int x, int y)
{
	int		src_x;
	int		src_y;
	int		pixel;
	double	scale_x;
	double	scale_y;

	scale_x = (double)game->textures_bonus[tex_num].img.width / TEX_WIDTH;
	scale_y = (double)game->textures_bonus[tex_num].img.height / TEX_HEIGHT;
	src_x = (int)(x * scale_x);
	src_y = (int)(y * scale_y);
	if (src_x >= game->textures_bonus[tex_num].img.width)
		src_x = game->textures_bonus[tex_num].img.width - 1;
	if (src_y >= game->textures_bonus[tex_num].img.height)
		src_y = game->textures_bonus[tex_num].img.height - 1;
	pixel = (src_y * game->textures_bonus[tex_num].img.line_length / 4 + src_x);
	game->texture_data_bonus[tex_num][y * TEX_WIDTH + x] = ((int *)
			game->textures_bonus[tex_num].img.addr)[pixel];
}

// Calculate scaling factors
// if the texture size doesn't match expected dimensions
static void	scaling_texture_bonus(t_game *game, int tex_num, int x, int y)
{
	while (y < TEX_HEIGHT)
	{
		x = 0;
		while (x < TEX_WIDTH)
		{
			scaling_pixel_bonus(game, tex_num, x, y);
			x++;
		}
		y++;
	}
	mlx_destroy_image(game->mlx, game->textures_bonus[tex_num].img.img);
	game->textures_bonus[tex_num].img.img = NULL;
}

static void	load_texture_bonus(t_game *game, int tex_num)
{
	game->textures_bonus[tex_num].img.img = mlx_xpm_file_to_image(game->mlx,
			game->textures_bonus[tex_num].path, \
			&game->textures_bonus[tex_num].img.width,
			&game->textures_bonus[tex_num].img.height);
	if (!game->textures_bonus[tex_num].img.img)
	{
		ft_dprintf(2, "Error\nFailed to load texture '%s'\n",
			game->textures_bonus[tex_num].path);
		error_exit(game, ERR_TEX_LOAD);
	}
	game->textures_bonus[tex_num].img.addr = mlx_get_data_addr(
			game->textures_bonus[tex_num].img.img,
			&game->textures_bonus[tex_num].img.bits_per_pixel,
			&game->textures_bonus[tex_num].img.line_length,
			&game->textures_bonus[tex_num].img.endian);
	if (!game->textures_bonus[tex_num].img.addr)
		error_exit(game, ERR_MEMORY);
	scaling_texture_bonus(game, tex_num, 0, 0);
}

// Load additional sprite textures with proper assignments
// Players use p1.xpm for both players
// Enemy car textures - use three different car models as requested
// Use the coin sprite from folder 8
// Use gun.xpm, we've already modified it for proper transparency
// Add door sprite
// Load the sprite textures including the gun, door and all enemy types
// Display information about loaded sprite textures
// If all three car textures are loaded, display additional information
// validate_upload can be quoted out since its just for debugging
void	load_textures_bonus(t_game *game)
{
	int	i;

	allocate_texture_data_bonus(game);
	game->textures_bonus[SPRITE_PLAYER1].path = \
			ft_strdup("textures/bonus/p1.xpm");
	game->textures_bonus[SPRITE_PLAYER2].path = \
			ft_strdup("textures/bonus/p2.xpm");
	game->textures_bonus[SPRITE_ENEMY].path = \
			ft_strdup("textures/bonus/car1.xpm");
	game->textures_bonus[SPRITE_ENEMY_BENTLEY].path = \
			ft_strdup("textures/bonus/bentley.xpm");
	game->textures_bonus[SPRITE_ENEMY_FIAT].path = \
			ft_strdup("textures/bonus/fiat.xpm");
	game->textures_bonus[SPRITE_COIN].path = \
			ft_strdup("textures/bonus/coin.xpm");
	game->textures_bonus[SPRITE_GUN].path = \
			ft_strdup("textures/bonus/gun.xpm");
	game->textures_bonus[SPRITE_DOOR].path = \
			ft_strdup("textures/bonus/door.xpm");
	i = -1;
	while (++i <= SPRITE_ENEMY_FIAT)
	{
		if (game->textures_bonus[i].path)
			load_texture_bonus(game, i);
	}
}
