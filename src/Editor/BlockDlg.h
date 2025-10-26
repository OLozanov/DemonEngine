#pragma once

#include "Forms.h"

#include "Utils/EventHandler.h"

class Editor;

class BlockDlgImpl : public BlockDlg
{
public:
    BlockDlgImpl(wxWindow* parent, Editor& editor);

    void update();

    Event<void()> onBlockUpdate;

private:
    void onChangeType(wxCommandEvent& event) override;
    void onOrderDown(wxSpinEvent& event) override;
    void onOrderUp(wxSpinEvent& event) override;

    Editor& m_editor;
};
