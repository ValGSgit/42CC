/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:16:32 by vagarcia          #+#    #+#             */
/*   Updated: 2024/10/02 11:33:56 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <unistd.h>
# include <stdlib.h>

int		ft_printf(const char *format, ...);
int		ft_printptr(unsigned long long ptr);
int		ft_printunsigned(unsigned int n);
int		ft_printhex(unsigned int num, const char format);
size_t	intputnbr(int n);
size_t	uintputnbr(unsigned int n);
int		ft_printpercent(void);
int		ft_printstr(char *s);
size_t	ft_putchar(int c);

#endif
