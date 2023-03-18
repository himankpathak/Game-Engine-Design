#pragma once
#include <WO.h>
#include <string>
#include <memory>
#include "PxPhysicsAPI.h"
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this

class Block : public Aftr::WO
{
public:
    virtual ~Block();

    static Block* New(const std::string& modelFileName, Aftr::Vector scale, Aftr::MESH_SHADING_TYPE shadingType, physx::PxPhysics* pxPhysics = nullptr, physx::PxScene* pxScene = nullptr, Aftr::Mat4 pose = Aftr::Mat4());

    float degToRad(float deg);
    physx::PxTransform convertPoseToPxTransform(Aftr::Mat4 pose);

    Aftr::Vector* getPos();
    void setPos(Aftr::Vector pos);
    Aftr::Mat4* getBlockPose();
    void setBlockPose(Aftr::Mat4 pose);
    Aftr::Vector* getRelativeRotation();
    Aftr::Vector* getGlobalRotation();
    void updatePoseFromPhysicsEngine();

    virtual void onUpdateWO() override;

protected:
    Aftr::Vector p = Aftr::Vector(10, 15, 4);
    Aftr::Vector* positionInfo = &p;
    Aftr::Mat4 p2 = Aftr::Mat4();
    Aftr::Mat4* poseInfo = &p2;
    Aftr::Vector curr_relativeRotationInfo, prev_relativeRotationInfo;
    Aftr::Vector curr_globalRotationInfo, prev_globalRotationInfo;

    physx::PxRigidDynamic* pxRigidDynamic = nullptr;

    Block(const std::string& modelFileName, Aftr::Vector scale, Aftr::MESH_SHADING_TYPE shadingType, physx::PxPhysics* pxPhysics, physx::PxScene* pxScene, Aftr::Mat4 pose);

};