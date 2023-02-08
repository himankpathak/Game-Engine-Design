#include "Block.h"

Block::~Block()
{
}

Block::Block(const std::string& modelFileName, Aftr::Vector scale, Aftr::MESH_SHADING_TYPE shadingType)
    : IFace(this), WO() {
    this->Aftr::WO::onCreate(modelFileName, scale, shadingType);
}

Block* Block::New(const std::string& modelFileName, Aftr::Vector scale, Aftr::MESH_SHADING_TYPE shadingType)
{
    Block* newBlock = new Block(modelFileName, scale, shadingType);//stuff you want to happen in descendent classes
    //newBlock->onCreate();//you can call a parent’s function within
    return newBlock;
}


float Block::degToRad(float deg) {
    return deg * Aftr::PI / 180;
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
    return &positionInfo;
}

void Block::onUpdateWO()
{
    WO::onUpdateWO();

    // translation
    this->setPosition(positionInfo);

    // relative rotations 
    this->rotateAboutRelX(degToRad(curr_relativeRotationInfo.x - prev_relativeRotationInfo.x));
    this->rotateAboutRelY(degToRad(curr_relativeRotationInfo.y - prev_relativeRotationInfo.y));
    this->rotateAboutRelZ(degToRad(curr_relativeRotationInfo.z - prev_relativeRotationInfo.z));

    prev_relativeRotationInfo = curr_relativeRotationInfo;

    // global rotations
    this->rotateAboutGlobalX(degToRad(curr_globalRotationInfo.x - prev_globalRotationInfo.x));
    this->rotateAboutGlobalY(degToRad(curr_globalRotationInfo.y - prev_globalRotationInfo.y));
    this->rotateAboutGlobalZ(degToRad(curr_globalRotationInfo.z - prev_globalRotationInfo.z));

    prev_globalRotationInfo = curr_globalRotationInfo;

}

