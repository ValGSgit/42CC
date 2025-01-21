/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:39 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/15 17:09:03 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	is_colored_map(t_pixl **matrix)
{
	int	i;
	int	j;

	i = 0;
	while (matrix[i])
	{
		j = 0;
		while (!matrix[i][j].is_last)
		{
			if (matrix[i][j].color != -1)
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

int	get_map_scale(t_pixl **matrix)
{
	int	y;
	int	x;

	x = 0;
	y = 0;
	while (!matrix[y][x].is_last)
		x++;
	while (matrix[y])
		y++;
	return (y + x);
}

void	init_data(t_data *data, t_pixl **matrix)
{
	data->matrix = matrix;
	data->map_scale = get_map_scale(matrix);
	data->scale = get_scale(data) * 15;
	data->z_scale = get_z_scale(data);
	data->is_isometric = 1;
	data->random_color = 0;
	data->angle = 0.663599;
	data->win_x = 1300;
	data->win_y = 740;
	data->shift_x = (data->win_x / 3) - 20;
	data->shift_y = data->win_y / 3;
	data->color = 1;
	data->mlx_ptr = mlx_init();
	data->is_colors = is_colored_map(matrix);
	if (!data->mlx_ptr)
		put_err(MLX_ERR);
	data->win_ptr = \
	mlx_new_window(data->mlx_ptr, data->win_x, data->win_y, TITLE);
	if (!data->win_ptr)
		put_err(WIN_ERR);
}

int	ft_exit(t_data *data)
{
	mlx_destroy_image(data->mlx_ptr, data->img.img);
	mlx_destroy_window(data->mlx_ptr, data->win_ptr);
	mlx_destroy_display(data->mlx_ptr);
	free_matrix(data->matrix);
	free(data->mlx_ptr);
	free(data);
	ft_putstr_fd(END_MSG, 2);
	exit(EXIT_SUCCESS);
}

int	main(int ac, char **av)
{
	t_pixl			**matrix;
	t_data			*data;

	if (ac != 2)
		put_err(INPUT_ERR);
	else if (invalid(av[1]))
		put_err(EXTENSION_ERR);
	else if (map_check(av[1]))
		put_err(INVALID_MAP_ERR);
	matrix = process_map(av[1]);
	data = (t_data *)malloc_check(1, sizeof(t_data));
	init_data(data, matrix);
	data->img.img = mlx_new_image(data->mlx_ptr, data->win_x, data->win_y);
	data->img.addr = mlx_get_data_addr(data->img.img, \
	&data->img.bits_per_pixel, &data->img.line_length, &data->img.order);
	draw(matrix, data);
	mlx_hook(data->win_ptr, 17, 0, ft_exit, data);
	mlx_hook(data->win_ptr, 2, 1L << 0, if_key, data);
	mlx_loop(data->mlx_ptr);
}
