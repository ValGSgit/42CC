/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:01 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/23 17:49:37 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/**
 * Determines what type of object a ray has hit in the map
 * 
 * This function extends the basic wall detection to include special bonus
 * objects like coins, keys, and doors. Each type of map element returns
 * a different hit code to allow for specialized rendering and game logic.
 * 
 * @param ray Pointer to the ray structure containing hit coordinates
 * @param map The 2D map grid
 * @return Hit type: 0=no hit, 1=wall/coin, 2=key, 3=F-object, 4=door
 */
static int	ray_hits_what(t_ray *ray, char **map)
{
	if (map[ray->map_y][ray->map_x] == '1')
		return (1);
	else if (BONUS && map[ray->map_y][ray->map_x] == 'C')
		return (1);
	else if (BONUS && map[ray->map_y][ray->map_x] == 'K')
		return (2);
	else if (BONUS && map[ray->map_y][ray->map_x] == 'F')
		return (3);
	else if (BONUS && map[ray->map_y][ray->map_x] == 'D')
		return (4);
	return (0);
}

/**
 * Performs Digital Differential Analysis (DDA) algorithm for bonus mode
 * 
 * This enhanced version of the DDA algorithm includes safety measures like
 * maximum distance checking to prevent infinite loops, and special handling
 * for bonus objects. The DDA works by stepping through grid lines efficiently:
 * 
 * - Instead of checking every point along the ray, DDA jumps from grid line
 *   to grid line (either x-sides or y-sides of map squares)
 * - At each step, it determines whether to move in x or y direction based on
 *   which grid line is closer (sideDistX vs sideDistY)
 * - This ensures no walls are missed while being computationally efficient
 * - The algorithm stops when hitting a wall or reaching maximum distance
 * 
 * @param ray Pointer to the ray structure to process
 * @param map The 2D map grid to check for collisions
 * @param map_height Height of the map for bounds checking
 * @param map_width Width of the map for bounds checking
 */
void	perform_dda_bonus(t_ray *ray, char **map, int map_height, int map_width)
{
	int	max_distance;
	int	steps;

	max_distance = 100;
	steps = 0;
	while (ray->hit == 0 && steps < max_distance)
	{
		if (perform_dda1(ray, map_height, map_width))
			continue ;
		if (map[ray->map_y] != NULL
			&& ray->map_x < (int)ft_strlen(map[ray->map_y]))
		{
			ray->hit = ray_hits_what(ray, map);
		}
		else
			ray->hit = 1;
		steps++;
	}
	if (steps >= max_distance)
		ray->hit = 1;
}

/**
 * Renders textured walls with camera tilt compensation for bonus mode
 * 
 * This function performs textured wall rendering
 * but with enhanced features for bonus mode. The texture mapping process:
 * 
 * 1. Calculates texture step size based on wall height on screen
 * 2. Determines starting texture position accounting for camera tilt (ray->y)
 * 3. For each vertical pixel, maps to corresponding texture pixel (texel)
 * 4. Uses affine texture mapping which is perspective-correct for vertical walls
 * 
 * The camera tilt compensation ensures that when the player looks up/down,
 * walls are rendered correctly while sprites maintain consistent positioning.
 * 
 * @param game Pointer to the game structure
 * @param ray Pointer to the ray with calculated wall parameters
 * @param x Current screen x-coordinate being rendered
 * @param tex_num Texture number to use for rendering
 */
void	draw_texture_bonus(t_game *game, t_ray *ray, int x, int tex_num)
{
	int		y;
	int		tex_y;
	double	step;
	double	tex_pos;
	int		color;

	step = 1.0 * TEX_HEIGHT / ray->line_height;
	tex_pos = (ray->draw_start - WIN_HEIGHT / 2 \
		- ray->y + ray->line_height / 2) * step;
	y = ray->draw_start;
	while (y < ray->draw_end)
	{
		tex_y = (int)tex_pos & (TEX_HEIGHT - 1);
		tex_pos += step;
		color = get_texture_color(game, tex_num, ray->tex_x, tex_y);
		if (color)
			my_mlx_pixel_put(&game->img, x, y, color);
		y++;
	}
}

/**
 * Performs complete raycasting for one screen column in bonus mode
 * 
 * This function orchestrates the entire raycasting process for a single vertical
 * stripe of the screen in bonus mode. The raycasting pipeline consists of:
 * 
 * 1. Ray init: Set up ray direction based on player view and screen position
 * 2. DDA setup: Calculate step directions and initial distances to grid lines
 * 3. DDA execution: Step through map grid until hitting a wall or object
 * 4. Distance: Calculate perpendicular distance to avoid fisheye effect
 * 5. Wall height calculation: Determine how tall the wall appears on screen
 * 6. Texture mapping: Calculate which part of the texture to display
 * 7. Rendering: Draw the textured wall stripe for special objects
 * 
 * The bonus mode adds support for interactive objects like doors and special
 * textured elements that enhance gameplay beyond basic wall rendering.
 * 
 * @param game Pointer to the game structure
 * @param i Player index for multiplayer support
 * @param x Current screen x-coordinate being processed
 */
void	cast_rays_bonus(t_game *game, int i, int x)
{
	t_ray	ray;

	init_ray(&ray, x, game, i);
	calculate_step_and_side_dist(&ray, &game->player[i]);
	perform_dda_bonus(&ray, game->map.grid, game->map.height,
		game->map.width);
	calculate_obj_distance(&ray, &game->player[i]);
	calculate_obj_height(&ray);
	calculate_obj_x(&ray, &game->player[i]);
	if (ray.hit == 3)
		draw_texture_bonus(game, &ray, x, 6);
	if (ray.hit == 4)
		draw_texture_bonus(game, &ray, x, 7);
}
