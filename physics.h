#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>
#include "wobject.h"
#include "quat.h"
#include <list>

void QuatfToDQuat(Quatf_t quatf, dQuaternion dquat);
void DQuatToQuatf(dQuaternion dquat, Quatf_t quatf);

class CollContact {
  public:
    CollContact();

    WorldObject *obj;
    float distance;
    Vec3f position;
    Vec3f normal;
};

class CollQuery
{
  public:
    std::list<CollContact> contacts;
};

class Physics
{
    dWorldID odeWorld;
    dSpaceID odeSpace;
    dJointGroupID odeContacts;

    float timeStep;
    float timeStepRemainder;

    /* Singleton */
    static Physics _instance;
    Physics();
    ~Physics();
    Physics(const Physics&);
    Physics &operator=(const Physics&);
  public:
    const dWorldID &getOdeWorld() const { return odeWorld; };
    const dSpaceID &getOdeSpace() const { return odeSpace; };
    const dJointGroupID &getOdeContacts() const { return odeContacts; };

    void simulate(float dt);

    static Physics &getInstance() { return _instance; };
};

void nearCallback (void *data, dGeomID o1, dGeomID o2);

void rayCast(const Vec3f *origin, const Vec3f *dir, float len, CollQuery *collQuery);

#endif
