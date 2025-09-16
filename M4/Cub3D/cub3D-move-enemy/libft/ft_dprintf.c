/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 10:21:31 by kfan              #+#    #+#             */
/*   Updated: 2025/05/03 16:48:11 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_discon(char c)
{
	if (c == 'c' || c == 's' || c == 'p' || c == 'd' || c == 'i' || c == 'u'
		|| c == 'x' || c == 'X')
		return (1);
	if (c == '%')
		return (2);
	else
		return (0);
}

static int	ft_dflags(int fd, const char *input, long arg)
{
	int	i;

	i = 0;
	while (ft_discon(input[i]) == 0)
		i++;
	if (input[i] == 'c')
		i += ft_dputchar(fd, (char)arg);
	else if (input[i] == 's')
		i += ft_dputstr(fd, (const char *)arg);
	else if (input[i] == 'd' || input[i] == 'i')
		i += ft_dputnbr(fd, (int)arg);
	else if (input[i] == 'u')
		i += ft_dputnbruns(fd, (unsigned int)arg);
	else if (input[i] == 'x')
		i += ft_dhex(fd, (unsigned int)arg, 0);
	else if (input[i] == 'X')
		i += ft_dhex(fd, (unsigned int)arg, 32);
	else if (input[i] == 'p')
		i += ft_daddress(fd, (unsigned long)arg);
	return (i);
}

static int	ft_dispercent(int fd, const char *input)
{
	int	i;

	i = 0;
	while (ft_discon(input[i]) == 0 && input[i] != '%')
		i++;
	if (ft_discon(input[i]) == 1)
		return (0);
	if (input[i] == '%')
		i += ft_dputchar('%', fd);
	return (i);
}

static int	ft_dloop(int fd, const char *input, va_list args)
{
	int	i;
	int	j;
	int	out;

	i = 0;
	out = 0;
	while (input[i] != '\0')
	{
		if (input[i] == '%')
		{
			j = ft_dispercent(fd, &input[++i]);
			if (j != 0)
				out += j;
			else
				out += ft_dflags(fd, &input[i], va_arg(args, long));
			i += j;
			if (j == 0)
				i++;
		}
		else
			out += write(fd, &input[i++], 1);
	}
	return (out);
}

int	ft_dprintf(int fd, const char *input, ...)
{
	va_list	args;
	int		out;

	if (!input)
		return (-1);
	va_start(args, input);
	out = ft_dloop(fd, input, args);
	va_end(args);
	return (out);
}

/*
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	int	i;
	int	j;

	(void)argc;
	(void)argv;
	i = printf(NULL);
	printf("return (value printf: %d\n", i));
	j = ft_printf(NULL);
	ft_printf("return (value ft_printf: %d\n", j));
}
*/