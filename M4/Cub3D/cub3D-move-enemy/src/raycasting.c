/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:01 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/23 17:47:52 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/**
 * Initializes ray properties for raycasting
 * 
 * This function sets up the ray parameters for raycasting. In raycasting:
 * - The map is a 2D square grid where each cell is either empty or a wall
 * - For each vertical screen column, a ray is sent from player position
 * - The ray direction depends on player's view direction and screen position
 * - We use the ray to find walls and calculate their distance and height
 * 
 * Ray initialization includes calculating:
 * - camera_x: x-coordinate in camera space (-1 to 1 across screen width)
 * - ray_dir: direction of the ray combining player direction and camera plane
 * - map position: which map grid cell the player is in
 * - delta_dist: distance between consecutive grid lines for x and y
 * 
 * @param ray Pointer to the ray structure to initialize
 * @param x Current x-coordinate of screen being processed
 * @param game Pointer to the game structure
 * @param i Player index
 */
void	init_ray(t_ray *ray, int x, t_game *game, int i)
{
	ray->camera_x = 2 * x / (double)WIN_WIDTH - 1;
	ray->ray_dir_x = game->player[i].dir_x + game->player[i].plane_x
		* ray->camera_x;
	ray->ray_dir_y = game->player[i].dir_y + game->player[i].plane_y
		* ray->camera_x;
	ray->map_x = (int)game->player[i].pos_x;
	ray->map_y = (int)game->player[i].pos_y;
	if (ray->ray_dir_x == 0)
		ray->delta_dist_x = 1e30;
	else
		ray->delta_dist_x = fabs(1 / ray->ray_dir_x);
	if (ray->ray_dir_y == 0)
		ray->delta_dist_y = 1e30;
	else
		ray->delta_dist_y = fabs(1 / ray->ray_dir_y);
	ray->hit = 0;
	ray->bonus = 0;
	ray->y = game->camera_y[i];
}

/**
 * Performs one step of the Digital Differential Analysis (DDA) algorithm
 * 
 * This is part of the core raycasting algorithm that efficiently finds which 
 * map squares a ray passes through. The DDA algorithm works by:
 * 1. Comparing the distance to the next x-side and y-side
 * 2. Jumping to the closer side (either in x or y direction)
 * 3. Checking if the new square contains a wall
 * 
 * This implementation handles one step of this process, moving the ray
 * to the next grid square and updating side_dist values. Using DDA instead
 * of checking every pixel along the ray's path is much more efficient and
 * precise, ensuring no walls are missed.
 * 
 * @param ray Pointer to the ray structure
 * @param map_height Height of the map grid
 * @param map_width Width of the map grid
 * @return 1 if the ray moved out of bounds, 0 otherwise
 */
int	perform_dda1(t_ray *ray, int map_height, int map_width)
{
	if (ray->side_dist_x < ray->side_dist_y)
	{
		ray->side_dist_x += ray->delta_dist_x;
		ray->map_x += ray->step_x;
		ray->side = 0;
	}
	else
	{
		ray->side_dist_y += ray->delta_dist_y;
		ray->map_y += ray->step_y;
		ray->side = 1;
	}
	if (ray->map_x < 0 || ray->map_x >= map_width || ray->map_y < 0
		|| ray->map_y >= map_height)
	{
		ray->hit = 1;
		return (1);
	}
	return (0);
}

/**
 * Executes the Digital Differential Analysis (DDA) algorithm to find wall hits
 * 
 * The DDA algorithm is the core of raycasting, 
 * 	finding the first wall a ray encounters:
 * 1. Starting from the player's position, the ray advances through the grid
 * 2. At each step, it moves to either the next x-side or y-side 
 * 3. After each move, it checks if the current grid cell contains a wall
 * 4. This continues until either a wall is hit or maximum distance is reached
 * 
 * This is much more efficient than checking every pixel along the ray's path:
 * - It makes a minimum number of checks by only testing at grid boundaries
 * - It guarantees no walls are missed by testing at every grid line intersection
 * - It has safeguards against infinite loops 
 * 	 with maximum distance and bounds checking
 * 
 * The algorithm also handles special cases:
 * - Different wall types (regular walls and special objects in bonus mode)
 * - Map bounds checking to prevent array access errors
 * 
 * @param ray Pointer to the ray structure to update
 * @param map 2D grid representing the game map
 * @param map_height Height of the map grid
 * @param map_width Width of the map grid
 */
void	perform_dda(t_ray *ray, char **map, int map_height, int map_width)
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
			if (map[ray->map_y][ray->map_x] == '1')
				ray->hit = 1;
			if (BONUS && map[ray->map_y][ray->map_x] == 'C')
				ray->hit = 2;
			if (BONUS)
				check_hit(ray, map);
		}
		else
			ray->hit = 1;
		steps++;
	}
	if (steps >= max_distance)
		ray->hit = 1;
}

/**
 * Determines the appropriate texture based on which wall face was hit
 * 
 * This function selects the correct wall texture by analyzing:
 * 1. Which side of the wall was hit (x-side or y-side)
 * 2. The direction of the ray that hit the wall
 * 
 * The texture selection process:
 * - For x-side hits (side == 0):
 *   - If ray direction x is positive, it hit a west-facing wall (EAST texture)
 *   - If ray direction x is negative, it hit an east-facing wall (WEST texture)
 * - For y-side hits (side == 1):
 *   - If ray direction y is positive, it hit a north-facing wall (SOUTH texture)
 *   - If ray direction y is negative, it hit a south-facing wall (NORTH texture)
 * 
 * @param ray Pointer to the ray structure with hit information
 * @return Integer representing the texture index to use
 */
int	determine_texture(t_ray *ray)
{
	if (ray->side == 0)
	{
		if (ray->ray_dir_x > 0)
			return (EAST);
		else
			return (WEST);
	}
	else
	{
		if (ray->ray_dir_y > 0)
			return (SOUTH);
		else
			return (NORTH);
	}
}

/**
 * Performs raycasting for the entire screen width to create the 3D view
 * 
 * This is the main raycasting function, executing the entire process:
 * 1. For each vertical screen column, a ray is cast from the player position
 * 2. The ray's properties (direction, map position) are initialized
 * 3. The DDA algorithm is used to find wall intersections
 * 4. Wall distance and height are calculated with perspective correction
 * 5. Texture coordinates are computed for realistic wall rendering
 * 6. The vertical line is drawn with the appropriate texture
 * 
 * The raycasting creates the 3D illusion from a 2D map by:
 * - Transforming the player's 2D position and direction into a 3D perspective
 * - Using the distance to walls to determine their height on screen
 * - Applying textures with correct scaling based on distance
 * - Storing depth information in a z-buffer (for bonus features like sprites)
 * 
 * This implementation follows the principles of the DDA algorithm,
 * ensuring efficient rendering without the fisheye distortion.
 * 
 * @param game Pointer to the game structure containing map and player data
 * @param i Player index to determine which player's view to render
 */
void	cast_rays(t_game *game, int i)
{
	int		x;
	t_ray	ray;

	x = 0;
	while (x < WIN_WIDTH)
	{
		init_ray(&ray, x, game, i);
		calculate_step_and_side_dist(&ray, &game->player[i]);
		perform_dda(&ray, game->map.grid, game->map.height, game->map.width);
		calculate_wall_distance(&ray, &game->player[i]);
		calculate_wall_height(&ray);
		calculate_texture_x(&ray, &game->player[i]);
		draw_vertical_line(game, &ray, x);
		if (BONUS)
			game->zbuffer[x] = ray.perp_wall_dist;
		x++;
	}
}
