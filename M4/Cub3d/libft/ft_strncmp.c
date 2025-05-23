/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ka-hou <ka-hou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 13:28:28 by kfan              #+#    #+#             */
/*   Updated: 2025/03/13 14:26:26 by ka-hou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

//modified for pipex to take empty str safely
//and if n==0 return 1
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	str1;
	unsigned char	str2;

	i = 0;
	if (!s1 || !s2)
		return (1);
	if (n == 0)
		return (1);
	while (i < n && (s1[i] != '\0' || s2[i] != '\0'))
	{
		str1 = s1[i];
		str2 = s2[i];
		if (str1 != str2)
			return (str1 - str2);
		i++;
	}
	return (0);
}

/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	(void)argc;
	printf("%d", strncmp(argv[1], argv[2], atoi(argv[3])));
	printf("%d", ft_strncmp(argv[1], argv[2], atoi(argv[3])));
}
*/
