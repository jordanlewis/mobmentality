#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "vec3f.h"

class WorldObject;
class AIController;

class Graphics
{
    Graphics();
    ~Graphics();
    Graphics(const Graphics&);
    Graphics &operator=(const Graphics &);
    static Graphics _instance;

  public:
    int wres, hres;
    void render();
    void render(WorldObject *);
    void render(AIController *);
    void DrawArrow(Vec3f pos, Vec3f dir);

    static Graphics &getInstance() {return _instance;};
};
#endif
