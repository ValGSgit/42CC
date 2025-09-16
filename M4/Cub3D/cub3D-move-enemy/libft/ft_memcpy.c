/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 14:14:21 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 20:53:27 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t				i;
	unsigned char		*p1;
	const unsigned char	*p2;

	if (!dest && !src)
		return (NULL);
	i = 0;
	p1 = dest;
	p2 = src;
	while (i < n)
	{
		p1[i] = p2[i];
		i++;
	}
	return (dest);
}

/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	(void)argc;
	char str1[11] = "1234567890";
	char str2[11] = "1234567890";
	char *out1;
	char *out2;
	int dest = atoi(argv[1]);
	int src = atoi(argv[2]);
	int n = atoi(argv[3]);

	out1 = memcpy(&str1[dest], &str1[src], n);
	out2 = ft_memcpy(&str2[dest], &str2[src], n);
	printf("%s\n", out1);
	printf("%s\n", out2);
	return (0);
}
*/
