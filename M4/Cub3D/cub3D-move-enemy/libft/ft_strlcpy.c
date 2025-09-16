/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 18:37:54 by kfan              #+#    #+#             */
/*   Updated: 2024/09/09 19:05:34 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size != 0)
	{
		size--;
		while (i < size && src[i] != '\0')
		{
			dst[i] = src[i];
			i++;
		}
		dst[i] = '\0';
	}
	return (ft_strlen(src));
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>

int	main(void)
{
	char dest1[] = "Hello";
	char src1[] = "Worldsssssss";
	char dest2[] = "Hello";
	char src2[] = "Worldsssssss";

	printf("Total length of string tried to copy: %zu",
	strlcpy(dest1, src1, 15));
	printf("\n%s, %s\n", src1, dest1);
		printf("Total length of string tried to copy: %zu",
	ft_strlcpy(dest2, src2, 15));
	printf("\n%s, %s", src2, dest2);
	return (0);
}
*/
