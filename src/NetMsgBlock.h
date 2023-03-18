#pragma once

#include "NetMsg.h"
#include "Block.h"
#include "sstream"

namespace Aftr {
    class NetMsgBlock : public NetMsg
    {
    public:
        NetMsgMacroDeclaration(NetMsgBlock);

        NetMsgBlock();
        virtual ~NetMsgBlock();
        virtual bool toStream(NetMessengerStreamBuffer& os) const;
        virtual bool fromStream(NetMessengerStreamBuffer& is);
        virtual void onMessageArrived();
        virtual std::string toString() const;

        std::string action;
        int block_type;
        Mat4 pose;
        physx::PxVec3 gravity;
    };
}
