#pragma once

#include "WOImGuiAbstract.h"
#include "AftrImGui_Markdown_Renderer.h"
#include <WOImGui.h>


class Instructions : public Aftr::WOImGuiAbstract
{
public:
    virtual ~Instructions();

    static void getInstructions();
};
