/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 16:22:19 by vagarcia          #+#    #+#             */
/*   Updated: 2024/09/11 14:41:25 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*cs1;
	unsigned char	*cs2;

	cs1 = (unsigned char *)s1;
	cs2 = (unsigned char *)s2;
	if (n == 0)
		return (0);
	i = 0;
	while (cs1[i] == cs2[i] && cs2[i] != '\0' && i + 1 < n)
		i++;
	if (cs1[i] == cs2[i])
		return (0);
	else
		return (cs1[i] - cs2[i]);
}
/*
#include <string.h>
#include <stdio.h>
int main() {
  char str1[] = "abd0asfz", str2[] = "abd9gass";
  int result;
  int result1;

  // comparing strings str1 and str2
  result = strncmp(str1, str2, 9);
  printf("strcmp(str1, str2) = %d\n", result);

  // comparing strings str1 and str3
  result1 = ft_strncmp(str1, str2, 9);
  printf("ft_strcmp(str1, str2) = %d\n", result1);

  return 0;
}*/
