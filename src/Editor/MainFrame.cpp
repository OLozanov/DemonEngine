#include <wx/msgdlg.h>

#include "MainFrame.h"

MainFrameImpl::MainFrameImpl(wxWindow* parent)
: MainFrame(parent)
, m_mainView(new PerspectiveView(this, m_editor))
, m_topView(new OrthoView(this, m_editor, ViewType::TopView))
, m_sideView(new OrthoView(this, m_editor, ViewType::SideView))
, m_frontView(new OrthoView(this, m_editor, ViewType::FrontView))
, m_preferencesDlg(new PreferencesDlgImpl(this, m_editor, m_mainView))
, m_mapInfoDlg(new MapInfoDlgImpl(this, m_editor))
, m_console(new ConsoleImpl(this, m_editor, m_mainView->camera()))
, m_polyDlg(new PolyDlgImpl(this, m_editor))
, m_blockDlg(new BlockDlgImpl(this, m_editor))
, m_objectDlg(new ObjectDlgImpl(this, m_editor))
, m_surfaceDlg(new SurfaceDlgImpl(this, m_editor))
, m_surfaceEditDlg(new SurfaceEditDlgImpl(this, m_editor))
, m_surfaceCpDlg(new SurfaceCpDlgImpl(this, m_editor))
, m_boxDlg(new BoxDlgImpl(this, m_editor))
, m_planeDlg(new PlaneDlgImpl(this, m_editor))
, m_cylinderDlg(new CylinderDlgImpl(this, m_editor))
, m_coneDlg(new ConeDlgImpl(this, m_editor))
, m_sphereDlg(new SphereDlgImpl(this, m_editor))
, m_saveDlg(new wxFileDialog(this, _("Save map"), _(".\\Maps"), wxEmptyString, _("DC map (*.dcm)|*.dcm"), wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog")))
, m_openDlg(new wxFileDialog(this, _("Select file"), _(".\\Maps"), wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_OPEN, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog")))
, m_editor(*m_console)
{
    Connect(wxEVT_SIZE, (wxObjectEventFunction)&MainFrameImpl::onResize);
    Connect(wxEVT_KEY_UP, (wxObjectEventFunction)&MainFrameImpl::onKeyUp);

    m_toolBar->ToggleTool(ID_BLOCKS, true);

    m_editor.onUpdate.bind(this, &MainFrameImpl::refreshViews);
    
    m_editor.onPolygonSelectionUpdate.bind([this](){
        m_polyDlg->update();
    });

    m_editor.onObjectSelectionUpdate.bind([this]() {
        m_objectDlg->update();
    });

    m_editor.onSurfaceSelectionUpdate.bind([this]() {
        m_surfaceEditDlg->update();
    });

    m_blockDlg->onBlockUpdate.bind(this, &MainFrameImpl::refreshViews);
    m_objectDlg->onPropertyUpdate.bind(this, &MainFrameImpl::refreshViews);
    m_surfaceEditDlg->onSurfaceUpdate.bind(this, &MainFrameImpl::refreshViews);

    m_mainView->onEditFinish.bind(this, &MainFrameImpl::refreshViews);
    m_topView->onEditFinish.bind(this, &MainFrameImpl::refreshViews);
    m_sideView->onEditFinish.bind(this, &MainFrameImpl::refreshViews);
    m_frontView->onEditFinish.bind(this, &MainFrameImpl::refreshViews);

    m_editor.loadMaterial("blocks01");
    m_editor.loadMaterial("tile");
    m_editor.loadMaterial("grass01");
}

void MainFrameImpl::onMenuNew(wxCommandEvent& event)
{
    m_currentFile.clear();
    m_fileMenu->Enable(wxID_SAVE, false);
    m_mapMenu->Enable(ID_RUN, false);

    m_editor.cleanup();
    m_mainView->reset();
    m_topView->reset();
    m_sideView->reset();
    m_frontView->reset();

    refreshViews();
}

void MainFrameImpl::onMenuOpen(wxCommandEvent& event)
{
    if (m_openDlg->ShowModal() != wxID_CANCEL)
    {
        wxString path = m_openDlg->GetPath();
        std::string filename = path.ToStdString();

        m_editor.cleanup();

        try
        {
            EditorInfo editorInfo;

            m_editor.read(filename, editorInfo);
            m_currentFile = filename;
            m_fileMenu->Enable(wxID_SAVE, true);
            m_mapMenu->Enable(ID_RUN, true);

            setEditorParams(editorInfo);
        }
        catch (std::exception exception)
        {
            wxMessageBox(exception.what(), "Map loading failure");
        }
        catch (...)
        {
            wxMessageBox("Unknown error", "Map loading failure");
        }
    }
}

void MainFrameImpl::onMenuSave(wxCommandEvent& event)
{
    if (!m_currentFile.empty())
    {
        EditorInfo editorInfo = getEditorInfo();
        m_editor.write(m_currentFile, editorInfo);
    }
}

void MainFrameImpl::onMenuSaveAs(wxCommandEvent& event)
{
    if (m_saveDlg->ShowModal() != wxID_CANCEL)
    {
        wxString path = m_saveDlg->GetPath();       
        m_currentFile = path.ToStdString();

        EditorInfo editorInfo = getEditorInfo();

        m_editor.write(m_currentFile, editorInfo);

        m_fileMenu->Enable(wxID_SAVE, true);
        m_mapMenu->Enable(ID_RUN, true);
    }
}

void MainFrameImpl::onMenuPreferences(wxCommandEvent& event)
{
    m_preferencesDlg->Show();
}

void MainFrameImpl::onMenuQuit(wxCommandEvent& event)
{ 
    Close(); 
}

void MainFrameImpl::onMenuPortals(wxCommandEvent& event)
{
    m_editor.showPortals(m_viewMenu->IsChecked(ID_PRT));
    m_mainView->Refresh();
}

void MainFrameImpl::onMenuPlane(wxCommandEvent& event)
{
    m_planeDlg->ShowModal();
}

void MainFrameImpl::onMenuBox(wxCommandEvent& event)
{
    m_boxDlg->ShowModal();
}

void MainFrameImpl::onMenuCylinder(wxCommandEvent& event)
{
    m_cylinderDlg->ShowModal();
}

void MainFrameImpl::onMenuCone(wxCommandEvent& event)
{
    m_coneDlg->ShowModal();
}

void MainFrameImpl::onMenuSphere(wxCommandEvent& event)
{
    m_sphereDlg->ShowModal();
}

void MainFrameImpl::onMenuSurface(wxCommandEvent& event)
{
    m_surfaceDlg->ShowModal();
}

void MainFrameImpl::onMenuCp(wxCommandEvent& event)
{
    m_surfaceCpDlg->update();
    m_surfaceCpDlg->ShowModal();
}

void MainFrameImpl::onMenuMapInfo(wxCommandEvent& event)
{
    m_mapInfoDlg->ShowModal();
}

void MainFrameImpl::onMenuBuildMap(wxCommandEvent& event)
{
    m_editor.buildMap();
}

void MainFrameImpl::onMenuRebuildCsg(wxCommandEvent& event)
{
    m_editor.rebuildCsg();
}

void MainFrameImpl::onMenuRunMap(wxCommandEvent& event)
{
    if (m_currentFile.empty()) return;

    size_t startpos = m_currentFile.find_last_of("/\\");
    size_t endpos = m_currentFile.find_last_of('.');

    startpos = (startpos == m_currentFile.npos) ? 0 : startpos + 1;
    endpos = (endpos == m_currentFile.npos) ? m_currentFile.size() : endpos;

    size_t len = endpos - startpos;

    std::string map = m_currentFile.substr(startpos, len);

    std::string executable = "DemonEngine2";
    std::string command = executable + " -m " + map;

    std::system(command.c_str());
}

void MainFrameImpl::onDuplicate(wxCommandEvent& event)
{
    m_editor.duplicateSelected();
}

void MainFrameImpl::onAddBlock(wxCommandEvent& event) 
{ 
    m_editor.addBlock(BlockType::Add); refreshViews(); 
}

void MainFrameImpl::onSubBlock(wxCommandEvent& event) 
{ 
    m_editor.addBlock(BlockType::Subtruct); refreshViews(); 
}

void MainFrameImpl::onSolidBlock(wxCommandEvent& event)
{ 
    m_editor.addBlock(BlockType::Solid); refreshViews(); 
}

void MainFrameImpl::onMoveMode(wxCommandEvent& event) 
{ 
    m_editor.setEditMode(EditMode::Move); 
}

void MainFrameImpl::onRotateMode(wxCommandEvent& event) 
{ 
    m_editor.setEditMode(EditMode::Rotate); 
}

void MainFrameImpl::onScaleMode(wxCommandEvent& event) 
{ 
    m_editor.setEditMode(EditMode::Scale); 
}

void MainFrameImpl::onVertType(wxCommandEvent& event)
{
    m_editor.setEditType(EditType::Vertices);
}

void MainFrameImpl::onBlockType(wxCommandEvent& event)
{
    m_editor.setEditType(EditType::Blocks);
}

void MainFrameImpl::onObjectType(wxCommandEvent& event)
{
    m_editor.setEditType(EditType::Objects);
}

void MainFrameImpl::onPolyType(wxCommandEvent& event)
{
    m_editor.setEditType(EditType::Polygons);
}

void MainFrameImpl::onDecalType(wxCommandEvent& event)
{
    m_editor.setEditType(EditType::Decals);
}

void MainFrameImpl::onDisplaceType(wxCommandEvent& event)
{
    m_editor.setEditType(EditType::Displace);
}

void MainFrameImpl::onSurfaceType(wxCommandEvent& event)
{
    m_editor.setEditType(EditType::Surfaces);
}

void MainFrameImpl::onCpType(wxCommandEvent& event)
{
    m_editor.setEditType(EditType::ControlPoints);
}

void MainFrameImpl::onCreateSurface(wxCommandEvent& event)
{
    m_surfaceDlg->ShowModal();
}

void MainFrameImpl::onResize(wxSizeEvent& event)
{
    int width;
    int height;

    DoGetClientSize(&width, &height);

    wxSize consoleSize = m_console->GetSize();
    m_console->SetPosition({ width - consoleSize.x, 0 });
    m_console->SetSize(consoleSize.x, height);
    m_console->onResize();

    width -= consoleSize.x;

    m_mainView->SetSize(width / 2, height / 2);
    m_topView->SetSize(width / 2, height / 2);
    m_sideView->SetSize(width / 2, height / 2);
    m_frontView->SetSize(width / 2, height / 2);

    m_topView->SetPosition({ 0, height / 2 });
    m_sideView->SetPosition({ width / 2, 0 });
    m_frontView->SetPosition({ width / 2, height / 2 });
}

void MainFrameImpl::onKeyUp(wxKeyEvent& event)
{
    if (event.GetKeyCode() == 'P')
    {
        switch (m_editor.getEditType())
        {
        case EditType::Polygons:
            if (!m_polyDlg->IsVisible())
            {
                m_polyDlg->update();
                m_polyDlg->Show(true);
            }
            else m_polyDlg->SetFocus();
        break;
        case EditType::Blocks:
            if (!m_blockDlg->IsVisible())
            {
                m_blockDlg->update();
                m_blockDlg->Show(true);
            }
            else m_blockDlg->SetFocus();
        break;
        case EditType::Objects:
            if (!m_objectDlg->IsVisible())
            {
                m_objectDlg->update();
                m_objectDlg->Show(true);
            }
            else m_objectDlg->SetFocus();
        break;
        case EditType::Surfaces:
            if (!m_surfaceEditDlg->IsVisible())
            {
                m_surfaceEditDlg->update();
                m_surfaceEditDlg->Show(true);
            }
            else m_surfaceEditDlg->SetFocus();
        break;
        }
    }
}

void MainFrameImpl::refreshViews()
{
    m_mainView->Refresh();
    m_topView->Refresh();
    m_sideView->Refresh();
    m_frontView->Refresh();
}

EditorInfo MainFrameImpl::getEditorInfo()
{
    EditorInfo editorInfo;

    const ViewCamera& camera = m_mainView->camera();

    editorInfo.campos = camera.pos();
    editorInfo.camvangle = camera.verticalAngle();
    editorInfo.camhangle = camera.horizontalAngle();

    editorInfo.topViewOffset = m_topView->offset();
    editorInfo.topViewScale = m_topView->scale();

    editorInfo.sideViewOffset = m_sideView->offset();
    editorInfo.sideViewScale = m_sideView->scale();

    editorInfo.frontViewOffset = m_frontView->offset();
    editorInfo.frontViewScale = m_frontView->scale();

    return editorInfo;
}

void MainFrameImpl::setEditorParams(const EditorInfo& editorInfo)
{
    ViewCamera& camera = m_mainView->camera();

    camera.setPos(editorInfo.campos);
    camera.setAngles(editorInfo.camvangle, editorInfo.camhangle);

    m_topView->setOffset(editorInfo.topViewOffset);
    m_topView->setScale(editorInfo.topViewScale);

    m_sideView->setOffset(editorInfo.sideViewOffset);
    m_sideView->setScale(editorInfo.sideViewScale);

    m_frontView->setOffset(editorInfo.frontViewOffset);
    m_frontView->setScale(editorInfo.frontViewScale);
}