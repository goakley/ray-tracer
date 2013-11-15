#ifndef VEC3F_H
#define VEC3F_H

/** Represents a vector with (x,y,z) float components */
typedef struct {
  float x, y, z;
} vec3f;

/** Normalizes a vector (magnitude = 1) in-place */
void vec3f_normalize(vec3f*);

/** Calculates the dot product of two vectors */
float vec3f_dot(vec3f, vec3f);

/** Calculates the cross product of two vectors */
vec3f vec3f_cross(vec3f, vec3f);

#endif
