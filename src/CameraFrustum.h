#pragma once 

#include "vector"
#include "WO.h"
#include "Camera.h"
#include "Model.h"
#include "AftrGeometryFrustum.h"
#include "Block.h"

namespace Aftr {
    class CameraFrustum : public AftrGeometryFrustum {
    public:
        CameraFrustum(std::vector<Block*>* cullableObjectsList);
        CameraFrustum(std::vector<Block*>* cullableObjectsList, float widthToHeight, float verticalFOVDeg, float near, float far, Aftr::Vector lookDirection, Aftr::Vector normalDirection, Aftr::Vector cameraPosition);
        virtual ~CameraFrustum();

        virtual void updateFrustum(Camera* camera, WO* frustum, std::string camera_mode);

        bool checkIfBlockInFrustum(Block* wo);
        void cullBlock(Block* wo);
        void enableBlock(Block* wo);
        void printBlockCoord(Block* wo);

    private:
        WO* frustumVisualObj = nullptr;
        std::vector<Block*>* cullableObjects;
        std::vector<unsigned int> culledObjIDs;

        Aftr::Vector position, lookDir, normalDirection;
        float widthToHeight, verticalFOVDeg, nearPlane, farPlane;

    };
}