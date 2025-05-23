/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 16:28:28 by kfan              #+#    #+#             */
/*   Updated: 2025/04/04 23:34:33 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;
	char	*ptr;

	ptr = (char *)big;
	if (!*little)
		return (ptr);
	i = 0;
	while (i < len && ptr[i] != '\0')
	{
		j = 0;
		if (ptr[i] == little[j])
		{
			while (ptr[i + j] == little[j] && little[j] != '\0')
			{
				j++;
				if (j == ft_strlen(little) && i + j <= len)
					return (&ptr[i]);
			}
		}
		i++;
	}
	return (NULL);
}

/*
#include <stdio.h>
#include <stdlib.h>
int	main(int argc, char **argv)
{
	(void)argc;
	printf("%s", ft_strnstr(argv[1], argv[2], atoi(argv[3])));
	return (0);
}
*/
