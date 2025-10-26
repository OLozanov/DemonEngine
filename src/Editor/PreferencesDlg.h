#pragma once

#include "Forms.h"

class Editor;
class PerspectiveView;

class PreferencesDlgImpl : public PreferencesDlg
{
public:
    PreferencesDlgImpl(wxWindow* parent, Editor& editor, PerspectiveView* mainView);

private:
    void onCancel(wxCommandEvent& event) override;
    void onOk(wxCommandEvent& event) override;

private:
    Editor& m_editor;
    PerspectiveView* m_mainView;
};