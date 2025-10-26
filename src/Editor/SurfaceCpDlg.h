#pragma once

#include "Forms.h"

class Editor;

class SurfaceCpDlgImpl : public SurfaceCpDlg
{
public:
    SurfaceCpDlgImpl(wxWindow* parent, Editor& editor);

    void update();

private:
    void onSetWeight(wxCommandEvent& event) override;
    void onClose(wxCommandEvent& event) override;

    Editor& m_editor;
};