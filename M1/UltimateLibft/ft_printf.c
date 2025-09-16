/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 12:59:41 by vagarcia          #+#    #+#             */
/*   Updated: 2024/10/23 11:14:11 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_putchar(int c)
{
	write(1, &c, 1);
	return (1);
}

void	ft_putstr(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		write(1, &s[i], 1);
		i++;
	}
}

int	ft_printstr(char *s)
{
	int	i;

	i = 0;
	if (s == NULL)
	{
		ft_putstr("(null)");
		return (6);
	}
	while (s[i])
	{
		write (1, &s[i], 1);
		i++;
	}
	return (i);
}

int	ft_formats(va_list args, const char s)
{
	int	printlen;

	printlen = 0;
	if (s == 'c')
		printlen += ft_putchar(va_arg(args, int));
	else if (s == 's')
		printlen += ft_printstr(va_arg(args, char *));
	else if (s == 'p')
		printlen += ft_printptr(va_arg(args, unsigned long long));
	else if (s == 'd' || s == 'i')
		printlen += intputnbr(va_arg(args, int));
	else if (s == 'u')
		printlen += uintputnbr(va_arg(args, unsigned int));
	else if (s == 'x' || s == 'X')
		printlen += ft_printhex(va_arg(args, unsigned int), s);
	else if (s == '%')
		printlen += ft_printpercent();
	else
		printlen += ft_printpercent() + ft_putchar(s);
	return (printlen);
}

int	ft_printf(const char *str, ...)
{
	size_t	i;
	va_list	args;
	int		print_len;

	i = 0;
	print_len = 0;
	va_start(args, str);
	if (str == NULL)
		return (-1);
	while (str[i] != '\0')
	{
		if (str[i + 1] == '\0' && str[i] == '%' && str[i - 1] == ' ')
			return (va_end(args), -1);
		if (str[i] == '%' && str[i + 1] != '\0')
		{
			print_len += ft_formats(args, str[i + 1]);
			i++;
		}
		else
			print_len += ft_putchar(str[i]);
		i++;
	}
	va_end(args);
	return (print_len);
}
