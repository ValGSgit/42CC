/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:39 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 16:04:47 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	get_dimensions(t_matrix **matrix)
{
	int	y;
	int	x;

	x = 0;
	y = 0;
	while (matrix[y] && !matrix[y][x].last)
		x++;
	while (matrix[y])
		y++;
	return (y + x);
}

void	init_data(t_data *data, t_matrix **matrix)
{
	if (!matrix)
		put_err("Malloc failed :(\n");
	data->matrix = matrix;
	data->dimensions = get_dimensions(matrix);
	data->scale = get_scale(data->dimensions) * 15;
	data->height = get_heights(data);
	data->is_isometric = 1;
	data->angle = 0.666420;
	data->win_x = 1200;
	data->win_y = 720;
	data->target_x = (data->win_x / 3) - 20;
	data->target_y = data->win_y / 3;
	data->color = C114;
	data->mlx_ptr = mlx_init();
	if (!data->mlx_ptr)
		put_err("Minilibx failed to initialize :(\n");
	data->win_ptr = mlx_new_window(data->mlx_ptr, data->win_x, data->win_y,
			TITLE);
	if (!data->win_ptr)
		put_err("Error opening window :(\n");
}

int	main(int ac, char **av)
{
	t_matrix	**matrix;
	t_data		*data;

	if (ac != 2)
		put_err("Invalid format... Valid looks like: ./fdf [map.42]\n");
	else if (invalid(av[1]))
		put_err("Invalid file extension [.fdf]\n");
	else if (!map_check(av[1]))
		put_err("Invalid map :(\n");
	matrix = process_map(av[1]);
	if (!matrix)
		return (free_matrix(matrix), put_err("Malloc failed :("), exit(1), 1);
	data = (t_data *)ft_calloc(1, sizeof(t_data));
	if (!data)
		return (put_err("Malloc failed :("), free_matrix(matrix), exit(1), 1);
	init_data(data, matrix);
	data->img.img = mlx_new_image(data->mlx_ptr, data->win_x, data->win_y);
	data->img.addr = mlx_get_data_addr(data->img.img, &data->img.bpp,
			&data->img.line_length, &data->img.order);
	put_matrix(matrix, data);
	mlx_hook(data->win_ptr, 17, 0, clear, data);
	mlx_hook(data->win_ptr, 2, 1L << 0, if_key, data);
	mlx_loop(data->mlx_ptr);
}
