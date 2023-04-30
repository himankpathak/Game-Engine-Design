#include "CameraFrustum.h"


Aftr::CameraFrustum::CameraFrustum(std::vector<Block*>* cullableObjectsList) :
    cullableObjects(cullableObjectsList),
    Aftr::AftrGeometryFrustum()
{
}

Aftr::CameraFrustum::CameraFrustum(std::vector<Block*>* cullableObjectsList, float widthToHeight, float verticalFOVDeg, float near, float far, Aftr::Vector lookDirection, Aftr::Vector normalDirection, Aftr::Vector cameraPosition) :
    cullableObjects(cullableObjectsList),
    Aftr::AftrGeometryFrustum(widthToHeight, verticalFOVDeg, near, far, lookDirection, normalDirection, cameraPosition)
{
}

Aftr::CameraFrustum::~CameraFrustum()
{
    this->AftrGeometryFrustum::~AftrGeometryFrustum();
}

void Aftr::CameraFrustum::updateFrustum(Camera* camera, WO* frustum, std::string camera_mode)
{
    auto widthToHeight = camera->getCameraAspectRatioWidthToHeight();
    auto verticalFOVDeg = camera->getCameraVerticalFOVDeg();
    auto near = camera->getCameraNearClippingPlaneDistance();
    auto far = camera->getCameraFarClippingPlaneDistance();
    widthToHeight = 4.0f / 3.0f;
    float horzFOVDeg = 90.0f;
    verticalFOVDeg = 2.0f * std::atan(std::tan(horzFOVDeg * Aftr::DEGtoRAD / 2.0f) * (1.0f / widthToHeight)) * Aftr::RADtoDEG;
    nearPlane = 1.0f;
    farPlane = 50.0f;


    if (camera_mode == "close") {
        lookDir = camera->getLookDirection();
        normalDirection = camera->getNormalDirection();
        position = camera->getPosition();
    }
    else {
        lookDir = frustum->getLookDirection();
        normalDirection = frustum->getNormalDirection();
        position = frustum->getPosition();
    }

    this->init(widthToHeight, verticalFOVDeg, nearPlane, farPlane, lookDir, normalDirection, position);

    for (size_t i = 0; i < cullableObjects->size(); i++) {
        Block* wo = cullableObjects->at(i);
        bool testIsWithinFrustum = checkIfBlockInFrustum(cullableObjects->at(i));

        if (std::find(culledObjIDs.begin(), culledObjIDs.end(), wo->getID()) != culledObjIDs.end()) {
            if (testIsWithinFrustum) {
                enableBlock(wo);
            }
        }
        else {

            if (!testIsWithinFrustum) {
                cullBlock(wo);
            }
        }

    }
}



bool Aftr::CameraFrustum::checkIfBlockInFrustum(Block* wo)
{
    Aftr::Vector boundingBoxLocations[8];
    Aftr::Mat4 displayMatrix = wo->getDisplayMatrix();

    auto boundingBox = wo->getModel()->getBoundingBox().getlxlylz();
    auto currentPosition = wo->getPosition();

    for (size_t i = 0; i < 8; i++) {
        switch (i) {
        case(0):
            boundingBoxLocations[i] = Aftr::Vector{ currentPosition.x - (boundingBox.x / 2),
                                            currentPosition.y - (boundingBox.y / 2),
                                            currentPosition.z - (boundingBox.z / 2) };
            break;
        case(1):
            boundingBoxLocations[i] = Aftr::Vector{ currentPosition.x + (boundingBox.x / 2),
                                            currentPosition.y - (boundingBox.y / 2),
                                            currentPosition.z - (boundingBox.z / 2) };
            break;
        case(2):
            boundingBoxLocations[i] = Aftr::Vector{ currentPosition.x - (boundingBox.x / 2),
                                            currentPosition.y + (boundingBox.y / 2),
                                            currentPosition.z - (boundingBox.z / 2) };
            break;
        case(3):
            boundingBoxLocations[i] = Aftr::Vector{ currentPosition.x + (boundingBox.x / 2),
                                            currentPosition.y + (boundingBox.y / 2),
                                            currentPosition.z - (boundingBox.z / 2) };
            break;
        case(4):
            boundingBoxLocations[i] = Aftr::Vector{ currentPosition.x - (boundingBox.x / 2),
                                            currentPosition.y - (boundingBox.y / 2),
                                            currentPosition.z + (boundingBox.z / 2) };
            break;
        case(5):
            boundingBoxLocations[i] = Aftr::Vector{ currentPosition.x + (boundingBox.x / 2),
                                            currentPosition.y - (boundingBox.y / 2),
                                            currentPosition.z + (boundingBox.z / 2) };
            break;
        case(6):
            boundingBoxLocations[i] = Aftr::Vector{ currentPosition.x - (boundingBox.x / 2),
                                            currentPosition.y + (boundingBox.y / 2),
                                            currentPosition.z + (boundingBox.z / 2) };
            break;
        case(7):
            boundingBoxLocations[i] = Aftr::Vector{ currentPosition.x + (boundingBox.x / 2),
                                            currentPosition.y + (boundingBox.y / 2),
                                            currentPosition.z + (boundingBox.z / 2) };
            break;
        }

        boundingBoxLocations[i] = displayMatrix * boundingBoxLocations[i];

        if (this->isInFrustum(boundingBoxLocations[i])) {
            return true;
        }
    }

    Aftr::Vector normal;
    for (int i = 0; i < 6; i++) {
        int counter = 0;
        normal = this->getPlaneNormal(i);

        for (size_t j = 0; j < 8; j++) {
            if (boundingBoxLocations[j].dotProduct(normal) < this->getPlaneCoef(i)) {
                counter++;
            }
        }

        if (counter == 8) {
            return false;
        }
    }

    return true;
}

void Aftr::CameraFrustum::cullBlock(Block* wo)
{
    wo->isVisible = false;
    culledObjIDs.push_back(wo->getID());

    std::cout << ">>>>>>>>>       Object is being culled     <<<<<<<<<<" << std::endl;
    printBlockCoord(wo);
}

void Aftr::CameraFrustum::enableBlock(Block* wo)
{
    wo->isVisible = true;

    culledObjIDs.erase(std::remove(culledObjIDs.begin(), culledObjIDs.end(), wo->getID()), culledObjIDs.end());

    std::cout << ">>>>>>>>>        Object is being enabled     <<<<<<<<<<" << std::endl;
    printBlockCoord(wo);
}

void Aftr::CameraFrustum::printBlockCoord(Block* wo)
{
    Aftr::Vector position = wo->getPosition();
    std::cout << "Coordinates: " << position.x << "," << position.y << "," << position.z << std::endl;
}
