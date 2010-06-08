#ifndef STEERING_H
#define STEERING_H
class SteerInfo
{
  public:
    SteerInfo(Vec3f acc=Vec3f(0,0,0), float rot=0.0)
        : acc(acc), rot(rot) {}
    Vec3f acc;
    float rot;
};

#endif
