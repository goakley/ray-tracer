#ifndef SHAPES_H
#define SHAPES_H

#include "ray.h"


/** Represents a sphere */
typedef struct {
  vec3f center; /**< position of the center of the sphere (x,y,z) */
  float radius; /**< radius of the sphere */
  vec3f color; /**< surface colour of the sphere (r,g,b) */
  float reflection; /**< reflectivity value (0 == non-reflective) */
  float transparency; /**< surface transparency value (0 == opaque) */
  vec3f emission; /**< emission colour of the sphere ((0,0,0) == no emission)*/
} sphere_t;

/** Determines if a sphere is intersected by a ray.
 * Returns 0 if no intersection and non-0 if intersection.
 * If t0 AND t1 are non-null and an intersection is found, places the two
 * 't' values into them, where multiplying the ray direction by a 't' value
 * and adding it to the ray origin yields an intersection point
 * (ray = O + tD). */
int sphere_intersects(sphere_t sphere, /**< The sphere to test for intersection
                                        * by the ray */
                      ray_t ray, /**< The ray that may or may not intersect the
                                  * sphere */
                      float *t0, /**< The direction magnitude of an
                                  * intersection point, if one exists */
                      float *t1 /**< The direction magnitude of an intersection
                                 * point, if more than one exists */
                      );


/** Represents a cuboid structure */
typedef struct {
  vec3f min; /**< The cuboid coordinate with the smallest (x,y,z) positions */
  vec3f max; /**< The cuboid coordinate with the largest (x,y,z) positions */
} cuboid_t;


/** TOTALLY UNTESTED AND UNUSED */
int cuboid_intersects(cuboid_t cuboid, /**< The cuboid to test for intersection
                                        * by the ray */
                      ray_t ray, /** The ray that may or may not intersect the
                                  * cuboid */
                      float *t0, /**< The direction magnitude of an
                                  * intersection point, if one exists */
                      float *t1 /**< The direction magnitude of an intersection
                                 * point, if more than one exists */
                      );


/** Represents an infinite plane */
typedef struct {
  vec3f point; /**< The origin point (closest to (0,0,0)) on the plane */
  vec3f normal; /**< The normal vector to the plane (determines orientation) */
} plane_t;

/** TOTALLY UNTESTED AND UNUSED */
int plane_intersects(plane_t plane, /**< The plane to test for intersection by
                                     * the ray */
                     ray_t ray, /** The ray that may or may not intersect the
                                 * plane */
                     float *t /**< The direction magnitude of the intersection
                               * point, if it exists */
                     );


typedef struct {
  vec3f point; /**< The position of the center of the disc */
  vec3f normal; /**< The normal vector to the plane on which the disc lies */
  float radius; /**< The radius of the disc */
} disc_t;

/** TOTALLY UNTESTED AND UNUSED */
int disc_intersects(disc_t disc, /**< The disc to test for intersection by the
                                  * ray */
                    ray_t ray, /** The ray that may or may not intersect the
                                * disc */
                    float *t /**< The direction magnitude of the intersection
                              * point, if it exists */
                    );


#endif
