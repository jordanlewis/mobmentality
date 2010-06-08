#ifndef GOBJECT_H
#define GOBJECT_H
#include "quat.h"

class GeomInfo;
class Vec3f;

class GObject
{
    GeomInfo *geominfo;
  public:
    GObject(GeomInfo *geominfo);

    void draw(Vec3f, Quatf_t);
};
#endif
