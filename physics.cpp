#include <math.h>
#include <ode/ode.h>
#include <iostream>
#include "quat.h"
#include "physics.h"
#include "pobject.h"
#include "world.h"

using namespace std;

Physics Physics::_instance;

void QuatfToDQuat(Quatf_t quatf, dQuaternion dquat)
{
    dquat[1] = quatf[0];
    dquat[2] = quatf[1];
    dquat[3] = quatf[2];
    dquat[0] = quatf[3];
}

void DQuatToQuatf(dQuaternion dquat, Quatf_t quatf)
{
    quatf[0] = dquat[1];
    quatf[1] = dquat[2];
    quatf[2] = dquat[3];
    quatf[3] = dquat[0];
}


Physics::Physics() : timeStep(.01), timeStepRemainder(0)
{
    dInitODE();
    odeWorld = dWorldCreate();
    odeSpace = dHashSpaceCreate(0);
    odeContacts = dJointGroupCreate(0);

    dWorldSetAutoDisableFlag(odeWorld, 1);
    dWorldSetGravity(odeWorld, 0, -10, 0);

    timeStep = .01;
}

Physics::~Physics()
{
    if (odeContacts)
        dJointGroupDestroy(odeContacts);
    if (odeSpace)
        dSpaceDestroy(odeSpace);
    if (odeWorld)
        dWorldDestroy(odeWorld);

    dCloseODE();
}

void Physics::simulate(float dt)
{
    World &world = World::getInstance();
    float desiredTimeSteps, nSteps;
    unsigned int i;

    dt += timeStepRemainder * timeStep;

    desiredTimeSteps = dt / timeStep;
    nSteps = floorf(desiredTimeSteps);
    timeStepRemainder = desiredTimeSteps - nSteps;

    for (i = 0; i < world.wobjects.size(); i++)
    {
        if (!world.wobjects[i]->pobject)
            continue;
        world.wobjects[i]->pobject->prePhysics();
    }
    for (i = 0; i < nSteps; i++)
    {
        dSpaceCollide(odeSpace, NULL, &nearCallback);
        dWorldStep(odeWorld, timeStep);
        dJointGroupEmpty(odeContacts);
    }
    for (i = 0; i < world.wobjects.size(); i++)
    {
        if (!world.wobjects[i]->pobject)
            continue;
        world.wobjects[i]->pobject->postPhysics();
    }
    if (nSteps > 1000)
        exit(0);
}

void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    Physics &p = Physics::getInstance();

    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);

    dContact contact[8];

    int numCollisions = dCollide(o1, o2, 8, &contact[0].geom, sizeof(dContact));
    if (numCollisions > 0)
    {
        for (int i = 0; i < numCollisions; i++)
        {
            contact[i].surface.mode = dContactBounce;
            contact[i].surface.bounce = 0;
            contact[i].surface.bounce_vel = 0.1;

            dJointID c = dJointCreateContact(p.getOdeWorld(),
                                             p.getOdeContacts(), contact + i);
            dJointAttach(c, b1, b2);
        }
    }
}
