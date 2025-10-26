#pragma once

#include "Forms.h"
#include "PreferencesDlg.h"
#include "MapInfoDlg.h"
#include "EditConsole.h"
#include "PolyDlg.h"
#include "BlockDlg.h"
#include "ObjectDlg.h"
#include "SurfaceDlg.h"
#include "SurfaceEditDlg.h"
#include "SurfaceCpDlg.h"

#include "PlaneDlg.h"
#include "BoxDlg.h"
#include "CylinderDlg.h"
#include "ConeDlg.h"
#include "SphereDlg.h"

#include "View/PerspectiveView.h"
#include "View/OrthoView.h"

#include <wx/panel.h>
#include <wx/filedlg.h>

#include "Editor.h"

class MainFrameImpl : public MainFrame
{
public:

    MainFrameImpl(wxWindow* parent);

private:
    void onMenuNew(wxCommandEvent& event) override;
    void onMenuOpen(wxCommandEvent& event) override;
    void onMenuSave(wxCommandEvent& event) override;
    void onMenuSaveAs(wxCommandEvent& event) override;
    void onMenuPreferences(wxCommandEvent& event) override;
    void onMenuQuit(wxCommandEvent& event) override;

    void onMenuPortals(wxCommandEvent& event) override;

    void onMenuPlane(wxCommandEvent& event) override;
    void onMenuBox(wxCommandEvent& event) override;
    void onMenuCylinder(wxCommandEvent& event) override;
    void onMenuCone(wxCommandEvent& event) override;
    void onMenuSphere(wxCommandEvent& event) override;

    void onMenuSurface(wxCommandEvent& event) override;
    void onMenuCp(wxCommandEvent& event) override;

    void onMenuMapInfo(wxCommandEvent& event) override;
    void onMenuBuildMap(wxCommandEvent& event) override;
    void onMenuRebuildCsg(wxCommandEvent& event) override;
    void onMenuRunMap(wxCommandEvent& event) override;
    
    void onDuplicate(wxCommandEvent& event) override;
    void onAddBlock(wxCommandEvent& event) override;
    void onSubBlock(wxCommandEvent& event) override;
    void onSolidBlock(wxCommandEvent& event) override;

    void onMoveMode(wxCommandEvent& event) override;
    void onRotateMode(wxCommandEvent& event) override;
    void onScaleMode(wxCommandEvent& event) override;

    void onVertType(wxCommandEvent& event) override;
    void onBlockType(wxCommandEvent& event) override;
    void onObjectType(wxCommandEvent& event) override;
    void onPolyType(wxCommandEvent& event) override;
    void onDecalType(wxCommandEvent& event) override;
    void onDisplaceType(wxCommandEvent& event) override;
    void onSurfaceType(wxCommandEvent& event) override;
    void onCpType(wxCommandEvent& event) override;

    void onCreateSurface(wxCommandEvent& event) override;

    void onResize(wxSizeEvent& event);
    
    void onKeyUp(wxKeyEvent& event);
    void refreshViews();

    EditorInfo getEditorInfo();
    void setEditorParams(const EditorInfo& editorInfo);

private:
    PerspectiveView* m_mainView;
    OrthoView* m_topView;
    OrthoView* m_sideView;
    OrthoView* m_frontView;

    PreferencesDlgImpl* m_preferencesDlg;
    MapInfoDlgImpl* m_mapInfoDlg;
    ConsoleImpl* m_console;
    PolyDlgImpl* m_polyDlg;
    BlockDlgImpl* m_blockDlg;
    ObjectDlgImpl* m_objectDlg;
    SurfaceDlgImpl* m_surfaceDlg;
    SurfaceEditDlgImpl* m_surfaceEditDlg;
    SurfaceCpDlgImpl* m_surfaceCpDlg;

    PlaneDlgImpl* m_planeDlg;
    BoxDlgImpl* m_boxDlg;
    CylinderDlgImpl* m_cylinderDlg;
    ConeDlgImpl* m_coneDlg;
    SphereDlgImpl* m_sphereDlg;

    wxFileDialog* m_saveDlg;
    wxFileDialog* m_openDlg;

    Editor m_editor;
    
    std::string m_currentFile;
};