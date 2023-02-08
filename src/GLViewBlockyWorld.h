#pragma once

#include "GLView.h"
#include "Instructions.h"
#include "Block.h"

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
        static GLViewBlockyWorld *New(const std::vector<std::string> &outArgs);
        virtual ~GLViewBlockyWorld();
        virtual void updateWorld(); ///< Called once per frame
        virtual void loadMap();     ///< Called once at startup to build this module's scene
        virtual void createBlockyWorldWayPoints();
        virtual void onResizeWindow(GLsizei width, GLsizei height);
        virtual void onMouseDown(const SDL_MouseButtonEvent &e);
        virtual void onMouseUp(const SDL_MouseButtonEvent &e);
        virtual void onMouseMove(const SDL_MouseMotionEvent &e);
        virtual void onKeyDown(const SDL_KeyboardEvent &key);
        virtual void onKeyUp(const SDL_KeyboardEvent &key);
        virtual void updateActiveKeys(SDL_KeyCode keycode, bool state);

        virtual void placeBox(bool proj);
        virtual void updateProjection();

    protected:
        GLViewBlockyWorld(const std::vector<std::string> &args);
        virtual void onCreate();

        std::string cube_loc, cube_proj_loc;
        bool center_on_camera = true;
        std::map<SDL_KeyCode, bool> active_keys;

        std::vector<WO*> blocks;
        Block* prj_block;
    };

    /** \} */

} // namespace Aftr
