#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "openrt.h"


#define WHT " \t\r\n"

#define STRTOK strtok(NULL, WHT)
#define STRTOKF atof(STRTOK)



typedef struct {
  float time;
  vec3f pos;
} inflection_t;

int compare_inflection(const void *p1, const void *p2)
{
  if (((inflection_t*)p1)->time < ((inflection_t*)p2)->time)
    return -1;
  if (((inflection_t*)p1)->time > ((inflection_t*)p2)->time)
    return 1;
  return 0;
}



typedef struct {
  char name[64];
  float start;
  float end;
  inflection_t inflection_l[128];
  size_t inflection_c;
  sphere_t sphere;
} object_t;


object_t object_l[256];
size_t object_c = 0;



void loadfile(char *filepath)
{
  FILE *file;
  char *buffer, *token;
  long int size;
  file = fopen(filepath, "r");
  assert(file != NULL);
  fseek(file, 0L, SEEK_END);
  size = ftell(file);
  rewind(file);
  buffer = malloc(size+1);
  assert(buffer != NULL);
  assert(size == fread(buffer, 1, size, file));
  fclose(file);
  token = strtok(buffer, WHT);
  do
    {
      if (token[0] == 'S') // SPHERE ___
        {
          char *name = STRTOK;
          strncpy(object_l[object_c].name, name, 64);
          object_l[object_c].inflection_c = 0;
          sphere_t sphere = {{STRTOKF,STRTOKF,STRTOKF}, STRTOKF, {STRTOKF,STRTOKF,STRTOKF}, STRTOKF, STRTOKF, {STRTOKF,STRTOKF,STRTOKF}};
          object_l[object_c].sphere = sphere;
          object_c++;
        }
      else // time ___
        {
          float time = atof(token);
          token = STRTOK;
          char *name = STRTOK;
          object_t *object = NULL;
          size_t i;
          for (i = 0; i < object_c; i++)
            {
              if (0 == strcmp(name, object_l[i].name))
                object = object_l + i;
            }
          if (token[0] == 'S') // time START ___
            {
              object->start = time;
              object->inflection_l[object->inflection_c] = (inflection_t){time, object->sphere.center};
              object->inflection_c++;
            }
          else if (token[0] == 'E') // time END ___
            {
              object->end = time;
            }
          else // time MOVE ___
            {
              object->inflection_l[object->inflection_c] = (inflection_t){time, {STRTOKF,STRTOKF,STRTOKF}};
              object->inflection_c++;
            }
        }
    }
  while ((token = STRTOK));
  free(buffer);
}

void load(const char *path)
{
  DIR *dir = opendir(path);
  assert(dir != NULL);
  struct dirent *ent;
  char filepath[512];
  while ((ent = readdir(dir)) != NULL)
    {
      if (ent->d_name[0] == '.')
        continue;
      sprintf(filepath, "%s/%s", path, ent->d_name);
      loadfile(filepath);
    }
  size_t i,j;
  for (i = 0; i < object_c; i++)
    {
      rtSphere(object_l[i].sphere);
      fprintf(stderr, "%f %f %f\n", object_l[i].sphere.center.x, object_l[i].sphere.center.y, object_l[i].sphere.center.z);
      // sort the times so they are in order
      qsort(object_l[i].inflection_l, object_l[i].inflection_c, sizeof(inflection_t), compare_inflection);
      // change the inflection deltas to absolutes
      for (j = 1; j < object_l[i].inflection_c; j++)
        {
          vec3f intermediate = {object_l[i].inflection_l[j].pos.x + object_l[i].inflection_l[j-1].pos.x,
                                object_l[i].inflection_l[j].pos.y + object_l[i].inflection_l[j-1].pos.y,
                                object_l[i].inflection_l[j].pos.z + object_l[i].inflection_l[j-1].pos.z};
          object_l[i].inflection_l[j].pos = intermediate;
        }
    }
  rtRender(30 /*i mean 45 oops*/, 800, 600, stdout);
}


int main(int argc, char **argv)
{
  load(argc > 1 ? argv[1] : "default");
  return 0;
}
