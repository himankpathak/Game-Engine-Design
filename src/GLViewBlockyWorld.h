#pragma once

#include "GLView.h"
#include "chrono"
#include "Instructions.h"
#include "Block.h"
#include "irrKlang.h"
#include <NetMessengerClient.h>
#include <NetMsgBlock.h>

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
        static GLViewBlockyWorld* New(const std::vector<std::string>& outArgs);
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
        virtual void placeBlock(bool proj);

        Block* player;
        Block* otherPlayer;

    protected:
        GLViewBlockyWorld(const std::vector<std::string>& args);
        virtual void onCreate();

        std::vector<std::string> blocks_loc;
        int active_block_index = 0;
        std::string camera_mode = "close";
        bool center_on_camera = true;
        std::map<SDL_KeyCode, bool> active_keys;
        Aftr::Vector prev_pos;

        std::vector<WO*> blocks;
        Block* prj_block;
        NetMessengerClient* client = nullptr;
        NetMsgBlock* netBlockMgr = nullptr;

        std::chrono::high_resolution_clock::time_point tcpRetry;

        irrklang::ISoundEngine* soundEngine;
        irrklang::ISound* bg_music;
        irrklang::ISound* spooky_music;
        float bg_music_vol = 0.2f;
        std::map<std::string, bool> music_flags;

    };

    /** \} */

} // namespace Aftr
