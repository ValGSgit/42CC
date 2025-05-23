/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:10 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/22 16:22:23 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	allocate_texture_data(t_game *game)
{
	int	i;

	game->texture_data = (int **)malloc(sizeof(int *) * 5);
	if (!game->texture_data)
		error_exit(game, ERR_MEMORY);
	i = 0;
	while (i < 4)
	{
		game->texture_data[i] = (int *)malloc(sizeof(int) * (TEX_WIDTH
					* TEX_HEIGHT));
		if (!game->texture_data[i])
			error_exit(game, ERR_MEMORY);
		i++;
	}
	game->texture_data[i] = NULL;
}

// Use nearest neighbor scaling to sample from the source texture
// Clamp source coordinates to valid range
void	scaling_pixel(t_game *game, int tex_num, int x, int y)
{
	int		src_x;
	int		src_y;
	int		pixel;
	double	scale_x;
	double	scale_y;

	scale_x = (double)game->textures[tex_num].img.width / TEX_WIDTH;
	scale_y = (double)game->textures[tex_num].img.height / TEX_HEIGHT;
	src_x = (int)(x * scale_x);
	src_y = (int)(y * scale_y);
	if (src_x >= game->textures[tex_num].img.width)
		src_x = game->textures[tex_num].img.width - 1;
	if (src_y >= game->textures[tex_num].img.height)
		src_y = game->textures[tex_num].img.height - 1;
	pixel = (src_y * game->textures[tex_num].img.line_length / 4 + src_x);
	game->texture_data[tex_num][y * TEX_WIDTH + x] = ((int *)
			game->textures[tex_num].img.addr)[pixel];
}

// Calculate scaling factors
// if the texture size doesn't match expected dimensions
void	scaling_texture(t_game *game, int tex_num, int x, int y)
{
	double	scale_x;
	double	scale_y;

	scale_x = (double)game->textures[tex_num].img.width / TEX_WIDTH;
	scale_y = (double)game->textures[tex_num].img.height / TEX_HEIGHT;
	while (y < TEX_HEIGHT)
	{
		x = 0;
		while (x < TEX_WIDTH)
		{
			scaling_pixel(game, tex_num, x, y);
			x++;
		}
		y++;
	}
	mlx_destroy_image(game->mlx, game->textures[tex_num].img.img);
	game->textures[tex_num].img.img = NULL;
}

// Replace access check with open check
void	load_texture(t_game *game, int tex_num)
{
	game->textures[tex_num].img.img = mlx_xpm_file_to_image(game->mlx,
			game->textures[tex_num].path, &game->textures[tex_num].img.width,
			&game->textures[tex_num].img.height);
	if (!game->textures[tex_num].img.img)
	{
		ft_dprintf(2, "Error\nFailed to load texture '%s'\n",
			game->textures[tex_num].path);
		error_exit(game, ERR_TEX_LOAD);
	}
	game->textures[tex_num].img.addr = mlx_get_data_addr(
			game->textures[tex_num].img.img,
			&game->textures[tex_num].img.bits_per_pixel,
			&game->textures[tex_num].img.line_length,
			&game->textures[tex_num].img.endian);
	if (!game->textures[tex_num].img.addr)
		error_exit(game, ERR_MEMORY);
	scaling_texture(game, tex_num, 0, 0);
}

void	load_textures(t_game *game)
{
	int	i;

	allocate_texture_data(game);
	i = 0;
	while (i < 4)
	{
		load_texture(game, i);
		i++;
	}
	if (BONUS)
		load_textures_bonus(game);
}
