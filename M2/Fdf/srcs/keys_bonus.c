/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:47 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 15:58:01 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	is_key(int key)
{
	if (key == XK_Return || key == XK_2 || key == XK_3 || \
			key == XK_z || key == XK_x || key == XK_c || \
			key == XK_a || key == XK_d || key == XK_q || key == XK_w || \
			key == XK_Left || key == XK_Right || key == XK_Up || key == XK_Down
		|| key == XK_Escape)
		return (1);
	return (0);
}

void	reset(t_data *data)
{
	data->scale = get_scale(data->dimensions) * 15;
	data->height = get_heights(data);
	data->angle = 0.663599;
	data->target_x = (data->win_x / 3) - 20;
	data->target_y = data->win_y / 3;
}

void	more_keys(int key, t_data *data)
{
	if (key == XK_Left)
		data->target_x -= 10;
	else if (key == XK_Right)
		data->target_x += 10;
	else if (key == XK_Up)
		data->target_y -= 10;
	else if (key == XK_Down)
		data->target_y += 10;
	else if (key == XK_a)
		data->angle += 0.07;
	else if (key == XK_d)
		data->angle -= 0.07;
	else if (key == XK_q)
		data->height += get_heights(data) / 2;
	else if (key == XK_w)
		data->height -= get_heights(data) / 2;
	else if (key == XK_z)
		data->scale += get_scale(data->dimensions);
	else if (key == XK_x)
		data->scale -= get_scale(data->dimensions);
}

void	do_key(int key, t_data *data)
{
	more_keys(key, data);
	if (key == XK_Return)
		reset(data);
	if (key == XK_2 || key == XK_3)
		data->is_isometric = !data->is_isometric;
	if (key == XK_c)
		data->color = get_color();
	if (key == XK_Escape)
		clear(data);
}

int	if_key(int key, t_data *data)
{
	if (key == XK_Return || key == XK_2 || key == XK_3 || \
			key == XK_z || key == XK_x || key == XK_c || \
			key == XK_a || key == XK_d || key == XK_q || key == XK_w || \
			key == XK_Left || key == XK_Right || key == XK_Up || key == XK_Down
		|| key == XK_Escape)
	{
		mlx_clear_window(data->mlx_ptr, data->win_ptr);
		do_key(key, data);
		print_keys(data);
		put_matrix(data->matrix, data);
	}
	return (0);
}
