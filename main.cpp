#include <unistd.h>
#include "ai.h"
#include "world.h"
#include "wobject.h"
#include "pobject.h"
#include "physics.h"
#include "geominfo.h"
#include "graphics.h"
#include "gobject.h"
#include "defs.h"
#include "input.h"

int main(int argc, char **argv)
{
    World &world = World::getInstance();
    Input &input = Input::getInstance();
    Graphics &graphics = Graphics::getInstance();

    PlaneInfo info = PlaneInfo(0, 1, 0, -5);
    PObject *pobj = new PObject(&info);
    GObject *gobj = new GObject(&info);
    WorldObject *wobj = new WorldObject(pobj, gobj);
    world.addObject(wobj);

    BoxInfo box = BoxInfo(1,1,1);
    Kinematic k = Kinematic(Vec3f(0,10,0));
    SteerInfo s = SteerInfo(Vec3f(1, 0, 0));
    PSteerable *psteer = new PSteerable(s, k, 100.0f, &box);
    gobj = new GObject(&box);
    wobj = new WorldObject (psteer, gobj);
    world.addObject(wobj);

    AIController aic = AIController(psteer);
    Seek *seek = new Seek();
    seek->target = Kinematic(Vec3f(0,0,1));
    aic.setStrategy(seek);


    double now;
    double last = GetTime();
    Physics &physics = Physics::getInstance();
    while (!world.done)
    {
        now = GetTime();
        input.processInput();
        physics.simulate(now - last);
        graphics.render();
        last = now;
        usleep(10000);
    }
}

