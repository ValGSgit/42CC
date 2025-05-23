/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:06 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/21 19:26:59 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	update_map(t_game *game, t_player *player1, t_player *player2)
{
	game->map.grid[(int)game->player[0].pos_y][(int)game->player[0].pos_x] \
		= 'F';
	game->map.grid[(int)game->player[1].pos_y][(int)game->player[1].pos_x] \
		= 'D';
	if ((int)game->player[0].pos_x != (int)player1->pos_x
		|| (int)game->player[0].pos_y != (int)player1->pos_y)
		game->map.grid[(int)player1->pos_y][(int)player1->pos_x] = '0';
	if ((int)game->player[1].pos_x != (int)player2->pos_x
		|| (int)game->player[1].pos_y != (int)player2->pos_y)
		game->map.grid[(int)player2->pos_y][(int)player2->pos_x] = '0';
}

void	save_pos(t_game *game, t_player *player1, t_player *player2)
{
	player1->pos_x = game->player[0].pos_x;
	player1->pos_y = game->player[0].pos_y;
	player2->pos_x = game->player[1].pos_x;
	player2->pos_y = game->player[1].pos_y;
	update_move(game);
}

// splitted part of render_frame to meet norminette, still works
static int	render_player2(t_game *game)
{
	game->img.img = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
	if (!game->img.img)
		return (1);
	game->img.addr = mlx_get_data_addr(game->img.img, &game->img.bits_per_pixel,
			&game->img.line_length, &game->img.endian);
	if (!game->img.addr)
		return (1);
	cast_rays(game, 1);
	draw_bonus(game, 1);
	mlx_put_image_to_window(game->mlx, game->win, game->img.img, WIN_WIDTH, 0);
	mlx_destroy_image(game->mlx, game->img.img);
	game->img.img = NULL;
	return (0);
}

static int	render_player1(t_game *game)
{
	game->img.img = mlx_new_image(game->mlx, WIN_WIDTH, WIN_HEIGHT);
	if (!game->img.img)
		return (1);
	game->img.addr = mlx_get_data_addr(game->img.img, &game->img.bits_per_pixel,
			&game->img.line_length, &game->img.endian);
	if (!game->img.addr)
		return (1);
	cast_rays(game, 0);
	if (BONUS)
		draw_bonus(game, 0);
	mlx_put_image_to_window(game->mlx, game->win, game->img.img, 0, 0);
	mlx_destroy_image(game->mlx, game->img.img);
	game->img.img = NULL;
	return (0);
}

int	render_frame(t_game *game)
{
	t_player	player1;
	t_player	player2;

	save_pos(game, &player1, &player2);
	if (BONUS)
		get_coin(game, 0);
	if (BONUS == 2)
	{
		get_coin(game, 1);
		update_map(game, &player1, &player2);
		update_sprites(game);
	}
	if (game->win == NULL)
		return (1);
	if (render_player1(game))
		error_exit(game, ERR_MEMORY);
	if (BONUS != 2)
		return (0);
	if (render_player2(game))
		error_exit(game, ERR_MEMORY);
	return (0);
}
