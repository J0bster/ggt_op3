/* Computer Graphics and Game Technology, Assignment Ray-tracing
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "shaders.h"
#include "perlin.h"
#include "v3math.h"
#include "intersection.h"
#include "scene.h"
#include "quat.h"
#include "constants.h"

// shade_constant()
//
// Always return the same color. This shader does no real computations
// based on normal, light position, etc. As such, it merely creates
// a "silhouette" of an object.

vec3 shade_constant(intersection_point ip)
{
  return v3_create(1, 0, 0);
}

vec3 shade_matte(intersection_point ip)
{
  vec3 color = v3_create(0, 0, 0);
  float offset = 0.0001;

  color.x += scene_ambient_light;
  color.y += scene_ambient_light;
  color.z += scene_ambient_light;


  for (int i = 0; i < scene_num_lights; i++)
  {
    vec3 light_position = scene_lights[i].position;
    float light_intensity = scene_lights[i].intensity;

    vec3 light_dir = v3_normalize(v3_subtract(ip.p, light_position));

    float dot = v3_dotprod(ip.n, v3_negate(light_dir));

    if (dot > 0.0)
    {
      vec3 offset_p = v3_add(ip.p, v3_multiply(ip.n, offset));
      if (!shadow_check(offset_p, light_position))
      {
        color.x += dot * light_intensity;
        color.y += dot * light_intensity;
        color.z += dot * light_intensity;
      }
    }
  }

  return color;
}

vec3 shade_blinn_phong(intersection_point ip)
{
  float kd = 0.8;
  float ks = 0.5;
  float alpha = 50.0;
  vec3 cd = v3_create(1, 0, 0);
  vec3 cs = v3_create(1, 1, 1);
  vec3 color = v3_create(0, 0, 0);
  float offset = 0.0001;
  
  color.x += scene_ambient_light * cd.x;
  color.y += scene_ambient_light * cd.y;
  color.z += scene_ambient_light * cd.z;


  for (int i = 0; i < scene_num_lights; i++)
  {
    vec3 light_position = scene_lights[i].position;
    float light_intensity = scene_lights[i].intensity;

    vec3 light_dir = v3_normalize(v3_subtract(ip.p, light_position));

    float dot = v3_dotprod(ip.n, v3_negate(light_dir));

    if (dot > 0.0)
    {
      vec3 offset_p = v3_add(ip.p, v3_multiply(ip.n, offset));
      if (!shadow_check(offset_p, light_position))
      {
        color.x += dot * light_intensity * kd * cd.x;
        color.y += dot * light_intensity * kd * cd.y;
        color.z += dot * light_intensity * kd * cd.z;
      }

      vec3 h = v3_normalize(v3_add(light_dir, ip.i));
      float dot_hn = v3_dotprod(h, ip.n);

      if (dot_hn > 0.0)
      {
        color.x += light_intensity * ks * pow(dot_hn, alpha) * cs.x;
        color.y += light_intensity * ks * pow(dot_hn, alpha) * cs.y;
        color.z += light_intensity * ks * pow(dot_hn, alpha) * cs.z;
      }
    }
  }

  return color;
}

vec3 shade_reflection(intersection_point ip)
{
  vec3 color = v3_create(0, 0, 0);

  vec3 r = v3_subtract(v3_multiply(ip.n, 2 * v3_dotprod(ip.n, ip.i)), ip.i);

  vec3 r_offset = v3_add(ip.p, v3_multiply(ip.n, 0.0001));

  vec3 new_color = ray_color(ip.ray_level + 1, r_offset, r);

  vec3 matte_color = shade_matte(ip);

  matte_color = v3_multiply(matte_color, 0.75);
  new_color = v3_multiply(new_color, 0.25);

  color = v3_add(matte_color, new_color);

  return color;
}

// Returns the shaded color for the given point to shade.
// Calls the relevant shading function based on the material index.
vec3 shade(intersection_point ip)
{
  switch (ip.material)
  {
  case 0:
    return shade_constant(ip);
  case 1:
    return shade_matte(ip);
  case 2:
    return shade_blinn_phong(ip);
  case 3:
    return shade_reflection(ip);
  default:
    return shade_constant(ip);
  }
}

// Determine the surface color for the first object intersected by
// the given ray, or return the scene background color when no
// intersection is found
vec3 ray_color(int level, vec3 ray_origin, vec3 ray_direction)
{
  intersection_point ip;

  // If this ray has been reflected too many times, simply
  // return the background color.
  if (level >= 3)
    return scene_background_color;

  // Check if the ray intersects anything in the scene
  if (find_first_intersection(&ip, ray_origin, ray_direction))
  {
    // Shade the found intersection point
    ip.ray_level = level;
    return shade(ip);
  }

  // Nothing was hit, return background color
  return scene_background_color;
}
