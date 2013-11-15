#ifndef RAY_H
#define RAY_H

#include "vec3f.h"

/** Represents a ray, of the form: ray = origin + direction */
typedef struct {
  vec3f origin; /**< The origin position of the ray */
  vec3f direction; /**< The directional vector associated with the ray */
} ray_t;

#endif
