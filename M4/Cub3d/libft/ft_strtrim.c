/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 10:56:31 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 20:15:37 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*ptr;
	size_t	i;
	size_t	j;

	if (!s1 || !set)
		return (NULL);
	i = 0;
	ptr = (char *)s1;
	while (ft_strchr(set, ptr[i]) && ptr[i])
		i++;
	ptr = &ptr[i];
	j = ft_strlen(ptr);
	while (ft_strchr(set, ptr[j - 1]) && j > 0)
		j--;
	return (ft_substr(ptr, 0, j));
}

/*
#include <stdio.h>

int	main(int argc, char **argv)
{
	(void)argc;
	printf("%s", ft_strtrim(argv[1], argv[2]));
	return (0);
}
*/