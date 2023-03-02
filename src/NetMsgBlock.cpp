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
    auto glView = (GLViewBlockyWorld*)ManagerGLView::getGLView();

    Vector pos = glView->player->getPosition();
    Mat4 disp = glView->player->getDisplayMatrix();

    os << pos.x;
    os << pos.y;
    os << pos.z;

    for (int i = 0; i < 16; i++) {
        os << disp[i];
    }

    return true;
}

bool NetMsgBlock::fromStream(NetMessengerStreamBuffer& is)
{

    is >> position.x;
    is >> position.y;
    is >> position.z;

    for (int i = 0; i < 16; i++) {
        is >> displayMat[i];
    }

    return true;
}

void NetMsgBlock::onMessageArrived()
{
    auto glView = (GLViewBlockyWorld*)ManagerGLView::getGLView();
    auto otherPos = glView->otherPlayer;

    if (otherPos) {
        otherPos->setPos(position);
        otherPos->setDisplayMatrix(displayMat);
    }
}

std::string NetMsgBlock::toString() const
{
    std::stringstream ss;

    ss << NetMsg::toString();
    return ss.str();
}
