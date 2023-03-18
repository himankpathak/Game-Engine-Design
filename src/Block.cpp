#include "Block.h"

Block::~Block()
{
}

Block::Block(const std::string& modelFileName, Aftr::Vector scale, Aftr::MESH_SHADING_TYPE shadingType, physx::PxPhysics* pxPhysics, physx::PxScene* pxScene, Aftr::Mat4 pose)
    : IFace(this), WO() {
    this->Aftr::WO::onCreate(modelFileName, scale, shadingType);

    if (pxPhysics && pxScene) {

        physx::PxMaterial* gMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        physx::PxShape* shape = pxPhysics->createShape(physx::PxBoxGeometry(2, 2, 2), *gMaterial, true);
        physx::PxTransform t = convertPoseToPxTransform(pose);

        pxRigidDynamic = pxPhysics->createRigidDynamic(t);
        pxRigidDynamic->attachShape(*shape);
        pxRigidDynamic->userData = this;
        pxScene->addActor(*pxRigidDynamic);
    }

}

Block* Block::New(const std::string& modelFileName, Aftr::Vector scale, Aftr::MESH_SHADING_TYPE shadingType, physx::PxPhysics* pxPhysics, physx::PxScene* pxScene, Aftr::Mat4 pose)
{
    Block* newBlock = new Block(modelFileName, scale, shadingType, pxPhysics, pxScene, pose);//stuff you want to happen in descendent classes
    //newBlock->onCreate();//you can call a parent’s function within
    return newBlock;
}


float Block::degToRad(float deg) {
    return deg * Aftr::PI / 180;
}

physx::PxTransform Block::convertPoseToPxTransform(Aftr::Mat4 pose) {
    int k = 0;
    float mat[16];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mat[k] = pose.at(i, j);
            k++;
        }
    }
    auto t = physx::PxTransform(physx::PxMat44(mat));
    return t;
}

Aftr::Vector* Block::getRelativeRotation()
{
    return &curr_relativeRotationInfo;
}

Aftr::Vector* Block::getGlobalRotation()
{
    return &curr_globalRotationInfo;
}

Aftr::Vector* Block::getPos()
{
    return positionInfo;
}

Aftr::Mat4* Block::getBlockPose()
{
    return poseInfo;
}

void Block::setPos(Aftr::Vector pos)
{
    *positionInfo = pos;
    this->setPosition(*positionInfo);
    *poseInfo = this->getPose();
}

void Block::setBlockPose(Aftr::Mat4 pose)
{
    *poseInfo = pose;
    this->setPose(*poseInfo);
    *positionInfo = this->getPosition();
}

void Block::updatePoseFromPhysicsEngine()
{
    if (this && pxRigidDynamic) {
        if (pxRigidDynamic->isSleeping()) {
            pxRigidDynamic->wakeUp();
        }
        physx::PxTransform pose = pxRigidDynamic->getGlobalPose();
        physx::PxMat44 consPosition(pose);
        Aftr::Mat4 updatedPose;

        for (int i = 0; i < 16; i++) {
            updatedPose[i] = consPosition(i % 4, i / 4);
        }
        this->setBlockPose(updatedPose);

    }
}

void Block::onUpdateWO()
{
    WO::onUpdateWO();

    // translation
    this->setPosition(*positionInfo);


    // relative rotations 
    if (prev_relativeRotationInfo != curr_relativeRotationInfo) {
        this->rotateAboutRelX(degToRad(curr_relativeRotationInfo.x - prev_relativeRotationInfo.x));
        this->rotateAboutRelY(degToRad(curr_relativeRotationInfo.y - prev_relativeRotationInfo.y));
        this->rotateAboutRelZ(degToRad(curr_relativeRotationInfo.z - prev_relativeRotationInfo.z));

        *poseInfo = this->getPose();

        prev_relativeRotationInfo = curr_relativeRotationInfo;
    }

    // global rotations
    if (prev_globalRotationInfo != curr_globalRotationInfo) {
        this->rotateAboutGlobalX(degToRad(curr_globalRotationInfo.x - prev_globalRotationInfo.x));
        this->rotateAboutGlobalY(degToRad(curr_globalRotationInfo.y - prev_globalRotationInfo.y));
        this->rotateAboutGlobalZ(degToRad(curr_globalRotationInfo.z - prev_globalRotationInfo.z));

        *poseInfo = this->getPose();

        prev_globalRotationInfo = curr_globalRotationInfo;
    }

}

