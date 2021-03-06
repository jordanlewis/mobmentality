#ifndef POBJECT_H
#define POBJECT_H
#include <ode/ode.h>
#include "kinematic.h"
#include "steering.h"
#include "vec3f.h"
#include "quat.h"

class GeomInfo;
class WorldObject;

typedef enum {
    REAL = 0,
    PHANTOM
} CollType_t;

class PObject
{
  protected:
    dGeomID geom;
  public:
    dSpaceID space;
    PObject(GeomInfo *info);
    ~PObject();

    WorldObject *wobject;

    CollType_t collType;


    bool isPlaceable();
    void getQuat(Quatf_t);
    Vec3f getPos();

    const dGeomID &getGeom() { return geom; };

    virtual void prePhysics() {return;};
    virtual void postPhysics() {return;};
};

class PMoveable: public PObject
{
  protected:
    Kinematic kinematic;
    dMass mass;
    dBodyID body;
  public:
    PMoveable(Kinematic &kinematic, float mass, GeomInfo *info);

    const Kinematic &getKinematic();

    virtual void prePhysics();
    virtual void postPhysics();
    Kinematic odeToKinematic();
    void kinematicToOde();
};

class PSteerable: public PMoveable
{
  protected:
    SteerInfo steerInfo;
  public:
    PSteerable(SteerInfo &steerinfo, Kinematic &kinematic, float mass,
               GeomInfo *info);

    const SteerInfo &getSteering();
    void setSteering(SteerInfo steerInfo);

    virtual void postPhysics();
    virtual void prePhysics();
    void resetOdeAngularVelocity();
    void steeringToOde();
};

#endif
