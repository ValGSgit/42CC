/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lines.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:40 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 17:36:24 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

float	*calc_stepups(float flag[2], t_pixl a, t_pixl b)
{
	float	max;

	flag[0] = b.x - a.x;
	flag[1] = b.y - a.y;
	if (abs_value(flag[0]) > abs_value(flag[1]))
		max = abs_value(flag[0]);
	else
		max = abs_value(flag[1]);
	flag[0] = flag[0] / max;
	flag[1] = flag[1] / max;
	return (flag);
}

void	draw_line(t_pixl a, t_pixl b, t_data *param)
{
	float	increment[2];
	int		color;

	init_coords(&a, &b, param);
	calc_stepups(increment, a, b);
	color = get_color(param, a, b);
	while ((int)(a.x - b.x) || (int)(a.y - b.y))
	{
		if (a.x > param->win_x || a.y > param->win_y || a.y < 0 || a.x < 0)
			break ;
		if (limit_x(a.x, param) < param->win_x
			&& limit_y(a.y, param) < param->win_y)
			my_mlx_pixel_put(&param->img, limit_x(a.x, param), limit_y(a.y,
					param), color);
		a.x += increment[0];
		a.y += increment[1];
	}
}

void	push_image_to_window(t_data *data)
{
	void	*previous_image;

	previous_image = data->img.img;
	if (!previous_image)
		ft_exit(data);
	mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->img.img, 0, 0);
	mlx_destroy_image(data->mlx_ptr, previous_image);
	data->img.img = mlx_new_image(data->mlx_ptr, data->win_x, data->win_y);
	data->img.addr = mlx_get_data_addr(data->img.img, &data->img.bits_per_pixel,
			&data->img.line_length, &data->img.order);
}

void	draw(t_pixl **matrix, t_data *data)
{
	int	y;
	int	x;

	y = 0;
	if (!matrix)
		ft_exit(data);
	if (!data)
		ft_exit(data);
	while (matrix[y])
	{
		x = 0;
		while (&matrix[y][x])
		{
			if (matrix[y + 1])
				draw_line(matrix[y][x], matrix[y + 1][x], data);
			if (!matrix[y][x].is_last)
				draw_line(matrix[y][x], matrix[y][x + 1], data);
			if (matrix[y][x].is_last)
				break ;
			x++;
		}
		y++;
	}
	push_image_to_window(data);
	print_option(data);
}

void	free_matrix(t_pixl **matrix)
{
	int	y;

	y = 0;
	if (!matrix)
		return ;
	while (matrix[y])
	{
		free(matrix[y]);
		y++;
	}
	free(matrix);
	matrix = NULL;
}
