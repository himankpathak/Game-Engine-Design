#include "NetMsgBlock.h"
#include "GLViewBlockyWorld.h"

using namespace Aftr;

NetMsgMacroDefinition(NetMsgBlock);

NetMsgBlock::NetMsgBlock()
{
}

NetMsgBlock::~NetMsgBlock()
{
}

bool NetMsgBlock::toStream(NetMessengerStreamBuffer& os) const
{
    os << action;

    if (action == "syncPlayer") {
        for (int i = 0; i < 16; i++) {
            os << pose[i];
        }
    }
    else if (action == "placeBlock") {
        os << block_type;

        for (int i = 0; i < 16; i++) {
            os << pose[i];
        }
    }
    else if (action == "syncGravity") {
        os << gravity.x;
        os << gravity.y;
        os << gravity.z;
    }

    return true;
}

bool NetMsgBlock::fromStream(NetMessengerStreamBuffer& is)
{
    is >> action;

    if (action == "syncPlayer") {
        for (int i = 0; i < 16; i++) {
            is >> pose[i];
        }
    }
    else if (action == "placeBlock") {
        is >> block_type;

        for (int i = 0; i < 16; i++) {
            is >> pose[i];
        }
    }
    else if (action == "syncGravity") {
        is >> gravity.x;
        is >> gravity.y;
        is >> gravity.z;
    }

    return true;
}

void NetMsgBlock::onMessageArrived()
{
    auto glView = (GLViewBlockyWorld*)ManagerGLView::getGLView();

    if (action == "syncPlayer") {
        auto otherPos = glView->otherPlayer;

        if (otherPos) {
            otherPos->setBlockPose(pose);
        }
    }
    else if (action == "placeBlock") {
        glView->placeBlock(false, block_type, pose);
    }
    else if (action == "syncGravity") {
        glView->updateGravity(gravity);
    }


}

std::string NetMsgBlock::toString() const
{
    std::stringstream ss;

    ss << NetMsg::toString();
    return ss.str();
}
