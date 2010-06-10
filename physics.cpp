#include <math.h>
#include <ode/ode.h>
#include <iostream>
#include "quat.h"
#include "physics.h"
#include "pobject.h"
#include "world.h"
#include "geominfo.h"

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

CollContact::CollContact () : obj(NULL), distance(0),
                              position(Vec3f(0,0,0)), normal(Vec3f(0,0,0))
{}

void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    Physics &p = Physics::getInstance();

    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);

    PObject *p1 = (PObject *)dGeomGetData(o1);
    PObject *p2 = (PObject *)dGeomGetData(o2);

    if (p2->collType == PHANTOM)
    {
        return;
    }

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

void rayCast(const Vec3f *origin, const Vec3f *dir, float len, CollQuery *collQuery)
{
    Physics &physics = Physics::getInstance();
    RayInfo info = RayInfo(len);
    PObject rayGeom(&info);
    //dGeomID rayGeom = dCreateRay(physics.getOdeSpace(), ray.len);
    rayGeom.collType = PHANTOM;
    dGeomRaySetLength(rayGeom.getGeom(), info.len);
    dGeomRaySet(rayGeom.getGeom(), (*origin)[0], (*origin)[1], (*origin)[2],
                (*dir)[0], (*dir)[1], (*dir)[2]);
    dSpaceCollide2(rayGeom.getGeom(),
                   (dGeomID)physics.getOdeSpace(), collQuery, &nearCallback);
}

