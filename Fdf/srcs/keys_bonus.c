/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:47 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 17:34:13 by vagarcia         ###   ########.fr       */
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
	data->scale = get_scale(data) * 15;
	data->z_scale = get_z_scale(data);
	data->angle = 0.663599;
	data->shift_x = (data->win_x / 3) - 20;
	data->shift_y = data->win_y / 3;
}

void	more_keys(int key, t_data *data)
{
	if (key == XK_Left)
		data->shift_x -= 10;
	else if (key == XK_Right)
		data->shift_x += 10;
	else if (key == XK_Up)
		data->shift_y -= 10;
	else if (key == XK_Down)
		data->shift_y += 10;
	else if (key == XK_a)
		data->angle += 0.07;
	else if (key == XK_d)
		data->angle -= 0.07;
	else if (key == XK_q)
		data->z_scale += get_z_scale(data) / 2;
	else if (key == XK_w)
		data->z_scale -= get_z_scale(data) / 2;
	else if (key == XK_z)
		data->scale += get_scale(data);
	else if (key == XK_x)
		data->scale -= get_scale(data);
}

void	do_key(int key, t_data *data)
{
	more_keys(key, data);
	if (key == XK_Return)
		reset(data);
	if (key == XK_2 || key == XK_3)
		data->is_isometric = !data->is_isometric;
	if (key == XK_c)
		change_color(data);
	if (key == XK_Escape)
		ft_exit(data);
}

int	if_key(int key, t_data *data)
{
	if (is_key(key))
	{
		mlx_clear_window(data->mlx_ptr, data->win_ptr);
		do_key(key, data);
		print_option(data);
		draw(data->matrix, data);
	}
	return (0);
}
