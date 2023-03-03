#include "GLViewBlockyWorld.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"

using namespace Aftr;

GLViewBlockyWorld* GLViewBlockyWorld::New(const std::vector< std::string >& args)
{
    GLViewBlockyWorld* glv = new GLViewBlockyWorld(args);
    glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
    glv->onCreate();
    return glv;
}


GLViewBlockyWorld::GLViewBlockyWorld(const std::vector< std::string >& args) : GLView(args)
{
    //Initialize any member variables that need to be used inside of LoadMap() here.
    //Note: At this point, the Managers are not yet initialized. The Engine initialization
    //occurs immediately after this method returns (see GLViewBlockyWorld::New() for
    //reference). Then the engine invoke's GLView::loadMap() for this module.
    //After loadMap() returns, GLView::onCreate is finally invoked.

    //The order of execution of a module startup:
    //GLView::New() is invoked:
    //    calls GLView::init()
    //       calls GLView::loadMap() (as well as initializing the engine's Managers)
    //    calls GLView::onCreate()

    //GLViewBlockyWorld::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewBlockyWorld::onCreate()
{
    //GLViewBlockyWorld::onCreate() is invoked after this module's LoadMap() is completed.
    //At this point, all the managers are initialized. That is, the engine is fully initialized.

    if (this->pe != NULL)
    {
        //optionally, change gravity direction and magnitude here
        //The user could load these values from the module's aftr.conf
        this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
        this->pe->setGravityScalar(Aftr::GRAVITY);
    }
    this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
    //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewBlockyWorld::~GLViewBlockyWorld()
{
    //Implicitly calls GLView::~GLView()
    //GLView::~GLView();
}


void GLViewBlockyWorld::updateWorld()
{
    GLView::updateWorld(); //Just call the parent's update world first.
    //If you want to add additional functionality, do it after
    //this call.

    updateControls();
    updateProjection();
    updateMusicSettings();

    auto timeNow = std::chrono::high_resolution_clock::now();
    auto retryAfter = std::chrono::duration_cast<std::chrono::seconds>(timeNow - tcpRetry).count();
    if (!client->isTCPSocketOpen() && retryAfter > 15)
    {
        tcpRetry = timeNow;
        this->client = NetMessengerClient::New("127.0.0.1", ManagerEnvironmentConfiguration::getVariableValue("Port"));
    }

    sendNetMessage("syncPlayer");
}


void GLViewBlockyWorld::onResizeWindow(GLsizei width, GLsizei height)
{
    GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewBlockyWorld::onMouseDown(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseDown(e);
}


void GLViewBlockyWorld::onMouseUp(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseUp(e);
}


void GLViewBlockyWorld::onMouseMove(const SDL_MouseMotionEvent& e)
{
    GLView::onMouseMove(e);
}

void GLViewBlockyWorld::onMouseWheelScroll(const SDL_MouseWheelEvent& e)
{
    GLView::onMouseWheelScroll(e);

    auto cvel = this->getCamera()->getCameraVelocity();
    if (e.y > 0) {
        this->getCamera()->setCameraVelocity(cvel + 0.25f);
    }
    else if (e.y < 0 && cvel > 0.5) {
        this->getCamera()->setCameraVelocity(cvel - 0.25f);
    }
}

void GLViewBlockyWorld::onKeyDown(const SDL_KeyboardEvent& key)
{
    GLView::onKeyDown(key);
    if (key.keysym.sym == SDLK_0)
        this->setNumPhysicsStepsPerRender(1);

    if (key.keysym.sym >= 48 && key.keysym.sym <= 57) {
        active_block_index = key.keysym.sym - 48;
    }

    if (key.keysym.sym == SDLK_p)
    {
        prj_block->isVisible = !prj_block->isVisible;
    }

    if (key.keysym.sym == SDLK_TAB) {
        prj_block->getModel()->useNextSkin();
    }

    if (key.keysym.sym == SDLK_c)
    {
        if (camera_mode == "close") {
            camera_mode = "free";
        }
        else {
            camera_mode = "close";
        }
    }

    if (key.keysym.sym == SDLK_SPACE)
    {
        placeBlock(false);
        sendNetMessage("placeBlock");
    }

    if (key.keysym.sym == SDLK_w)
    {
        active_keys[SDLK_w] = true;
    }
    if (key.keysym.sym == SDLK_s)
    {
        active_keys[SDLK_s] = true;
    }
    if (key.keysym.sym == SDLK_d)
    {
        active_keys[SDLK_d] = true;
    }
    if (key.keysym.sym == SDLK_a)
    {
        active_keys[SDLK_a] = true;
    }
    if (key.keysym.sym == SDLK_LSHIFT)
    {
        active_keys[SDLK_LSHIFT] = true;
    }
    if (key.keysym.sym == SDLK_LCTRL)
    {
        active_keys[SDLK_LCTRL] = true;
    }
}

void GLViewBlockyWorld::onKeyUp(const SDL_KeyboardEvent& key)
{
    GLView::onKeyUp(key);

    if (key.keysym.sym == SDLK_w)
    {
        active_keys[SDLK_w] = false;
    }
    if (key.keysym.sym == SDLK_s)
    {
        active_keys[SDLK_s] = false;
    }
    if (key.keysym.sym == SDLK_d)
    {
        active_keys[SDLK_d] = false;
    }
    if (key.keysym.sym == SDLK_a)
    {
        active_keys[SDLK_a] = false;
    }
    if (key.keysym.sym == SDLK_LSHIFT)
    {
        active_keys[SDLK_LSHIFT] = false;
    }
    if (key.keysym.sym == SDLK_LCTRL)
    {
        active_keys[SDLK_LCTRL] = false;
    }
}

void GLViewBlockyWorld::updateActiveKeys(SDL_KeyCode keycode, bool state)
{
    active_keys[keycode] = state;
}

void Aftr::GLViewBlockyWorld::loadMap()
{
    this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
    this->actorLst = new WorldList();
    this->netLst = new WorldList();

    ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
    ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
    ManagerOpenGLState::enableFrustumCulling = false;
    Axes::isVisible = true;
    this->glRenderer->isUsingShadowMapping(true); //set to TRUE to enable shadow mapping, must be using GL 3.2+

    this->cam->setPosition(0, 0, 15);

    soundEngine = irrklang::createIrrKlangDevice();
    bg_music = soundEngine->play2D((ManagerEnvironmentConfiguration::getLMM() + "/sounds/Wehrmut_Godmode.mp3").c_str(), true, false, true, irrklang::E_STREAM_MODE(0), true);
    bg_music->setVolume(0.2f);
    music_flags["bg_music_playing"] = true;

    irrklang::ISoundSource* ss1 = soundEngine->addSoundSourceFromFile((ManagerEnvironmentConfiguration::getLMM() + "/sounds/BlockSound1.mp3").c_str());
    auto ssa1 = soundEngine->addSoundSourceAlias(ss1, "BlockSound1");
    ssa1->setDefaultMinDistance(20);

    irrklang::ISoundSource* ss2 = soundEngine->addSoundSourceFromFile((ManagerEnvironmentConfiguration::getLMM() + "/sounds/BlockSound2.mp3").c_str());
    auto ssa2 = soundEngine->addSoundSourceAlias(ss2, "BlockSound2");
    ssa2->setDefaultMinDistance(20);

    std::string player_loc = ManagerEnvironmentConfiguration::getLMM() + "/models/steve.obj";

    blocks_loc.push_back(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Blatt.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Erde.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Diamanterz.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Glowstone.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Lapislazulierz.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Stein.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Obsidian.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Redstone-Erz.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Stein.3ds");
    blocks_loc.push_back(ManagerEnvironmentConfiguration::getLMM() + "/models/blocks/Wasser.3ds");

    std::string sound_loc = ManagerEnvironmentConfiguration::getSMM() + "/models/DefenseDaemon/Doppler/doppler.3ds";

    std::string wheeledCar(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
    std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
    std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");

    // SkyBox Textures readily available
    std::vector<std::string> skyBoxImageNames; // vector to store texture paths
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
    //skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg");
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
    skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg");
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
    // skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

    {
        // Create a light
        float ga = 0.1f; // Global Ambient Light level for this module
        ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
        WOLight* light = WOLight::New();
        light->isDirectionalLight(true);
        light->setPosition(Vector(0, 0, 100));
        // Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
        // for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
        light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 120.0f * Aftr::DEGtoRAD));
        light->setLabel("Light");
        worldLst->push_back(light);
    }

    {
        // Create the SkyBox
        WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
        wo->setPosition(Vector(0, 0, 0));
        wo->setLabel("Sky Box");
        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        worldLst->push_back(wo);
    }

    {
        ////Create the infinite grass plane (the floor)
        WO* wo = WO::New(grass, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
        wo->setPosition(Vector(0, 0, 0));
        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        wo->upon_async_model_loaded([wo]()
            {
                ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
                grassSkin.getMultiTextureSet().at(0).setTexRepeats(5.0f);
                grassSkin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f));  // Color of object when it is not in any light
                grassSkin.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f));  // Diffuse color components (ie, matte shading color of this object)
                grassSkin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); // Specular color component (ie, how "shiney" it is)
                grassSkin.setSpecularCoefficient(10);                       // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
            });
        wo->setLabel("Grass");
        worldLst->push_back(wo);
    }

    {
        // Create player model
        player = Block::New(player_loc, Vector(1, 1, 1), MESH_SHADING_TYPE::mstSMOOTH);
        player->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        player->setLabel("Player");
        worldLst->push_back(player);
    }
    {
        // Create other player model
        otherPlayer = Block::New(player_loc, Vector(1, 1, 1), MESH_SHADING_TYPE::mstSMOOTH);
        otherPlayer->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        otherPlayer->setLabel("OtherPlayer");
        worldLst->push_back(otherPlayer);
    }

    NetMsgBlock* blockMgr = new NetMsgBlock();
    this->netBlockMgr = blockMgr;

    placeBlock(true);

    client = NetMessengerClient::New("127.0.0.1", ManagerEnvironmentConfiguration::getVariableValue("Port"));

    {
        WO* wo = WO::New(sound_loc, Vector(1, 1, 1), MESH_SHADING_TYPE::mstSMOOTH);
        wo->setPosition(Vector(180, 180, 15));
        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        wo->setLabel("SoundEmitter");
        worldLst->push_back(wo);

        spooky_music = soundEngine->play3D((ManagerEnvironmentConfiguration::getLMM() + "/sounds/Sci-Fi_Atmos.mp3").c_str(), irrklang::vec3df(180, 180, 15), true, true, true, irrklang::E_STREAM_MODE(0), true);
        spooky_music->setVolume(0.8f);
        spooky_music->setMinDistance(20);
        auto sec = spooky_music->getSoundEffectControl();
        sec->enableEchoSoundEffect();
    }

    //{
    //   //Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
    //   WO* wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
    //   ((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
    //   wo->setPosition( Vector(0,0,0) );
    //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    //   wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );

    //   wo->setLabel( "Grass" );
    //   worldLst->push_back( wo );
    //}

    //{
    //   //Create the infinite grass plane that uses NVIDIAPhysX(the floor)
    //   WO* wo = WONVStaticPlane::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
    //   wo->setPosition( Vector( 0, 0, 0 ) );
    //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    //   wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 ).getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
    //   wo->setLabel( "Grass" );
    //   worldLst->push_back( wo );
    //}

    //{
    //   //Create the infinite grass plane (the floor)
    //   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
    //   wo->setPosition( Vector( 0, 0, 50.0f ) );
    //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    //   wo->setLabel( "Grass" );
    //   worldLst->push_back( wo );
    //}

    //{
    //   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
    //   wo->setPosition( Vector( 0, 0.5f, 75.0f ) );
    //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    //   wo->setLabel( "Grass" );
    //   worldLst->push_back( wo );
    //}

    //{
    //   WO* wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue( "sharedmultimediapath" ) + "/models/sphereRp5.wrl", Vector( 1.0f, 1.0f, 1.0f ), mstSMOOTH );
    //   wo->setPosition( 0, 0, 100.0f );
    //   wo->setLabel( "Sphere" );
    //   this->worldLst->push_back( wo );
    //}

    //{
    //   WO* wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
    //   ((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
    //   ((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
    //   wo->setPosition( Vector( 20, 20, 20 ) );
    //   wo->setLabel( "Paladin" );
    //   worldLst->push_back( wo );
    //   actorLst->push_back( wo );
    //   netLst->push_back( wo );
    //   this->setActor( wo );
    //}
    //
    //{
    //   WO* wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
    //   wo->setPosition( Vector( 20, 10, 20 ) );
    //   wo->isVisible = false; //hide the WOHuman's bounding box
    //   ((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
    //   wo->setLabel( "Human Cyborg" );
    //   worldLst->push_back( wo );
    //   actorLst->push_back( wo ); //Push the WOHuman as an actor
    //   netLst->push_back( wo );
    //   this->setActor( wo ); //Start module where human is the actor
    //}

    //{

    //   //Create and insert the WOWheeledVehicle
    //   std::vector< std::string > wheels;
    //   std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
    //   wheels.push_back( wheelStr );
    //   wheels.push_back( wheelStr );
    //   wheels.push_back( wheelStr );
    //   wheels.push_back( wheelStr );
    //   WO* wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
    //   wo->setPosition( Vector( 5, -15, 20 ) );
    //   wo->setLabel( "Car 1970s Beater" );
    //   ((WOODE*)wo)->mass = 200;
    //   worldLst->push_back( wo );
    //   actorLst->push_back( wo );
    //   this->setActor( wo );
    //   netLst->push_back( wo );
    //}

    // Make a Dear Im Gui instance via the WOImGui in the engine... This calls
    // the default Dear ImGui demo that shows all the features... To create your own,
    // inherit from WOImGui and override WOImGui::drawImGui_for_this_frame(...) (among any others you need).
    WOImGui* gui = WOImGui::New(nullptr);

    gui->setLabel("My Gui");
    gui->subscribe_drawImGuiWidget(
        [this, gui]() // this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
        {
            Instructions::getInstructions();

            ImVec4 color_blue = ImVec4{ 0.000f, 0.703f, 0.917f, 1 };
            ImVec4 color_orange = ImVec4{ 0.901f, 0.494f, 0.133f, 1 };

            ImGui::Begin("Sound Settings");
            ImGui::Separator();
            ImGui::TextColored(color_orange, "Spooky Music");
            ImGui::SameLine();
            ImGui::Checkbox("##spooky_music", &music_flags["spooky_music_playing"]);
            ImGui::Separator();
            ImGui::TextColored(color_orange, "Background Music");
            ImGui::SameLine();
            ImGui::Checkbox("##bg_music_playing", &music_flags["bg_music_playing"]);
            ImGui::SliderFloat("##bg_music_vol", &bg_music_vol, 0, 1);
            ImGui::Separator();
            ImGui::TextColored(color_blue, "Background Sound Effects");

            ImGui::Checkbox("Echo Effect", &music_flags["bg_echo"]);
            ImGui::Checkbox("Waves Reverb Effect", &music_flags["bg_waves"]);
            ImGui::Checkbox("Distort Effect", &music_flags["bg_distort"]);
            ImGui::Checkbox("Gargle Effect", &music_flags["bg_gargle"]);

            ImGui::End();

            ImGui::Begin("Controls");
            ImGui::Separator();

            ImGui::TextColored(color_orange, "Port:");
            ImGui::SameLine();
            ImGui::TextColored(color_blue, ManagerEnvironmentConfiguration::getVariableValue("Port").c_str());
            ImGui::TextColored(color_orange, "TCP Connection:");
            ImGui::SameLine();
            bool tsock = false;
            if (client)
                tsock = client->isTCPSocketOpen();
            ImGui::TextColored(color_blue, tsock ? "true" : "false");

            ImGui::TextColored(color_blue, "Center on camera");
            ImGui::SameLine();
            ImGui::Checkbox("##centercamera", &center_on_camera);
            ImGui::Separator();

            if (center_on_camera)
                ImGui::BeginDisabled();
            ImGui::TextColored(color_blue, "Translation");
            ImGui::TextColored(color_orange, "X Axis");
            ImGui::TextColored(color_orange, "-100");
            ImGui::SameLine();
            ImGui::SliderFloat("##tx", &prj_block->getPos()->x, -100, 100);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "100");
            ImGui::TextColored(color_orange, "Y Axis");
            ImGui::TextColored(color_orange, "-100");
            ImGui::SameLine();
            ImGui::SliderFloat("##ty", &prj_block->getPos()->y, -100, 100);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "100");
            ImGui::TextColored(color_orange, "Z Axis");
            ImGui::TextColored(color_orange, "   0");
            ImGui::SameLine();
            ImGui::SliderFloat("##tz", &prj_block->getPos()->z, 0, 100);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "100");
            ImGui::Separator();

            ImGui::TextColored(color_blue, "Relative Rotation");
            ImGui::TextColored(color_orange, "Relative X Axis");
            ImGui::TextColored(color_orange, "-180");
            ImGui::SameLine();
            ImGui::SliderFloat("##rx", &prj_block->getRelativeRotation()->x, -180, 180);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "180");
            ImGui::TextColored(color_orange, "Relative Y Axis");
            ImGui::TextColored(color_orange, "-180");
            ImGui::SameLine();
            ImGui::SliderFloat("##ry", &prj_block->getRelativeRotation()->y, -180, 180);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "180");
            ImGui::TextColored(color_orange, "Relative Z Axis");
            ImGui::TextColored(color_orange, "-180");
            ImGui::SameLine();
            ImGui::SliderFloat("##rz", &prj_block->getRelativeRotation()->z, -180, 180);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "180");
            ImGui::Separator();


            ImGui::TextColored(color_blue, "Global Rotation");
            ImGui::TextColored(color_orange, "Global X Axis");
            ImGui::TextColored(color_orange, "-180");
            ImGui::SameLine();
            ImGui::SliderFloat("##gx", &prj_block->getGlobalRotation()->x, -180, 180);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "180");
            ImGui::TextColored(color_orange, "Global Y Axis");
            ImGui::TextColored(color_orange, "-180");
            ImGui::SameLine();
            ImGui::SliderFloat("##gy", &prj_block->getGlobalRotation()->y, -180, 180);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "180");
            ImGui::TextColored(color_orange, "Global Z Axis");
            ImGui::TextColored(color_orange, "-180");
            ImGui::SameLine();
            ImGui::SliderFloat("##gz", &prj_block->getGlobalRotation()->z, -180, 180);
            ImGui::SameLine();
            ImGui::TextColored(color_orange, "180");
            ImGui::Separator();
            if (center_on_camera)
                ImGui::EndDisabled();

            ImVec2 button_size = ImVec2{ 150, 0 };
            float width = ImGui::GetWindowSize().x;
            float centre_pos = (width - button_size.x) / 2;
            ImGui::SetCursorPosX(centre_pos);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.752f, 0.223f, 0.168f, 1 });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.905f, 0.298f, 0.235f, 1 });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.905f, 0.298f, 0.235f, 1 });
            if (ImGui::Button("Place Block!", button_size)) {
                placeBlock(false);
                sendNetMessage("placeBlock");
            }
            ImGui::PopStyleColor(3);

            ImGui::End();
            // ImGui::ShowDemoWindow(); //Displays the default ImGui demo from C:/repos/aburn/engine/src/imgui_implot/implot_demo.cpp
            // WOImGui::draw_AftrImGui_Demo( gui ); //Displays a small Aftr Demo from C:/repos/aburn/engine/src/aftr/WOImGui.cpp
            // ImPlot::ShowDemoWindow(); //Displays the ImPlot demo using ImGui from C:/repos/aburn/engine/src/imgui_implot/implot_demo.cpp
        });
    this->worldLst->push_back(gui);

    //createBlockyWorldWayPoints();
}

void GLViewBlockyWorld::updateControls()
{
    if (active_keys[SDLK_w])
    {
        auto lookDirection = this->getCamera()->getLookDirection();
        lookDirection.z = 0;
        this->getCamera()->moveRelative(lookDirection * this->getCamera()->getCameraVelocity());

    }

    if (active_keys[SDLK_s])
    {
        auto lookDirection = this->getCamera()->getLookDirection();
        lookDirection.z = 0;
        this->getCamera()->moveRelative(lookDirection * -1 * this->getCamera()->getCameraVelocity());
    }

    if (active_keys[SDLK_d])
    {
        auto lookDirection = this->getCamera()->getLookDirection();
        auto normalDirection = this->getCamera()->getNormalDirection();

        this->getCamera()->moveRelative(lookDirection.crossProduct(normalDirection) * this->getCamera()->getCameraVelocity());
    }

    if (active_keys[SDLK_a])
    {
        auto lookDirection = this->getCamera()->getLookDirection();
        auto normalDirection = this->getCamera()->getNormalDirection();

        this->getCamera()->moveRelative(lookDirection.crossProduct(normalDirection) * -1 * this->getCamera()->getCameraVelocity());
    }

    if (active_keys[SDLK_LSHIFT])
    {
        auto lookDirection = this->getCamera()->getLookDirection();
        auto normalDirection = this->getCamera()->getLookDirection();

        this->getCamera()->moveRelative(Vector(0, 0, 1) * this->getCamera()->getCameraVelocity());
    }

    if (active_keys[SDLK_LCTRL])
    {
        auto lookDirection = this->getCamera()->getLookDirection();
        auto normalDirection = this->getCamera()->getLookDirection();

        this->getCamera()->moveRelative(Vector(0, 0, -1) * this->getCamera()->getCameraVelocity());
    }
}

void GLViewBlockyWorld::updateProjection()
{
    if (camera_mode == "close") {
        auto lookDirection = this->getCamera()->getLookDirection().normalizeMe();
        auto pos = (lookDirection * 15) + this->getCamera()->getPosition();
        pos.z -= 5;
        if (pos.z < 4)
            pos.z = 4;
        player->setPos(pos);

        auto rotateByInZ = lookDirection.x * 90 - 90;
        if (lookDirection.y > 0) rotateByInZ *= -1;
        player->getRelativeRotation()->z = rotateByInZ;
    }

    if (prj_block && center_on_camera) {
        auto lookDirection = this->getCamera()->getLookDirection().normalizeMe();
        auto pos = (lookDirection * 20) + this->getCamera()->getPosition();
        pos.z -= 4;
        if (pos.z < 2)
            pos.z = 2;

        prj_block->setPos(pos);
    }
}

void GLViewBlockyWorld::updateMusicSettings()
{
    if (bg_music) {
        bg_music->setVolume(bg_music_vol);

        if (music_flags["bg_music_playing"] && bg_music->getIsPaused()) {
            bg_music->setIsPaused(false);
        }
        else if (!music_flags["bg_music_playing"] && !bg_music->getIsPaused()) {
            bg_music->setIsPaused(true);
        }

        auto control = bg_music->getSoundEffectControl();
        if (music_flags["bg_echo"] && !control->isEchoSoundEffectEnabled()) {
            control->enableEchoSoundEffect();
        }
        else if (!music_flags["bg_echo"] && control->isEchoSoundEffectEnabled()) {
            control->disableEchoSoundEffect();
        }
        if (music_flags["bg_waves"] && !control->isWavesReverbSoundEffectEnabled()) {
            control->enableWavesReverbSoundEffect();
        }
        else if (!music_flags["bg_waves"] && control->isWavesReverbSoundEffectEnabled()) {
            control->disableWavesReverbSoundEffect();
        }
        if (music_flags["bg_distort"] && !control->isDistortionSoundEffectEnabled()) {
            control->enableDistortionSoundEffect();
        }
        else if (!music_flags["bg_distort"] && control->isDistortionSoundEffectEnabled()) {
            control->disableDistortionSoundEffect();
        }
        if (music_flags["bg_gargle"] && !control->isGargleSoundEffectEnabled()) {
            control->enableGargleSoundEffect();
        }
        else if (!music_flags["bg_gargle"] && control->isGargleSoundEffectEnabled()) {
            control->disableGargleSoundEffect();
        }
    }

    if (music_flags["spooky_music_playing"] && spooky_music->getIsPaused()) {
        spooky_music->setIsPaused(false);
    }
    else if (!music_flags["spooky_music_playing"] && !spooky_music->getIsPaused()) {
        spooky_music->setIsPaused(true);
    }

    auto position = this->getCamera()->getPosition();
    auto lookDir = this->getCamera()->getLookDirection();
    auto normalDir = this->getCamera()->getNormalDirection() * Vector(1.0, 1.0, -1.0);

    this->soundEngine->setListenerPosition(
        irrklang::vec3df(position.x, position.y, position.z),
        irrklang::vec3df(lookDir.x, lookDir.y, lookDir.z),
        irrklang::vec3df(abs(position.x - prev_pos.x) * 10, abs(position.y - prev_pos.y) * 10, abs(position.z - prev_pos.z) * 10),
        irrklang::vec3df(normalDir.x, normalDir.y, normalDir.z));

    prev_pos = position;
}

void GLViewBlockyWorld::placeBlock(bool proj, std::optional<int> index, std::optional<Vector> pos, std::optional<Mat4> dm) {
    if (proj) {
        prj_block = Block::New(blocks_loc[0], Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
        prj_block->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
        prj_block->setLabel("Cube Projection");

        auto tex = ManagerTex::loadTexAsync(ManagerEnvironmentConfiguration::getLMM() + "/models/cube_transparent.png");
        prj_block->upon_async_model_loaded([this, tex]()
            {
                auto skin = ModelMeshSkin(*tex);
                skin.setMeshShadingType(MESH_SHADING_TYPE::mstFLAT);
                prj_block->getModel()->addSkin(std::move(skin));
                prj_block->getModel()->useNextSkin();

            });

        worldLst->push_back(prj_block);

    }
    else {
        int indexToUse;
        Vector posToUse;
        Mat4 dmToUse;
        if (index)
            indexToUse = index.value();
        else
            indexToUse = active_block_index;

        if (pos)
            posToUse = pos.value();
        else
            posToUse = prj_block->getPosition();

        if (dm)
            dmToUse = dm.value();
        else
            dmToUse = prj_block->getDisplayMatrix();

        WO* wo = WO::New(blocks_loc[indexToUse], indexToUse == 0 ? Vector(1, 1, 1) : Vector(4, 4, 4), MESH_SHADING_TYPE::mstFLAT);

        wo->setPosition(posToUse);
        wo->setDisplayMatrix(dmToUse);

        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        wo->setLabel("Cube");
        blocks.push_back(wo);
        worldLst->push_back(wo);

        soundEngine->play3D(rand() % 2 ? "BlockSound1" : "BlockSound2", irrklang::vec3df(posToUse.x, posToUse.y, posToUse.z));
    }
}

void GLViewBlockyWorld::sendNetMessage(std::string action) {
    if (client && netBlockMgr && client->isTCPSocketOpen()) {
        netBlockMgr->action = action;

        if (action == "syncPlayer") {
            netBlockMgr->position = player->getPosition();
            netBlockMgr->displayMat = player->getDisplayMatrix();
        }
        else if (action == "placeBlock") {
            netBlockMgr->block_type = active_block_index;
            netBlockMgr->position = prj_block->getPosition();
            netBlockMgr->displayMat = prj_block->getDisplayMatrix();
        }

        client->sendNetMsgSynchronousUDP(*netBlockMgr);
    }
}

void GLViewBlockyWorld::createBlockyWorldWayPoints()
{
    // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
    WayPointParametersBase params(this);
    params.frequency = 5000;
    params.useCamera = true;
    params.visible = true;
    WOWayPointSpherical* wayPt = WOWayPointSpherical::New(params, 3);
    wayPt->setPosition(Vector(50, 0, 3));
    worldLst->push_back(wayPt);
}
