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
        os << position.x;
        os << position.y;
        os << position.z;

        for (int i = 0; i < 16; i++) {
            os << displayMat[i];
        }
    }
    else if (action == "placeBlock") {
        os << block_type;

        os << position.x;
        os << position.y;
        os << position.z;

        for (int i = 0; i < 16; i++) {
            os << displayMat[i];
        }
    }

    return true;
}

bool NetMsgBlock::fromStream(NetMessengerStreamBuffer& is)
{
    is >> action;

    if (action == "syncPlayer") {
        is >> position.x;
        is >> position.y;
        is >> position.z;

        for (int i = 0; i < 16; i++) {
            is >> displayMat[i];
        }
    }
    else if (action == "placeBlock") {
        is >> block_type;

        is >> position.x;
        is >> position.y;
        is >> position.z;

        for (int i = 0; i < 16; i++) {
            is >> displayMat[i];
        }
    }

    return true;
}

void NetMsgBlock::onMessageArrived()
{
    auto glView = (GLViewBlockyWorld*)ManagerGLView::getGLView();

    if (action == "syncPlayer") {
        auto otherPos = glView->otherPlayer;

        if (otherPos) {
            otherPos->setPos(position);
            otherPos->setDisplayMatrix(displayMat);
        }
    }
    else if (action == "placeBlock") {
        glView->placeBlock(false, block_type, position, displayMat);
    }


}

std::string NetMsgBlock::toString() const
{
    std::stringstream ss;

    ss << NetMsg::toString();
    return ss.str();
}
