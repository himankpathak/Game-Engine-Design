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
        virtual bool toStream(NetMessengerStreamBuffer& ostream) const;
        virtual bool fromStream(NetMessengerStreamBuffer& istream);
        virtual void onMessageArrived();
        virtual std::string toString() const;

    protected:
        Vector position;
        Mat4 displayMat;
    };
}
