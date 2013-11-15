#include "openrt.h"
#include "ray.h"
#include "shapes.h"
#include "vec3f.h"

#include <float.h>
#include <math.h>

#ifndef M_TAU
#define M_TAU 6.28318530717958647693
#endif


static vec3f trace(ray_t ray, unsigned depth);


/*****************************************************************************/
/* DEALING WITH CONSTANTS */


static vec3f ambiance = {1.0f, 1.0f, 1.0f};

void rtAmbiancef(float r, float g, float b)
{
  ambiance = (vec3f){r,g,b};
}


static vec3f camera = {0.0f, 0.0f, 0.0f};

void rtCameraf(float x, float y, float z)
{
  camera = (vec3f){x,y,z};
}


static vec3f target = {0.0f, 0.0f, -1.0f};

void rtTargetf(float x, float y, float z)
{
  target = (vec3f){x,y,z};
}


static vec3f color = {1.0f, 1.0f, 1.0f};

void rtColorf(float r, float g, float b)
{
  color = (vec3f){r,g,b};
}


static vec3f emission = {0.0f, 0.0f, 0.0f};

void rtEmissionf(float r, float g, float b)
{
  emission = (vec3f){r,g,b};
}


static float reflection = 0.0f;

void rtReflectionf(float _reflection) { reflection = _reflection; }


static float transparency = 0.0f;

void rtTransparencyf(float _transparency) { transparency = _transparency; }


static unsigned maxdepth = 5;

void rtDepthu(unsigned depth) { maxdepth = depth; }


/*****************************************************************************/
/* DEALING WITH ENTITIES */


static sphere_t spheres[128];

static size_t sphere_c = 0;

void rtSpheref(float x, float y, float z, float radius)
{
  if (sphere_c >= 128)
    return;
  spheres[sphere_c] = (sphere_t){
    .center       = {x,y,z},
    .radius       = radius,
    .color        = color,
    .reflection   = reflection,
    .transparency = transparency,
    .emission     = emission
  };
  sphere_c++;
}

void rtSphere(sphere_t sphere)
{
  if (sphere_c >= 128)
    return;
  spheres[sphere_c] = sphere;
  sphere_c++;
}


void rtFlush()
{
  sphere_c = 0;
}


/*****************************************************************************/
/* GETTING VALUES */


unsigned rtGetu(char value)
{
  switch(value)
    {
    case RT_DEPTH: return maxdepth;
    case RT_SPHERE_COUNT: return (unsigned)sphere_c;
    case RT_SPHERE_MAX: return 128;
    default: return 0;
    }
}


/*****************************************************************************/
/* RENDERING */


void rtRender(float fov, unsigned width, unsigned height, FILE *output)
{
  // TODO: ADD TARGET (VIEW.DIRECTION) (aaahhh ;_;) (FOUND IT MATRICES!)
  if (!output || width == 0 || height == 0 || fov <= 0.0f)
    return;
  // the delta between the two points (the vector, essentially)
  // TODO: figure out why x and y have to be negated (aka "wat")
  vec3f viewdiff = {-(target.x - camera.x), -(target.y - camera.y), target.z - camera.z};
  vec3f_normalize(&viewdiff);
  // gluLookAt
  vec3f up = {0,1,0};
  vec3f s = vec3f_cross(viewdiff, up);
  vec3f sn = s;
  vec3f_normalize(&sn);
  vec3f u = vec3f_cross(sn, viewdiff);
  float m[] = {s.x, s.y, s.z, 0, u.x, u.y, u.z, 0, -viewdiff.x, -viewdiff.y, -viewdiff.z, 0, 0, 0, 0, 1}; // row-majour
  // pre-invert the width and height for faster calculations in the loop
  float invWidth = 1 / ((float)width), invHeight = 1 / ((float)height);
  // calculate the aspect ratio
  float aspectratio = ((float)width) / ((float)height);
  // FOV tangent: tan(fov degrees to radians conversion)
  float angle_toa = tanf((M_TAU / 4.0) * (fov / (180.0f)));
  // perform the raytracing
  unsigned x,y;
  vec3f pixel;
  fprintf(output, "P6 %u %u 255\n", width, height);
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          // (between -1 and 1) * angle_toa * aspectratio - the x coord of the xy-flattened 0d viewing rectangle
          float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle_toa * aspectratio;
          // (between -1 and 1 ) * angle_toa - the y coord of the xy-flattened 0d vieweing rectangle
          float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle_toa;
          /*
          // verbose the two corners of the viewing rectangle
          if (y == 0 && x == 0)
          fprintf(stderr, "MIN: %f %f\n", 2 * ((x + 0.5) * invWidth) - 1, 1 - 2 * ((y + 0.5) * invHeight));
          if (y == height - 1 && x == width - 1)
          fprintf(stderr, "MAX: %f %f\n", 2 * ((x + 0.5) * invWidth) - 1, 1 - 2 * ((y + 0.5) * invHeight));
          */
          // set the direction ray between the tracing point and the eye
          // note that the magnitude when at the very center of the viewing rectangle is 1
          vec3f raydir = {xx,yy,-1.0f};
          vec3f_normalize(&raydir);
          // rotate around the proper axis
          raydir = (vec3f){
            m[0]*raydir.x + m[1]*raydir.y + m[2]*raydir.z + m[3],
            m[4]*raydir.x + m[5]*raydir.y + m[6]*raydir.z + m[7],
            m[8]*raydir.x + m[9]*raydir.y + m[10]*raydir.z + m[11]
          };
          vec3f_normalize(&raydir);
          // construct the ray between the eye and the scene
          ray_t ray = {camera, raydir};
          // IT BEGIIIIIIINS!
          pixel = trace(ray, 0);
          // output the result of the tracing
          fprintf(output, "%c%c%c",
                  (unsigned char)((1.0f < pixel.x ? 1.0f : pixel.x) * 255),
                  (unsigned char)((1.0f < pixel.y ? 1.0f : pixel.y) * 255),
                  (unsigned char)((1.0f < pixel.z ? 1.0f : pixel.z) * 255));
        }
    }
}


/*****************************************************************************/
/* PRIVATE */


static vec3f trace(ray_t ray, unsigned depth)
{
  float tnear = FLT_MAX;
  sphere_t *sphere = NULL;
  size_t i,j;
  // find the nearest intersecting sphere
  for (i = 0; i < sphere_c; i++)
    {
      float t0 = FLT_MAX, t1 = FLT_MAX;
      if (sphere_intersects(spheres[i], ray, &t0, &t1))
        {
          if (t0 < 0) t0 = t1;
          if (t0 < tnear)
            {
              tnear = t0;
              sphere = spheres + i;
            }
        }
    }
  // if no intersection found, return an out-of-bounds coluor vector
  if (!sphere)
    return (vec3f){2.0f,2.0f,2.0f};
  // clear the surface colour
  vec3f surfaceColor = {0.0f,0.0f,0.0f};
  // calculate the point at which the ray intersected the sphere
  vec3f phit = {ray.origin.x + tnear * ray.direction.x,
                ray.origin.y + tnear * ray.direction.y,
                ray.origin.z + tnear * ray.direction.z};
  // calculate the normal to the intersection point of the sphere
  vec3f nhit = {phit.x - sphere->center.x,
                phit.y - sphere->center.y,
                phit.z - sphere->center.z};
  vec3f_normalize(&nhit);
  // introduce some bias to avoid intersecting the origin
  float bias = 1e-4;
  // assume the normal is not inside of the sphere
  int inside = 0;
  // check if the normal should be going inside of the sphere
  if (vec3f_dot(ray.direction, nhit) > 0)
    {
      nhit.x = -nhit.x;
      nhit.y = -nhit.y;
      nhit.z = -nhit.z;
      inside = 1;
    }
  // check if the sphere is transparent or reflective
  if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < maxdepth)
    { // do fancy transparency / reflection mathiness
      float facingratio = -vec3f_dot(ray.direction, nhit);
      // crazy fresnel effect on different refraction indices
      float fresneleffect = 1 * 0.1 + powf(1.0f - facingratio, 3) * 0.9;
      // calculate the vector of reflection
      float refldir_ = 2 * vec3f_dot(ray.direction, nhit);
      vec3f refldir = {ray.direction.x - nhit.x * refldir_,
                       ray.direction.y - nhit.y * refldir_,
                       ray.direction.z - nhit.z * refldir_};
      vec3f_normalize(&refldir);
      ray_t reflection_ = {
        {phit.x + bias * nhit.x,
         phit.y + bias * nhit.y,
         phit.z + bias * nhit.z},
        refldir};
      // r-r-r-recurse to calculate what's reflected
      vec3f reflection = trace(reflection_, depth + 1);
      // calculate the vector of refraction
      vec3f refraction = {0.0f,0.0f,0.0f};
      if (sphere->transparency)
        { // transparency means refraction is non-zero
          float ior = 1.1, eta = (inside) ? ior : 1 / ior;
          float cosi = -vec3f_dot(nhit, ray.direction);
          float k = 1 - eta * eta * (1 - cosi * cosi);
          float refrdir_ = (eta * cosi - sqrtf(k));
          vec3f refrdir = {eta * ray.direction.x + refrdir_ * nhit.x,
                           eta * ray.direction.y + refrdir_ * nhit.y,
                           eta * ray.direction.z + refrdir_ * nhit.z};
          vec3f_normalize(&refrdir);
          ray_t refraction_ = {
            {phit.x - bias * nhit.x,
             phit.y - bias * nhit.y,
             phit.z - bias * nhit.z},
            refrdir};
          // r-r-r-recurse to obtain the refraction colour
          refraction = trace(refraction_, depth + 1);
        }
      // the ultimate equation: calculate the colour
      surfaceColor = (vec3f){(reflection.x * fresneleffect + refraction.x * (1 - fresneleffect) * sphere->transparency) * sphere->color.x,
                             (reflection.y * fresneleffect + refraction.y * (1 - fresneleffect) * sphere->transparency) * sphere->color.y,
                             (reflection.z * fresneleffect + refraction.z * (1 - fresneleffect) * sphere->transparency) * sphere->color.z};
    }
  else
    { // the sphere is not transparent or reflective (yay opacity)
      for (i = 0; i < sphere_c; i++)
        { // check for light sources
          if (spheres[i].emission.x > 0)
            { // check the effect of the light source on this object
              vec3f transmission = {1.0f,1.0f,1.0f};
              vec3f lightDirection = {spheres[i].center.x - phit.x,
                                      spheres[i].center.y - phit.y,
                                      spheres[i].center.z - phit.z};
              vec3f_normalize(&lightDirection);
              // check for shadowing on this object
              for (j = 0; j < sphere_c; j++)
                {
                  if (i != j)
                    {
                      float t0, t1;
                      ray_t passray = {{phit.x + bias * nhit.x,
                                        phit.y + bias * nhit.y,
                                        phit.z + bias * nhit.z},
                                       lightDirection};
                      if (sphere_intersects(spheres[j], passray, &t0, &t1))
                        { // shadowing means there's no transmission
                          transmission = (vec3f){0.0f,0.0f,0.0f};
                          break;
                        }
                    }
                }
              // artificially cap this as positive
              float crack = (0.0f > vec3f_dot(nhit, lightDirection) ? 0.0f : vec3f_dot(nhit, lightDirection));
              surfaceColor = (vec3f){surfaceColor.x + sphere->color.x * transmission.x * crack * spheres[i].emission.x,
                                     surfaceColor.y + sphere->color.y * transmission.y * crack * spheres[i].emission.y,
                                     surfaceColor.z + sphere->color.z * transmission.z * crack * spheres[i].emission.z};
            }
        }
    }
  // combine the calculated surface colour with the sphere's emission colour
  vec3f result = {surfaceColor.x + sphere->emission.x,
                  surfaceColor.y + sphere->emission.y,
                  surfaceColor.z + sphere->emission.z};
  return result;
}
