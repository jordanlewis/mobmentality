#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "wobject.h"
class World
{
    World();
    ~World();
    World(const World&);
    World& operator=(const World&);
    static World _instance;
  public:
    static World &getInstance() {return _instance;};

    static int size;
    bool done;
    std::vector<WorldObject *> wobjects;
    void addObject(WorldObject *obj);
    void addAgent();

    void cleanObjects();
};
#endif
