#pragma once

#include "GLView.h"
#include "Instructions.h"

namespace Aftr
{
    class Camera;

    /**
       \class GLViewFirstABurnModule
       \author Scott Nykl
       \brief A child of an abstract GLView. This class is the top-most manager of the module.

       Read \see GLView for important constructor and init information.

       \see GLView

        \{
    */

    class GLViewFirstABurnModule : public GLView
    {
    public:
        static GLViewFirstABurnModule* New(const std::vector< std::string >& outArgs);
        virtual ~GLViewFirstABurnModule();
        virtual void updateWorld(); ///< Called once per frame
        virtual void loadMap(); ///< Called once at startup to build this module's scene
        virtual void createFirstABurnModuleWayPoints();
        virtual void onResizeWindow(GLsizei width, GLsizei height);
        virtual void onMouseDown(const SDL_MouseButtonEvent& e);
        virtual void onMouseUp(const SDL_MouseButtonEvent& e);
        virtual void onMouseMove(const SDL_MouseMotionEvent& e);
        virtual void onKeyDown(const SDL_KeyboardEvent& key);
        virtual void onKeyUp(const SDL_KeyboardEvent& key);
        virtual void placeBox();

    protected:
        GLViewFirstABurnModule(const std::vector< std::string >& args);
        virtual void onCreate();

    };

    /** \} */

} //namespace Aftr
