#include "Instructions.h"

Instructions::~Instructions()
{
}

void Instructions::getInstructions()
{
    static Aftr::AftrImGui_Markdown_Renderer md_render = Aftr::make_default_MarkdownRenderer();

    ImGui::Begin("Markdown");
    Aftr::AftrImGui_Markdown_Doc doc{ std::filesystem::path{ "../Instructions.md" } };
    md_render.draw_markdown(doc);
    ImGui::End();
}
