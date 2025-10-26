#pragma once

#include "Forms.h"

class Editor;

class DetailDlgImpl : public DetailDlg
{
public:
    DetailDlgImpl(wxWindow* parent, Editor& editor);

private:
    void onAddBtn(wxCommandEvent& event) override;
    void onAddDetailBtn(wxCommandEvent& event) override;
    void onRemoveDetailDlg(wxCommandEvent& event) override;

private:

    Editor& m_editor;
};
