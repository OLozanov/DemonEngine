#pragma once

#include <wx/window.h>

#include "Render/Render.h"

#include "Utils/EventHandler.h"

#include "Editor.h"

enum EditAxis
{
    XAxis = 0,
    YAxis = 1,
    ZAxis = 2
};

class View : public wxWindow
{
public:
    View(wxWindow* parent, Editor& editor, bool depthBuffer = false)
    : wxWindow(parent, wxID_ANY, {0, 0}, {500, 300}, wxBORDER_DOUBLE)
    , m_editor(editor)
    , m_swapChain(Render::GpuInstance::GetInstance().createSwapChain(GetHandle()), depthBuffer)
    {
    }

    Event<void()> onEditFinish;

protected:

    Editor& m_editor;
    Render::SwapChain m_swapChain;

    int m_width;
    int m_height;
};