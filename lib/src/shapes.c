#include "shapes.h"

#include <math.h>

int sphere_intersects(sphere_t sphere, ray_t ray, float *t0, float *t1)
{
  // geometric method
  vec3f root_dist = {sphere.center.x - ray.origin.x,
                     sphere.center.y - ray.origin.y,
                     sphere.center.z - ray.origin.z};
  float ray_midlen = vec3f_dot(root_dist, ray.direction);
  if (ray_midlen < 0) // if the length is negative, no use (behind ray origin)
    return 0;
  float d2 = vec3f_dot(root_dist, root_dist) - ray_midlen * ray_midlen;
  float r2 = sphere.radius * sphere.radius;
  if (d2 > r2) // if d > radius, misses the sphere completely
    return 0;
  float chord_midlen = sqrtf(r2 - d2); // right triangle
  if (t0 && t1)
    { // store the t-intersect points
      *t0 = ray_midlen - chord_midlen;
      *t1 = ray_midlen + chord_midlen;
    }
  return 1;
  /*
  // mathematical method DOESN'T WORK, but if it did, is it better?
  vec3f root_dist = {ray.origin.x - sphere.center.x,
                     ray.origin.y - sphere.center.y,
                     ray.origin.z - sphere.center.z};
  float a = vec3f_dot(ray.direction, ray.direction);
  float b = 2 * vec3f_dot(ray.direction, root_dist);
  float c = vec3f_dot(root_dist, root_dist) - sphere.radius * sphere.radius;
  float dis = b * b - 4 * a * c;
  if (dis < 0)
    return 0;
  if (t0 && t1)
    {
      float part = (b > 0) ? -0.5 * (b + sqrtf(dis)) : -0.5 * (b - sqrtf(dis));
      *t0 = part / a;
      *t1 = c / part;
      if (*t0 > *t1)
        {
          float t2 = *t0;
          *t0 = *t1;
          *t1 = t2;
        }
    }
  return 1;
  */
}


/* TOTALLY UNTESTED AND UNUSED */
int cuboid_intersects(cuboid_t cuboid, ray_t ray, float *t0, float *t1)
{
  float temp;
  float tmin = (cuboid.min.x - ray.origin.x) / ray.direction.x;
  float tmax = (cuboid.max.x - ray.origin.x) / ray.direction.x;
  if (tmin > tmax)
    {
      temp = tmin;
      tmin = tmax;
      tmax = temp;
    }
  float tminy = (cuboid.min.y - ray.origin.y) / ray.direction.y;
  float tmaxy = (cuboid.max.y - ray.origin.y) / ray.direction.y;
  if (tminy > tmaxy)
    {
      temp = tminy;
      tminy = tmaxy;
      tmaxy = temp;
    }
  if ((tmin > tmaxy) || (tminy > tmax))
    return 0;
  if (tminy > tmin)
    tmin = tminy;
  if (tmaxy < tmax)
    tmax = tmaxy;
  float tminz = (cuboid.min.z - ray.origin.z) / ray.direction.z;
  float tmaxz = (cuboid.max.z - ray.origin.z) / ray.direction.z;
  if (tminz > tmaxz)
    {
      temp = tminz;
      tminz = tmaxz;
      tmaxz = temp;
    }
  if ((tmin > tmaxz) || (tminz > tmax))
    return 0;
  if (tminz > tmin)
    tmin = tminz;
  if (tmaxz < tmax)
    tmax = tmaxz;
  if (t0 && t1)
    {
      *t0 = tmin;
      *t1 = tmax;
    }
  return 1;
}


/* TOTALLY UNTESTED AND UNUSED */
int plane_intersects(plane_t plane, ray_t ray, float *t)
{
  // normalize the vectors
  vec3f_normalize(&plane.normal);
  vec3f_normalize(&ray.direction);
  float denom = vec3f_dot(plane.normal, ray.direction);
  if (denom <= 1e-6)
    return 0;
  vec3f p0l0 = {plane.point.x - ray.origin.x,
                plane.point.y - ray.origin.y,
                plane.point.z - ray.origin.z};
  float d = vec3f_dot(p0l0, plane.normal);
  if (d < 0)
    return 0;
  if (t)
    *t = d;
  return 1;
}


/* TOTALLY UNTESTED AND UNUSED */
int disc_intersects(disc_t disc, ray_t ray, float *t)
{
  // normalize the direction vector
  vec3f_normalize(&ray.direction);
  float d;
  // check if the disc plane is intersectable (prior condition)
  if (!plane_intersects(*(plane_t*)(&disc), ray, &d))
    return 0;
  // calculate the point of intersection on the disc plane
  vec3f point = {ray.origin.x + d * ray.direction.x,
                 ray.origin.y + d * ray.direction.y,
                 ray.origin.z + d * ray.direction.z};
  // get the vector distance between the disc point and the intersection
  vec3f vecdist = {point.x - disc.point.x,
                   point.y - disc.point.y,
                   point.z - disc.point.z};
  // get the distance^2 between the disc point and the intersection point
  float d2 = vec3f_dot(vecdist,vecdist);
  // check if the intersection point is outside of the radius
  if (sqrtf(d2) > disc.radius)
    return 0;
  // we're okay
  if (t)
    *t = d;
  return 1;
}
