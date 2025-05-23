/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_texture.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:10 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/22 15:22:26 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	free_textures_bonus(t_game *game)
{
	int	i;

	if (!game->texture_data_bonus)
		return ;
	i = 0;
	while (game->texture_data_bonus[i])
	{
		if (game->texture_data_bonus[i])
			free(game->texture_data_bonus[i]);
		i++;
	}
	if (game->texture_data_bonus)
		free(game->texture_data_bonus);
	game->texture_data_bonus = NULL;
}

void	free_textures(t_game *game)
{
	int	i;

	if (!game->texture_data)
		return ;
	i = 0;
	while (game->texture_data[i])
	{
		if (game->texture_data[i])
			free(game->texture_data[i]);
		i++;
	}
	if (game->texture_data)
	{
		free(game->texture_data);
	}
}

// tex_num is door
int	get_texture_color(t_game *game, int tex_num, int tex_x, int tex_y)
{
	if (BONUS && tex_num == 4 && game->textures_bonus[0].path)
		return (game->texture_data_bonus[0][TEX_WIDTH * tex_y + tex_x]);
	if (BONUS && tex_num == 5 && game->textures_bonus[1].path)
		return (game->texture_data_bonus[1][TEX_WIDTH * tex_y + tex_x]);
	if (BONUS && tex_num == 6 && game->textures_bonus[2].path)
		return (game->texture_data_bonus[2][TEX_WIDTH * tex_y + tex_x]);
	if (BONUS && tex_num == 7 && game->textures_bonus[3].path)
		return (game->texture_data_bonus[3][TEX_WIDTH * tex_y + tex_x]);
	if (tex_x < 0 || tex_x >= TEX_WIDTH || tex_y < 0 || tex_y >= TEX_HEIGHT
		|| tex_num < 0 || tex_num >= 4)
		return (0);
	return (game->texture_data[tex_num][TEX_WIDTH * tex_y + tex_x]);
}
