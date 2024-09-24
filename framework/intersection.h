#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "types.h"

extern unsigned long long num_rays_shot;
extern unsigned long long num_shadow_rays_shot;
extern unsigned long long num_triangles_tested;
extern unsigned long long num_bboxes_tested;

int     find_first_intersection(intersection_point *ip,
            vec3 ray_origin, vec3 ray_direction);

int     shadow_check(vec3 ray_origin, vec3 ray_direction);

#endif
