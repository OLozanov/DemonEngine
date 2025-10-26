#pragma once

#include "Forms.h"
#include "Editor.h"

class SurfaceDlgImpl : public SurfaceDlg
{
public:
    SurfaceDlgImpl(wxWindow* parent, Editor& editor);

private:

    enum SurfaceCreateType
    {
        surf_bezier = 0,
        surf_bspline = 1
    };

    void onCreate(wxCommandEvent& event) override;
    void onCancel(wxCommandEvent& event) override;
    void onTypeChange(wxCommandEvent& event) override;

private:
    Editor& m_editor;
};