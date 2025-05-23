/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_array.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 12:02:25 by kfan              #+#    #+#             */
/*   Updated: 2025/05/04 15:13:20 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static void	free_if_failed(char **temp, int i)
{
	while (i > 0)
		free(temp[--i]);
	free(temp);
}

char	**add_array(char **input, char *entry, int y)
{
	char	**temp;
	int		i;

	while (input && input[y])
		y++;
	temp = malloc(sizeof(char *) * (y + 2));
	if (!temp)
		return (perror("Error\nadd_array: malloc failed"), NULL);
	temp[y + 1] = NULL;
	i = 0;
	while (i < y)
	{
		temp[i] = ft_strdup(input[i]);
		if (!temp[i])
			return (free_if_failed(temp, i),
				perror("Error\nadd_array: ft_strdup failed"), NULL);
		i++;
	}
	temp[y] = ft_strdup(entry);
	if (!temp[y])
		return (free_if_failed(temp, i),
			perror("Error\nadd_array: ft_strdup failed"), NULL);
	return (temp);
}

char	**copy_map(char **map)
{
	char	**temp;
	int		y;
	int		i;

	i = 0;
	while (map[i])
		i++;
	temp = ft_calloc((i + 1), sizeof(void *));
	if (!temp)
		return (perror("Error\nft_calloc failed"), NULL);
	y = 0;
	while (y < i)
	{
		temp[y] = ft_strdup(map[y]);
		if (!temp[y])
			return (perror("Error\nft_strdup failed"), free_if_failed(temp, y),
				NULL);
		y++;
	}
	return (temp);
}

void	print_array(char **temp)
{
	int	i;

	if (!temp)
		return ;
	i = 0;
	while (temp[i])
	{
		ft_dprintf(2, "%s\n", temp[i]);
		i++;
	}
}
