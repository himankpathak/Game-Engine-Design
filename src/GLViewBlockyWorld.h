#pragma once

#include "GLView.h"
#include "chrono"
#include "Instructions.h"
#include "Block.h"
#include "irrKlang.h"
#include "NetMessengerClient.h"
#include "NetMsgBlock.h"
#include "PxPhysicsAPI.h"
#include "CameraFrustum.h"
#include "MGLIndexedGeometry.h"
#include "IndexedGeometryLines.h"
#include "IndexedGeometryTriangles.h"
#include "GLSLShaderPerVertexColorGL32.h"

namespace Aftr
{
    class Camera;

    /**
       \class GLViewBlockyWorld
       \author Himank Pathak
       \brief A child of an abstract GLView. This class is the top-most manager of the module.

       Read \see GLView for important constructor and init information.

       \see GLView

        \{
    */

    class GLViewBlockyWorld : public GLView
    {
    public:
        static GLViewBlockyWorld* New(const std::vector<std::string>& outArgs, physx::PxPhysics* pxPhysics, physx::PxScene* pxScene);
        virtual ~GLViewBlockyWorld();
        virtual void updateWorld(); ///< Called once per frame
        virtual void loadMap();     ///< Called once at startup to build this module's scene
        virtual void createBlockyWorldWayPoints();
        virtual void onResizeWindow(GLsizei width, GLsizei height);
        virtual void onMouseDown(const SDL_MouseButtonEvent& e);
        virtual void onMouseUp(const SDL_MouseButtonEvent& e);
        virtual void onMouseMove(const SDL_MouseMotionEvent& e);
        virtual void onMouseWheelScroll(const SDL_MouseWheelEvent& e);
        virtual void onKeyDown(const SDL_KeyboardEvent& key);
        virtual void onKeyUp(const SDL_KeyboardEvent& key);
        virtual void updateActiveKeys(SDL_KeyCode keycode, bool state);

        virtual void updateControls();
        virtual void updateProjection();
        virtual void updateMusicSettings();
        virtual void placeBlock(bool proj, std::optional<int> index = std::nullopt, std::optional<Mat4> pose = std::nullopt);
        virtual void sendNetMessage(std::string action);
        virtual void updateGravity(physx::PxVec3 g);

        Block* player;
        Block* otherPlayer;

        CameraFrustum* cameraFrustumMgr = nullptr;
        WO* frustumOutline = nullptr;

    protected:
        GLViewBlockyWorld(const std::vector<std::string>& args, physx::PxPhysics* pxPhysics, physx::PxScene* pxScene);
        virtual void onCreate();

        std::vector<std::string> blocks_loc;
        int active_block_index = 1;
        std::string camera_mode = "close";
        bool center_on_camera = true;

        std::map<SDL_KeyCode, bool> active_keys;
        Aftr::Vector prev_pos;

        std::vector<Block*> blocks;
        Block* prj_block;
        NetMessengerClient* client = nullptr;
        NetMsgBlock* netBlockMgr = nullptr;

        std::chrono::high_resolution_clock::time_point tcpRetry;

        irrklang::ISoundEngine* soundEngine;
        irrklang::ISound* bg_music;
        irrklang::ISound* spooky_music;
        float bg_music_vol = 0.2f;
        std::map<std::string, bool> music_flags;

        physx::PxPhysics* pxPhysics = nullptr;
        physx::PxScene* pxScene = nullptr;
        physx::PxVec3 gravity = physx::PxVec3(0.0f, 0.0f, -9.8f);
        physx::PxVec3 prev_gravity;
    };

    /** \} */

} // namespace Aftr
