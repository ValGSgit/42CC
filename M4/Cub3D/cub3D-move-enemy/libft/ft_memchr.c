/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 13:53:51 by kfan              #+#    #+#             */
/*   Updated: 2024/09/18 17:49:44 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	a;
	unsigned char	b;
	unsigned char	*str;

	i = 0;
	a = c;
	str = (unsigned char *)s;
	while (i < n)
	{
		b = str[i];
		if (b == a)
			return (&str[i]);
		i++;
	}
	return (NULL);
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	main(int argc, char **argv)
{
	unsigned int j;

	j = atoi(argv[3]);
	if (argc == 4)
	{
	printf("src: %s, %c, %c bytes\n", argv[1], atoi(argv[2]), j);
	printf("%s", (char *)ft_memchr(argv[1], atoi(argv[2]), j));
	return (0);
	}
	else
	{
		return (0);
	}
}
*/
