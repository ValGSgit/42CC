/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:06 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/23 17:46:24 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/**
 * Calculates the appropriate pixel size for rendering the map
 * 
 * This function determines the optimal pixel size for the minimap display
 * by ensuring the entire map fits within the allocated screen space while
 * maximizing the size for visibility. It calculates the largest possible
 * pixel size where both map width and height can fit within the scaled
 * window dimensions.
 * 
 * @param game Pointer to the game structure containing map dimensions
 * @return The calculated pixel size to use for map rendering
 */
static int	check_map_size(t_game *game)
{
	int	pixel;
	int	max_height;
	int	max_width;

	pixel = 1;
	max_height = WIN_HEIGHT / MAP_SCALE;
	max_width = WIN_WIDTH / MAP_SCALE;
	while (pixel * game->map.height < max_height && pixel
		* game->map.width < max_width)
		pixel++;
	return (pixel);
}

/**
 * Renders special items on the minimap with distinctive colors
 * 
 * This function draws special elements like keys(K), coins(C), and doors(O)
 * with unique colors to visually distinguish them on the minimap. For each item
 * found at the given coordinates, it sets the corresponding pixel to a specific
 * RGB color value.
 * 
 * @param game Pointer to the game structure
 * @param x The x-coordinate of the pixel to check
 * @param y The y-coordinate of the pixel to check
 * @param pixel The size of each map square in pixels
 */
static void	draw_map_items(t_game *game, int x, int y, int pixel)
{
	char	*dst;

	dst = game->img.addr + (y * game->img.line_length + x
			* (game->img.bits_per_pixel / 8));
	if (game->map.grid[(y / pixel)][(x / pixel)] == 'K')
		*(unsigned int *)dst = create_rgb(255, 215, 0);
	if (game->map.grid[(y / pixel)][(x / pixel)] == 'C')
		*(unsigned int *)dst = create_rgb(139, 69, 19);
	if (game->map.grid[(y / pixel)][(x / pixel)] == 'O')
		*(unsigned int *)dst = create_rgb(196, 164, 132);
}

/**
 * Renders a player marker on the minimap
 * 
 * This function draws a small 3x3 pixel representation of a player at the 
 * specified position on the minimap. The color differs based on player index,
 * with the first player drawn in red and other players in blue for clear
 * distinction in multiplayer mode.
 * 
 * @param game Pointer to the game structure
 * @param x0 The center x-coordinate for the player marker
 * @param y0 The center y-coordinate for the player marker
 * @param i The player index to determine which color to use
 */
static void	draw_mini_player(t_game *game, int x0, int y0, int i)
{
	int		x;
	int		y;
	char	*dst;

	y = -1;
	while (y <= 1)
	{
		x = -1;
		while (x <= 1)
		{
			dst = game->img.addr + ((int)(y + y0) *game->img.line_length
					+ (int)(x + x0) *(game->img.bits_per_pixel / 8));
			if (i == 0)
				*(unsigned int *)dst = create_rgb(255, 0, 0);
			else
				*(unsigned int *)dst = create_rgb(0, 0, 255);
			x++;
		}
		y++;
	}
}

/**
 * Draws a directional line showing player's viewing direction on the minimap
 * 
 * This function visualizes the player's viewing direction as a colored line
 * extending from the player's position in the direction they are facing.
 * The line is drawn pixel by pixel using the player's direction vector,
 * with the color corresponding to the player index (red for first player,
 * blue for others).
 * 
 * The raycasting visualization helps players understand their orientation 
 * within the map, showing which way they are currently facing. This is similar
 * to how raycasting works in the main rendering, where rays are cast outward
 * from the player position in the direction they are looking.
 * 
 * @param game Pointer to the game structure
 * @param i The player index to determine which color to use
 * @param dst Pointer to the pixel data location in the image buffer
 */
void	draw_range(t_game *game, int i, char *dst)
{
	int		pixel;
	int		x0;
	int		y0;
	double	x;
	double	y;

	pixel = check_map_size(game);
	x0 = pixel * game->player[i].pos_x;
	y0 = pixel * game->player[i].pos_y;
	draw_mini_player(game, x0, y0, i);
	x = game->player[i].dir_x;
	y = game->player[i].dir_y;
	while (pixel > 0)
	{
		dst = game->img.addr + ((int)(y + y0) *game->img.line_length + (int)(x
					+ x0) *(game->img.bits_per_pixel / 8));
		if (i == 0)
			*(unsigned int *)dst = create_rgb(255, 0, 0);
		else
			*(unsigned int *)dst = create_rgb(0, 0, 255);
		x = x + game->player[i].dir_x;
		y = y + game->player[i].dir_y;
		pixel--;
	}
}

/**
 * Renders the complete minimap with walls, items, and player positions
 * 
 * This function creates a 2D top-down view of the game world as a minimap.
 * It iterates through each pixel of the minimap area, calculating which map
 * grid cell each pixel corresponds to, and renders walls as black pixels and 
 * empty spaces as white. It also calls helper functions to draw special items
 * (like keys, coins, doors) with appropriate colors.
 * 
 * The minimap provides players with spatial awareness of their surroundings,
 * allowing them to navigate the maze-like structure more effectively. This is
 * complementary to the main raycasting display, giving a different perspective
 * of the same world.
 * 
 * @param game Pointer to the game structure
 * @param x Initial x-coordinate to start drawing (usually 0)
 * @param y Initial y-coordinate to start drawing (usually 0)
 * @param pixel The size of each map square in pixels
 */
void	draw_map(t_game *game, int x, int y, int pixel)
{
	char	*dst;

	pixel = check_map_size(game);
	while (y < WIN_HEIGHT)
	{
		x = 0;
		while (x < WIN_WIDTH)
		{
			dst = game->img.addr + (y * game->img.line_length + x
					* (game->img.bits_per_pixel / 8));
			if (!game->map.grid[(y / pixel)])
				return ;
			if (!game->map.grid[(y / pixel)][(x / pixel)])
				break ;
			if (game->map.grid[(y / pixel)][(x / pixel)] == '1')
				*(unsigned int *)dst = 0;
			else
				*(unsigned int *)dst = create_rgb(255, 255, 255);
			draw_map_items(game, x, y, pixel);
			x++;
		}
		y++;
	}
}
