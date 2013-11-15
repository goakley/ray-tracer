#ifndef OPENRT_H
#define OPENRT_H

#include "shapes.h"
#include "vec3f.h"

#include <stdio.h>

/**
 * Sets the colour used when no shape is in view
 */
void rtAmbiancef(float r, float g, float b);

/**
 * Positions the camera at a point
 */
void rtCameraf(float x, float y, float z);

/**
 * Targets the camera at a point
 */
void rtTargetf(float x, float y, float z);

/**
 * Sets the current colour
 */
void rtColorf(float r, float g, float b);

/**
 * Sets the current emission colour
 */
void rtEmissionf(float r, float g, float b);

/**
 * Sets the current reflectivity
 */
void rtReflectionf(float reflection);

/**
 * Sets the current transparency
 */
void rtTransparencyf(float transparency);

/**
 * Sets the maximum render depth
 */
void rtDepthu(unsigned depth);

#define RT_DEPTH 0
#define RT_SPHERE_COUNT 1
#define RT_SPHERE_MAX 2

/**
 * Gets the current value of an unsigned value
 *
 * RT_DEPTH - The maximum render depth used when rendering
 * RT_SPHERE_COUNT - The number of spheres currently in the scene
 * RT_SPHERE_MAX - The maximum number of spheres the scene can hold
 */
unsigned rtGetu(char value);

/**
 * Adds a sphere entity to the scene
 */
void rtSphere(sphere_t sphere);
void rtSpheref(float x, float y, float z, float radius);

/**
 * Renders the scene into the PPM P6 image format
 */
void rtRender(float fov, /**< The field of view */
              unsigned width, /**< The pixel width of the image to render */
              unsigned height, /**< The pixel height of the image to render */
              FILE *output /**< A stream that will accept the image data */
              );

/**
 * Removes all entities from the scene
 */
void rtFlush();

#endif
