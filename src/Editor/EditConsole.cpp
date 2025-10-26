#include "EditConsole.h"
#include "Editor.h"

#include "View/ViewCamera.h"

#include "Objects/Light.h"
#include "Objects/StaticObject.h"
#include "Objects/Sound.h"
#include "Objects/Trigger.h"
#include "Objects/PlayerStart.h"
#include "Objects/MapFinish.h"
#include "Objects/Ladder.h"
#include "Objects/FogVolume.h"

///TODO: If its required to define entity manually
/*static const EntityType MoverType = { "Mover", {},
                                     {{"id", TypeInfoMember::Type::String, nullptr, ""},
                                      {"model", TypeInfoMember::Type::Model, nullptr, "Tech/door1.msh"},
                                      {"played sound", TypeInfoMember::Type::Sound, nullptr, ""},
                                      {"direction", TypeInfoMember::Type::Vec3, nullptr, vec3(0, 1, 0)},
                                      {"move distance", TypeInfoMember::Type::Float, nullptr, 2.0f},
                                      {"move speed", TypeInfoMember::Type::Float, nullptr, 0.2f}}
                                    };*/

ConsoleImpl::ConsoleImpl(wxWindow* parent, Editor& editor, const ViewCamera& camera)
: Console(parent)
, m_detailDlg(this, editor)
, m_colorDialog(this, &m_colorData)
, m_loadDlg(this, _("Select file"), _(".\\Textures"), wxEmptyString, _("*.mtl"), wxFD_DEFAULT_STYLE | wxFD_OPEN, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"))
, m_editor(editor)
, m_camera(camera)
, m_swapChain(Render::GpuInstance::GetInstance().createSwapChain(m_previewPanel->GetHandle()))
{
    m_matId = m_materialTree->AddRoot("Materials", -1, -1);
    m_meshTree->Expand(m_matId);

    initMeshList();
    initEntityList();
}

void ConsoleImpl::initMaterialList()
{
    wxTreeItemId root = m_materialTree->AddRoot("Materials", -1, -1);
    addMaterialDirectory(root, "./Textures/");

    m_materialTree->Expand(m_matId);
}

void ConsoleImpl::addMaterialDirectory(wxTreeItemId root, const std::filesystem::path& path)
{
    std::vector<std::filesystem::path> folders;
    std::vector<std::filesystem::path> meshes;

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            folders.push_back(entry.path());
        }
        else
        {
            std::string ext = entry.path().extension().string();
            if (ext == ".mtl") meshes.push_back(entry.path());
        }
    }

    for (const std::filesystem::path& path : folders)
    {
        wxTreeItemId dir = m_materialTree->AppendItem(root, path.filename().c_str(), -1, -1, 0);
        addMaterialDirectory(dir, path);
    }

    for (const std::filesystem::path& path : meshes)
    {
        auto fname = path.filename().replace_extension();

        TextureInfo* matinfo = new TextureInfo;
        matinfo->name = path.lexically_relative("./Textures/").string();
        matinfo->mat = nullptr;

        m_materialTree->AppendItem(root, fname.c_str(), -1, -1, matinfo);
    }
}

void ConsoleImpl::initMeshList()
{
    wxTreeItemId root = m_meshTree->AddRoot("Models", -1, - 1);
    addMeshDirectory(root, "./Models/");

    m_meshTree->Expand(root);
}

void ConsoleImpl::addMeshDirectory(wxTreeItemId root, const std::filesystem::path& path)
{
    std::vector<std::filesystem::path> folders;
    std::vector<std::filesystem::path> meshes;

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            folders.push_back(entry.path());
        }
        else
        {
            std::string ext = entry.path().extension().string();
            if (ext == ".msh") meshes.push_back(entry.path());
        }
    }

    for (const std::filesystem::path& path : folders)
    {
        wxTreeItemId dir = m_meshTree->AppendItem(root, path.filename().c_str(), -1, -1, 0);
        addMeshDirectory(dir, path);
    }

    for (const std::filesystem::path& path : meshes)
    {
        auto fname = path.filename().replace_extension();

        ModelInfo* minfo = new ModelInfo;
        minfo->name = path.lexically_relative("./Models/").string();
        minfo->model = nullptr;

        m_meshTree->AppendItem(root, fname.c_str(), -1, -1, minfo);
    }
}

void ConsoleImpl::initEntityList()
{
    m_entityTreeRoot = m_entityTree->AddRoot("Entities", -1, -1);

    wxTreeItemId map = m_entityTree->AppendItem(m_entityTreeRoot, "map", -1, -1, 0);
    wxTreeItemId items = m_entityTree->AppendItem(m_entityTreeRoot, "items", -1, -1, 0);
    wxTreeItemId enemies = m_entityTree->AppendItem(m_entityTreeRoot, "enemies", -1, -1, 0);
    wxTreeItemId activators = m_entityTree->AppendItem(m_entityTreeRoot, "activators", -1, -1, 0);
    wxTreeItemId activeObjects = m_entityTree->AppendItem(m_entityTreeRoot, "active objects", -1, -1, 0);
    wxTreeItemId interactiveObjects = m_entityTree->AppendItem(m_entityTreeRoot, "interactive objects", -1, -1, 0);

    m_entityCategories["map"] = map;
    m_entityCategories["items"] = items;
    m_entityCategories["enemies"] = enemies;
    m_entityCategories["activators"] = activators;
    m_entityCategories["active objects"] = activeObjects;
    m_entityCategories["interactive objects"] = interactiveObjects;

    m_entityTree->AppendItem(map, "PlayerStart", -1, -1, new ObjectInfo(ObjectType::PlayerStart));
    m_entityTree->AppendItem(map, "MapFinish", -1, -1, new ObjectInfo(ObjectType::MapFinish));
    m_entityTree->AppendItem(map, "Sound", -1, -1, new ObjectInfo(ObjectType::AmbientSound));
    m_entityTree->AppendItem(map, "Ladder", -1, -1, new ObjectInfo(ObjectType::Ladder));
    m_entityTree->AppendItem(map, "FogVolume", -1, -1, new ObjectInfo(ObjectType::FogVolume));

    m_entityTree->AppendItem(activators, "Trigger", -1, -1, new ObjectInfo(ObjectType::Trigger));

    m_entityTree->Expand(m_entityTreeRoot);

    readEntityList("Editor/classes.def");
}

vec3 ConsoleImpl::calcNewPos(float dist)
{
    return m_camera.pos() + m_camera.basis()[2] * dist;
}

void ConsoleImpl::addMaterial(const std::string& name, Material* mat)
{
    TextureInfo* matinfo = new TextureInfo;
    matinfo->mat = mat;

    wxTreeItemId root = m_materialTree->AppendItem(m_matId, name, -1, -1, matinfo);

    for (int i = 0; i < 2; i++)
    {
        ImagePtr& img = mat->img[i];

        if (!img) continue;

        char* name = nullptr;

        switch (i)
        {
        case 0: name = "diffuse"; break;
        case 1: name = "normal"; break;
        case 2: name = "height"; break;
        }

        TextureInfo* texinfo = new TextureInfo;
        texinfo->mat = mat;
        texinfo->img = img;

        m_materialTree->AppendItem(root, name, -1, -1, texinfo);
    }
}

void ConsoleImpl::onResize()
{
    wxSize size = GetSize();

    m_tabPanel->SetSize(size.x, size.y);
}

void ConsoleImpl::onMatTreeSelected(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();

    TextureInfo* tinfo = (TextureInfo*)m_materialTree->GetItemData(id);

    if (tinfo)
    {
        Material* mat = tinfo->mat;
        m_editor.setCurrentMaterial(mat);
    }
}

void ConsoleImpl::onApplyMaterial(wxCommandEvent& event)
{
    switch (m_editor.getEditType())
    {
    case EditType::Polygons:
        m_editor.applyMaterialToPolygons();
    break;
    case EditType::Surfaces:
        m_editor.applyMaterialToSurfaces();
    break;
    }
}

void ConsoleImpl::onAddMaterial(wxCommandEvent& event)
{
    m_loadDlg.SetDirectory(".\\Textures");
    m_loadDlg.SetWildcard("*.mtl");

    if (m_loadDlg.ShowModal() != wxID_CANCEL)
    {
        wxString path = m_loadDlg.GetPath();

        int left = path.Find("Textures\\");
        if (left == wxNOT_FOUND) return;

        left += 9;

        int right = path.Find(".mtl");
        if (right == wxNOT_FOUND) return;

        wxString matname = path.SubString(left, right - 1);
        std::string name = matname.ToStdString();

        m_editor.loadMaterial(name);
    }
}

void ConsoleImpl::onMeshTreeSelected(wxTreeEvent& event)
{
    m_meshId = event.GetItem();
}

void ConsoleImpl::onCreateMesh(wxCommandEvent& event)
{
    ModelInfo* minfo = (ModelInfo*)m_meshTree->GetItemData(m_meshId);

    if (!minfo) return;

    StaticObject* obj = new StaticObject(minfo->name);

    vec3 pos = calcNewPos(2.0);
    obj->setPos(pos);

    m_editor.addObject(obj);
}

void ConsoleImpl::onRefreshMesh(wxCommandEvent& event)
{
    m_meshTree->DeleteAllItems();
    initMeshList();
}

void ConsoleImpl::onCreateLight(wxCommandEvent& event)
{
    double radius;
    double falloff;
    double power;
    double angle;
    double innerAngle;

    m_lightRadiusEdt->GetValue().ToDouble(&radius);
    m_lightFalloffEdt->GetValue().ToDouble(&falloff);
    m_lightPowerEdt->GetValue().ToDouble(&power);

    wxColour wxcolor = m_lightColorPanel->GetBackgroundColour();
    vec3 color = { wxcolor.Red() / 255.0f, wxcolor.Green() / 255.0f, wxcolor.Blue() / 255.0f };

    LightShadow shadow = static_cast<LightShadow>(m_lightShadowBox->GetSelection());

    if (m_lightTypeBox->GetSelection() == omni_light)
    {
        OmniLight* light = new OmniLight(radius, falloff, power, color, shadow);

        vec3 pos = calcNewPos(2.0);
        light->setPos(pos);

        m_editor.addObject(light);
    }
    else
    {
        m_lightAngleEdt->GetValue().ToDouble(&angle);
        m_lightAngleInnerEdt->GetValue().ToDouble(&innerAngle);
    
        SpotLight* light = new SpotLight(radius, falloff, angle, innerAngle, power, color, shadow);

        vec3 pos = calcNewPos(2.0);
        light->setPos(pos);
        light->setOrientation(m_camera.basis());

        m_editor.addObject(light);
    }
}

void ConsoleImpl::onChangeLightType(wxCommandEvent& event)
{
    if (m_lightTypeBox->GetSelection() == omni_light)
    {
        m_lightAngleCaption->Hide();
        m_lightAngleEdt->Hide();
        m_lightAngleInnerEdt->Hide();
    }
    else
    {
        m_lightAngleCaption->Show(true);
        m_lightAngleEdt->Show(true);
        m_lightAngleInnerEdt->Show(true);
    }

    m_lightPanel->Layout();
}

void ConsoleImpl::onSelectLightColor(wxMouseEvent& event)
{
    m_colorDialog.Centre();

    if (m_colorDialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = m_colorDialog.GetColourData();
        wxColour color = retData.GetColour();

        m_lightColorPanel->SetBackgroundColour(color);
        m_lightColorPanel->Refresh();
    }
}

void ConsoleImpl::onEntityTreeSelected(wxTreeEvent& event)
{
    m_objectId = event.GetItem();
}

void ConsoleImpl::onCreateObject(wxCommandEvent& event)
{
    ObjectInfo* tinfo = (ObjectInfo*)m_entityTree->GetItemData(m_objectId);

    if (tinfo)
    {
        switch (tinfo->objectType)
        {
        case ObjectType::Entity:
        {
            Object* entity = Entity::CreateEntity(tinfo->entityClass);

            vec3 pos = calcNewPos(2.0);
            entity->setPos(pos);

            m_editor.addObject(entity);
        }
        break;
        case ObjectType::AmbientSound:
        {
            Object* obj = new AmbientSound();

            vec3 pos = calcNewPos(2.0);
            obj->setPos(pos);

            m_editor.addObject(obj);
        }
        break;
        case ObjectType::Trigger:
        {
            Object* obj = new Trigger();

            vec3 pos = calcNewPos(2.0);
            obj->setPos(pos);

            m_editor.addObject(obj);
        }
        break;
        case ObjectType::PlayerStart:
        {
            Object* obj = new PlayerStart();

            vec3 pos = calcNewPos(2.0);
            obj->setPos(pos);

            m_editor.addObject(obj);
        }
        break;
        case ObjectType::MapFinish:
        {
            Object* obj = new MapFinish();

            vec3 pos = calcNewPos(2.0);
            obj->setPos(pos);

            m_editor.addObject(obj);
        }
        break;
        case ObjectType::Ladder:
        {
            Object* obj = new Ladder();

            vec3 pos = calcNewPos(2.0);
            obj->setPos(pos);

            m_editor.addObject(obj);
        }
        break;
        case ObjectType::FogVolume:
            Object* obj = new FogVolume();

            vec3 pos = calcNewPos(2.0);
            obj->setPos(pos);

            m_editor.addObject(obj);
        break;
        }
    }
}

void ConsoleImpl::onDispRadiusChange(wxSpinDoubleEvent& event)
{
    m_editor.setDisplaceRadius(m_dispRadiusEdit->GetValue());
}

void ConsoleImpl::onDispRadiusChange(wxCommandEvent& event)
{
    m_editor.setDisplaceRadius(m_dispRadiusEdit->GetValue());
}

void ConsoleImpl::onDispPowerChange(wxSpinDoubleEvent& event)
{
    m_editor.setDisplacePower(m_dispPowerEdit->GetValue());
}

void ConsoleImpl::onDispPowerChange(wxCommandEvent& event)
{
    m_editor.setDisplacePower(m_dispPowerEdit->GetValue());
}

void ConsoleImpl::onLayerChange(wxCommandEvent& event)
{
    m_editor.setPaintLayer(m_layerCtrl->GetValue());
}

void ConsoleImpl::onDispUpBtn(wxCommandEvent& event)
{
    m_dispDownBtn->SetValue(false);
    m_dispSmoothBtn->SetValue(false);
    m_dispPaintBtn->SetValue(false);

    m_layerCtrl->Enable(false);

    m_editor.setDisplaceMode(DisplaceMode::Up);
}

void ConsoleImpl::onDispDownBtn(wxCommandEvent& event)
{
    m_dispUpBtn->SetValue(false);
    m_dispSmoothBtn->SetValue(false);
    m_dispPaintBtn->SetValue(false);

    m_layerCtrl->Enable(false);

    m_editor.setDisplaceMode(DisplaceMode::Down);
}

void ConsoleImpl::onDispSmoothBtn(wxCommandEvent& event)
{
    m_dispUpBtn->SetValue(false);
    m_dispDownBtn->SetValue(false);
    m_dispPaintBtn->SetValue(false);

    m_layerCtrl->Enable(false);

    m_editor.setDisplaceMode(DisplaceMode::Smooth);
}

void ConsoleImpl::onDispPaintBtn(wxCommandEvent& event)
{
    m_dispUpBtn->SetValue(false);
    m_dispDownBtn->SetValue(false);
    m_dispSmoothBtn->SetValue(false);

    m_layerCtrl->Enable(true);

    m_editor.setDisplaceMode(DisplaceMode::Paint);
}

void ConsoleImpl::onAddDetailBtn(wxCommandEvent& event)
{
    m_detailDlg.ShowModal();
}

void ConsoleImpl::onEditDetailBtn(wxCommandEvent& event)
{

}

void ConsoleImpl::onClearDetailBtn(wxCommandEvent& event)
{
    m_editor.clearSurfaceDetails();
}