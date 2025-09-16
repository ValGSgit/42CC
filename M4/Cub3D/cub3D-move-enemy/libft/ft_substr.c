/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 17:45:42 by kfan              #+#    #+#             */
/*   Updated: 2024/09/13 21:44:34 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*str;
	size_t	size;

	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	if (ft_strlen(s) <= len)
		size = ft_strlen(s) - start + 1;
	else if (ft_strlen(&s[start]) < len)
		size = len;
	else
		size = len + 1;
	str = malloc(size);
	if (!str)
		return (NULL);
	ft_bzero(str, size);
	ft_strlcpy(str, &s[start], size);
	return (str);
}

/*
#include <stdio.h>
#include <string.h>
int	main(int argc, char **argv)
{
	(void)argc;
	printf("%s", ft_substr(argv[1], atoi(argv[2]), atoi(argv[3])));
	return (0);
}
*/
