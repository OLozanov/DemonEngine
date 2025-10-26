#pragma once

#include "Forms.h"

class Editor;

class CylinderDlgImpl : public CylinderDlg
{
public:
    CylinderDlgImpl(wxWindow* parent, Editor& editor);

private:
    void onOkBtn(wxCommandEvent& event) override;
    void onCancelBtn(wxCommandEvent& event) override;

    Editor& m_editor;
};