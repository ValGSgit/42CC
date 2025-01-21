/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:44 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/15 11:31:05 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

void	my_mlx_pixel_put(t_img *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *) dst = color;
}

int	invalid(const char *s)
{
	ssize_t	i;

	i = ft_strlen(s) - 1;
	while (i >= 0 && s[i] != '.')
		i--;
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

void	*malloc_check(size_t size, size_t size_of)
{
	void	*mem;

	mem = malloc(size * size_of);
	if (!mem)
		put_err(MALLOC_ERR);
	return (mem);
}

int	open_file(char *file_name, int permissions)
{
	int	fd;

	fd = open(file_name, permissions);
	if (fd < 0)
	{
		close(fd);
		put_err(FILE_ERR);
	}
	return (fd);
}
