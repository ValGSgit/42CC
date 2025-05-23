/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_calculate.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:38:01 by vagarcia          #+#    #+#             */
/*   Updated: 2025/05/23 17:54:38 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/**
 * Calculates step direction and initial side distance for raycasting
 * 
 * This function prepares the ray for DDA algorithm execution by determining:
 * 1. The step direction in both x and y axes based on ray direction
 * 2. The initial side_dist values, which represent how far the ray must travel
 *    from its starting position to reach the first x-side or y-side
 * 
 * The mathematics ensure the ray will correctly traverse the grid:
 * - For negative ray directions, 
 * 	 we step backwards (-1) and calculate distance to previous grid line
 * - For positive ray directions, we step forwards (+1) 
 * 	 and calculate distance to next grid line
 * - The side_dist is scaled by delta_dist to get true Euclidean distance
 * 
 * @param ray Pointer to the ray structure
 * @param player Pointer to the player structure
 */
void	calculate_step_and_side_dist(t_ray *ray, t_player *player)
{
	if (ray->ray_dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dist_x = (player->pos_x - ray->map_x) * ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_dist_x = (ray->map_x + 1.0 - player->pos_x)
			* ray->delta_dist_x;
	}
	if (ray->ray_dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dist_y = (player->pos_y - ray->map_y) * ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_dist_y = (ray->map_y + 1.0 - player->pos_y)
			* ray->delta_dist_y;
	}
}

/**
 * Calculates the perpendicular wall distance to avoid fisheye effect
 * 
 * This function gets the distance from the camera plane to the wall hit point.
 * We use the perpendicular distance rather than the direct Euclidean distance
 * to avoid the fisheye effect that would distort the walls.
 * 
 * The perpendicular distance calculation:
 * - For x-side hits: Calculate distance based on x-coordinate difference
 * - For y-side hits: Calculate distance based on y-coordinate difference
 * - The formula includes a correction term 
 * 	(1-ray->step_x)/2 which is either 0 or 1 depending on the ray direction
 * 
 * This distance is later used to determine wall height,
 * the further away a wall is,
 * the shorter it appears on screen, creating the 3D perspective effect.
 * 
 * @param ray Pointer to the ray structure
 * @param player Pointer to the player structure
 */
void	calculate_wall_distance(t_ray *ray, t_player *player)
{
	if (ray->side == 0)
		ray->perp_wall_dist = (ray->map_x - player->pos_x + (1 - ray->step_x)
				/ 2) / ray->ray_dir_x;
	else
		ray->perp_wall_dist = (ray->map_y - player->pos_y + (1 - ray->step_y)
				/ 2) / ray->ray_dir_y;
}

/**
 * Calculates the height of the wall and its drawing boundaries on screen
 *
 * This function turns the perpendicular wall distance into visible wall height:
 * 1. The line_height is inversely proportional to perpendicular wall distance
 *    - Closer walls appear taller, further walls appear shorter
 * 2. The drawing start and end positions are to center the wall vertically
 *    - The y parameter allows for camera pitch effects (looking up/down)
 * 3. Drawing boundaries are clamped to screen dimensions
 *    - Prevents drawing outside the screen buffer
 * 
 * The formula used (WIN_HEIGHT / perpendicular distance)
 * creates the perspective effect that is essential.
 * 
 * @param ray Pointer to the ray structure containing wall distance information
 */
void	calculate_wall_height(t_ray *ray)
{
	ray->line_height = (int)(WIN_HEIGHT / ray->perp_wall_dist);
	ray->draw_start = -ray->line_height / 2 + WIN_HEIGHT / 2 + ray->y;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	ray->draw_end = ray->line_height / 2 + WIN_HEIGHT / 2 + ray->y;
	if (ray->draw_end >= WIN_HEIGHT)
		ray->draw_end = WIN_HEIGHT - 1;
}

/**
 * Calculates the x-coordinate on the texture for proper texture mapping
 * 
 * This function determines exactly where on the wall the ray hit, and translates
 * this to the appropriate horizontal texture coordinate:
 * 1. Calculate the exact hit position (wall_x) using the perpendicular distance
 *    - For x-side hits, use player's y-coordinate plus the projection
 *    - For y-side hits, use player's x-coordinate plus the projection
 * 2. Take the fractional part of wall_x (wall position within the grid cell)
 * 3. Convert to texture coordinate by multiplying by texture width
 * 4. Apply mirror corrections for certain sides to prevent texture mirroring
 *    - Flip x-coordinate when ray hits an x-side while moving left
 *    - Flip x-coordinate when ray hits a y-side while moving down
 * 
 * This ensures textures are displayed correctly on walls 
 * regardless of viewing angle.
 * 
 * @param ray Pointer to the ray structure
 * @param player Pointer to the player structure
 */
void	calculate_texture_x(t_ray *ray, t_player *player)
{
	if (ray->side == 0)
		ray->wall_x = player->pos_y + ray->perp_wall_dist * ray->ray_dir_y;
	else
		ray->wall_x = player->pos_x + ray->perp_wall_dist * ray->ray_dir_x;
	ray->wall_x -= floor(ray->wall_x);
	ray->tex_x = (int)(ray->wall_x * (double)TEX_WIDTH);
	if (ray->side == 0 && ray->ray_dir_x < 0)
		ray->tex_x = TEX_WIDTH - ray->tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y > 0)
		ray->tex_x = TEX_WIDTH - ray->tex_x - 1;
}
