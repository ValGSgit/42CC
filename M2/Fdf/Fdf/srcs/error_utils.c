/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:44 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 16:05:29 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

void	my_mlx_pixel_put(t_mlx *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bpp / 8));
	*(unsigned int *) dst = color;
}

int	invalid(const char *s)
{
	int	i;

	i = 0;
	while (s[i] && s[i] != '.')
		i++;
	if (i >= 0 && !ft_strcmp(s + i + 1, "fdf"))
		return (0);
	return (1);
}

void	put_err(const char *err)
{
	size_t	i;

	i = 0;
	while (err[i])
	{
		write(STDERR_FILENO, &err[i], 1);
		i++;
	}
	exit(EXIT_FAILURE);
}

int	open_file(char *file_name, int permissions)
{
	int	fd;

	fd = open(file_name, permissions);
	if (fd < 0)
	{
		close(fd);
		put_err("File error\n");
	}
	return (fd);
}

int	clear(t_data *data)
{
	mlx_destroy_image(data->mlx_ptr, data->img.img);
	mlx_destroy_window(data->mlx_ptr, data->win_ptr);
	mlx_destroy_display(data->mlx_ptr);
	free_matrix(data->matrix);
	free(data->mlx_ptr);
	free(data);
	ft_putstr_fd("Program closed succesfully :)\n", STDERR_FILENO);
	exit(0);
}
