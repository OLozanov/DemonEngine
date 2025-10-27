#pragma once

#include <wx/colordlg.h>
#include <wx/filedlg.h>

#include "Forms.h"
#include "DetailDlg.h"

#include "Objects/Entity.h"

#include <filesystem>

class ViewCamera;
class Editor;

class ConsoleImpl : public Console
{
    struct TextureInfo : public wxTreeItemData
    {
        std::string name;
        Material* mat;
        Image* img;
    };

    struct ModelInfo : public wxTreeItemData
    {
        std::string name;
        Model* model;
    };

    class ObjectInfo : public wxTreeItemData
    {
    public:
        ObjectType objectType;
        EntityClass entityClass;

        ObjectInfo(ObjectType type, EntityClass eclass = EntityClass::Invalid) : objectType(type), entityClass(eclass) {}
    };

public:

    ConsoleImpl(wxWindow* parent, Editor& editor, const ViewCamera& camera);

    void onResize();

private:

    void initMaterialList();
    void addMaterialDirectory(wxTreeItemId root, const std::filesystem::path& path);
    void initMeshList();
    void addMeshDirectory(wxTreeItemId root, const std::filesystem::path& path);
    void initEntityList();

    void parseClass(Lexer& lexer);
    void readEntityList(const std::string& filename);

    vec3 calcNewPos(float dist);

    void addMaterialDetails(const wxTreeItemId& id);

    void onMatTreeSelected(wxTreeEvent& event) override;
    void onApplyMaterial(wxCommandEvent& event) override;
    void onRefreshMaterials(wxCommandEvent& event) override;

    void onMeshTreeSelected(wxTreeEvent& event) override;
    void onCreateMesh(wxCommandEvent& event) override;
    void onRefreshMesh(wxCommandEvent& event) override;

    void onCreateLight(wxCommandEvent& event) override;
    void onChangeLightType(wxCommandEvent& event) override;
    void onSelectLightColor(wxMouseEvent& event) override;

    void onEntityTreeSelected(wxTreeEvent& event) override;
    void onCreateObject(wxCommandEvent& event) override;

    void onDispRadiusChange(wxSpinDoubleEvent& event) override;
    void onDispRadiusChange(wxCommandEvent& event) override;
    void onDispPowerChange(wxSpinDoubleEvent& event) override;
    void onDispPowerChange(wxCommandEvent& event) override;
    void onLayerChange(wxCommandEvent& event) override;
    void onDispUpBtn(wxCommandEvent& event) override;
    void onDispDownBtn(wxCommandEvent& event) override;
    void onDispSmoothBtn(wxCommandEvent& event) override;
    void onDispPaintBtn(wxCommandEvent& event) override;

    void onAddDetailBtn(wxCommandEvent& event) override;
    void onEditDetailBtn(wxCommandEvent& event) override;
    void onClearDetailBtn(wxCommandEvent& event) override;

private:
    DetailDlgImpl m_detailDlg;

    wxColourData m_colorData;
    wxColourDialog m_colorDialog;
    wxFileDialog m_loadDlg;

    Editor& m_editor;
    const ViewCamera& m_camera;

    Render::SwapChain m_swapChain;

    wxTreeItemId m_matId;
    wxTreeItemId m_meshId;
    wxTreeItemId m_objectId;

    wxTreeItemId m_entityTreeRoot;

    std::map<std::string, wxTreeItemId> m_entityCategories;
    std::map<EntityClass, EntityType*> m_entityTypes;

    enum LightType
    {
        omni_light = 0,
        spot_light = 1
    };
};