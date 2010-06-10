#include <ode/ode.h>
#include <iostream>
#include "pobject.h"
#include "physics.h"
#include "quat.h"
#include "geominfo.h"
#include "world.h"

PObject::PObject(GeomInfo *info) : collType(REAL)
{
    space = Physics::getInstance().getOdeSpace();
    geom  = info->createGeom(space);
    dGeomSetData(geom, this);
}

PObject::~PObject()
{
    dGeomDestroy(geom);
}

bool PObject::isPlaceable()
{
    int c = dGeomGetClass(geom);
    if (c == dPlaneClass)
        return false;
    return true;
}

void PObject::getQuat(Quatf_t quat)
{
    dReal dquat[4];
    if (isPlaceable())
    {
        dGeomGetQuaternion(geom, dquat);
        DQuatToQuatf(dquat, quat);
    }
    else
    {
        quat[0] = 0;
        quat[1] = 0;
        quat[2] = 0;
        quat[3] = 1;
    }

}

Vec3f PObject::getPos()
{
    if (isPlaceable())
    {
        const dReal *pos = dGeomGetPosition(geom);
        return Vec3f(pos[0], pos[1], pos[2]);
    }
    else
        return Vec3f(0,0,0);
}

PMoveable::PMoveable(Kinematic &kinematic, float mass, GeomInfo *info)
           : PObject(info), kinematic(kinematic)
{
    dQuaternion q;
    body = dBodyCreate(Physics::getInstance().getOdeWorld());
    info->createMass(&this->mass, mass);

    dBodySetMass(body, &this->mass);
    dGeomSetBody(geom, body);

    kinematicToOde();

    dQFromAxisAndAngle(q, 0, 1, 0, kinematic.orientation);
    dBodySetQuaternion(body, q);
}

const Kinematic &PMoveable::getKinematic()
{
    return kinematic;
}

void PMoveable::kinematicToOde()
{
    Kinematic &k = kinematic;
    dBodySetPosition(body, k.pos[0], k.pos[1], k.pos[2]);
    dBodySetLinearVel(body, k.vel[0], k.vel[1], k.vel[2]);
}

Kinematic PMoveable::odeToKinematic()
{
    Kinematic k;
    dQuaternion q_result, q_result1, q_base;
    float norm;
    const dReal *b_info;
    const dReal *q_current = dBodyGetQuaternion(body);

    q_base[0] = 0; q_base[1] = 0; q_base[2] = 0; q_base[3] = 1;
    dQMultiply0(q_result1, q_current, q_base);
    dQMultiply2(q_result, q_result1, q_current);

    k.orientation_v = Vec3f(q_result[1], q_result[2], q_result[3]);
    norm = sqrt(q_result[1] * q_result[1] + q_result[3] * q_result[3]);

    if (norm == 0)
        k.orientation = 0;
    else
        k.orientation = atan2(q_result[1] / norm, q_result[3] / norm);

    b_info = dBodyGetPosition(body);
    k.pos = Vec3f(b_info[0], b_info[1], b_info[2]);
    b_info = dBodyGetLinearVel(body);
    k.vel = Vec3f(b_info[0], b_info[1], b_info[2]);

    return k;
}

void PMoveable::prePhysics()
{
    return;
}

void PMoveable::postPhysics()
{
    kinematic = odeToKinematic();
}

PSteerable::PSteerable(SteerInfo &steerinfo, Kinematic &kinematic, float mass,
                       GeomInfo *info)
           : PMoveable(kinematic, mass, info), steerInfo(steerinfo)
{
}

const SteerInfo &PSteerable::getSteering()
{
    return steerInfo;
}

void PSteerable::setSteering(SteerInfo steerInfo)
{
    this->steerInfo = steerInfo;
}

void PSteerable::steeringToOde()
{
    if (steerInfo.acc.length() != 0 || steerInfo.rot)
        dBodyEnable(body);

    Vec3f f = steerInfo.acc * mass.mass;
    const dReal *angVel = dBodyGetAngularVel(body);
    dBodySetAngularVel(body, angVel[0], angVel[1] + steerInfo.rot,
                       angVel[2]);
    dBodyAddForce(body, f[0], f[1], f[2]);
}

void PSteerable::resetOdeAngularVelocity()
{
    const dReal *angVel = dBodyGetAngularVel(body);
    dBodySetAngularVel(body, angVel[0], angVel[1] - steerInfo.rot, angVel[2]);
}

void PSteerable::prePhysics()
{
    int n = World::size;
    if (kinematic.pos.x > n)
        kinematic.pos.x = -n;
    else if (kinematic.pos.x < -n)
        kinematic.pos.x = n;
    if (kinematic.pos.z > n)
        kinematic.pos.z = -n;
    else if (kinematic.pos.z < -n)
        kinematic.pos.z = n;
    kinematicToOde();
    steeringToOde();
    PMoveable::prePhysics();
}

void PSteerable::postPhysics()
{
    resetOdeAngularVelocity();
    PMoveable::postPhysics();
}
