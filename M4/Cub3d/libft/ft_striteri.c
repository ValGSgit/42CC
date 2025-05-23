/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 10:55:27 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 21:21:32 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	unsigned int	i;

	if (!s || !f)
		return ;
	i = 0;
	while (s[i])
	{
		f(i, &s[i]);
		i++;
	}
}

/*
void	ft_test(unsigned int i, char *c)
{
	(void)i;
	if (*c >= 97 && *c <= 122)
		*c = *c - 32;
}

#include <stdio.h>

int	main(int argc, char **argv)
{
	(void)argc;
	ft_striteri(argv[1], ft_test);
	printf("%s", argv[1]);
	return (0);
}
*/
