/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 12:02:25 by kfan              #+#    #+#             */
/*   Updated: 2025/05/04 15:14:18 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

void	print_example(char *temp)
{
	if (temp)
		write(2, temp, ft_strlen(temp));
	write(2, ".cub example: \n", 15);
	write(2, "NO ./path_to_the_north_texture\n", 31);
	write(2, "SO ./path_to_the_south_texture\n", 31);
	write(2, "WE ./path_to_the_west_texture\n", 30);
	write(2, "EA ./path_to_the_east_texture\n\n", 31);
	write(2, "F 220,100,0\n", 12);
	write(2, "C 225,30,0\n\n", 12);
	write(2, "111111\n", 7);
	write(2, "100101\n", 7);
	write(2, "101001\n", 7);
	write(2, "1100N1\n", 7);
	write(2, "111111\n", 7);
}

void	identifier_error(int x, int line)
{
	if (x == -2)
		ft_dprintf(2, "Error\nidentifier[index: %d] is missing\n", line);
	if (x == -1)
		ft_dprintf(2, "Error\nmap should be a .cub file\n");
	if (x == 0)
		ft_dprintf(2, "Error\nline %d: identifier \"NO\" already exists\n",
			line);
	if (x == 1)
		ft_dprintf(2, "Error\nline %d: identifier \"SO\" already exists\n",
			line);
	if (x == 2)
		ft_dprintf(2, "Error\nline %d: identifier \"WE\" already exists\n",
			line);
	if (x == 3)
		ft_dprintf(2, "Error\nline %d: identifier \"EA\" already exists\n",
			line);
	if (x == 4)
		ft_dprintf(2, "Error\nline %d: identifier \"F\" already exists\n",
			line);
	if (x == 5)
		ft_dprintf(2, "Error\nline %d: identifier \"C\" already exists\n",
			line);
	print_example(NULL);
}

void	colour_error(int i, int x, char **temp, int line)
{
	ft_free_split(temp);
	if (x == 4)
		ft_dprintf(2, "Error\nline %d: identifier \"F\": ", line);
	else if (x == 5)
		ft_dprintf(2, "Error\nline %d: identifier \"C\": ", line);
	if (i == 0)
		ft_dprintf(2, "fisrt colour ", 13);
	else if (i == 1)
		ft_dprintf(2, "second colour ", 14);
	else if (i == 2)
		ft_dprintf(2, "third colour ", 13);
	else if (i == 3)
		ft_dprintf(2, "there should only be 3 numbers\n", 31);
	if (i < 3)
		ft_dprintf(2, "should be in the range of 0 - 255\n", 34);
	print_example(NULL);
}
