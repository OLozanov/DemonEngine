#pragma once

#include "Forms.h"
#include "Editor.h"

class MapInfoDlgImpl : public MapInfoDlg
{
public:
    MapInfoDlgImpl(wxWindow* parent, Editor& editor);

private:
    void onShow(wxShowEvent& event) override;
    void onChanged(wxPropertyGridEvent& event) override;

private:
    Editor& m_editor;

    wxStringProperty* m_skyProp;
    wxBoolProperty* m_globalLightEnableProp;
    wxFloatProperty* m_xdirProp;
    wxFloatProperty* m_ydirProp;
    wxFloatProperty* m_zdirProp;
    wxColourProperty* m_dirLightColorProp;
    wxBoolProperty* m_giEnableProp;
    wxColourProperty* m_giColorProp;
};