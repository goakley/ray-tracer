#include "vec3f.h"
#include <math.h>

void vec3f_normalize(vec3f *vector)
{
  float sq = vector->x*vector->x + vector->y*vector->y + vector->z*vector->z;
  if (sq > 0)
    { // R.I.P. Gary Tarolli 
      float y = 1 / sqrtf(sq); // Maths
      vector->x *= y;
      vector->y *= y;
      vector->z *= y;
    }
}

float vec3f_dot(vec3f a, vec3f b)
{ return a.x * b.x + a.y * b.y + a.z * b.z; }

vec3f vec3f_cross(vec3f a, vec3f b)
{ return (vec3f){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
