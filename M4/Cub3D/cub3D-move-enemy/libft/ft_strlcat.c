/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 13:13:05 by kfan              #+#    #+#             */
/*   Updated: 2024/09/11 14:27:13 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	dstlen;
	size_t	i;

	dstlen = ft_strlen(dst);
	if (dstlen >= size)
		return (size + ft_strlen(src));
	i = 0;
	while (i < (size - dstlen - 1) && src[i] != '\0')
	{
		dst[i + dstlen] = src[i];
		i++;
	}
	if (dstlen < size || dstlen != 0)
		dst[i + dstlen] = '\0';
	return (dstlen + ft_strlen(src));
}

/*
#include <stdio.h>
#include <bsd/string.h>

int	main(void)
{
	char	dest[9] = "Hello";
	char	src[] = "World";

	printf("dst: %s, src: %s", dest, src);
	printf("\nreturn value: %zu", ft_strlcat(dest, src, 3));
	printf("\nresult: %s, %s, %s", dest, src, &dest[0]);
}
*/
