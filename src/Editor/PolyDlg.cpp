#include "PolyDlg.h"
#include "Editor.h"

PolyDlgImpl::PolyDlgImpl(wxWindow* parent, Editor& editor)
: PolyDlg(parent)
, m_editor(editor)
, m_resourceDlg(new ResourceDlgImpl(this))
, m_umoveRate(0.1f)
, m_vmoveRate(0.1f)
, m_uscaleRate(2.0f)
, m_vscaleRate(2.0f)
, m_rotateRate(5.0f)
{
}

void PolyDlgImpl::update()
{
    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();

    m_layerList->Clear();

    switch (polyNum)
    {
    case EditorSelectionType::None:
        m_portalBox->Disable();
        m_skyBox->Disable();
        m_invisibleBox->Disable();
        m_transparentBox->Disable();
        m_nocollisionBox->Disable();
        m_twosideBox->Disable();

        m_umoveBtn->Disable();
        m_umoveEdit->Disable();
        m_uscaleBtn->Disable();
        m_uscaleEdit->Disable();
        m_vmoveBtn->Disable();
        m_vmoveEdit->Disable();
        m_vscaleBtn->Disable();
        m_vscaleEdit->Disable();
        m_rotateBtn->Disable();
        m_rotateEdit->Disable();

        m_smgroopEdit->Disable();
        m_subdivideBtn->Disable();
        m_resEdit->Disable();

        m_layersCaption->Disable();
        m_layerList->Disable();
        m_addLayerBtn->Disable();
        m_delLayerBtn->Disable();
        m_layerUpBtn->Disable();
        m_layerDownBtn->Disable();

        m_tesselateBox->Disable();
        m_mappingCaption->Disable();
        m_mappingCombo->Disable();

        m_tesselateBox->SetValue(false);
    break;
    case EditorSelectionType::One:
    {
        m_portalBox->Enable();
        m_skyBox->Enable();
        m_invisibleBox->Enable();
        m_transparentBox->Enable();
        m_nocollisionBox->Enable();
        m_twosideBox->Enable();

        uint8_t flags = m_editor.getPolygonFlags();

        if (flags & PolyZonePortal) m_portalBox->SetValue(true);
        else m_portalBox->SetValue(false);

        if (flags & PolySky) m_skyBox->SetValue(true);
        else m_skyBox->SetValue(false);

        if (flags & PolyInvisible) m_invisibleBox->SetValue(true);
        else m_invisibleBox->SetValue(false);

        if (flags & PolyTransparent) m_transparentBox->SetValue(true);
        else m_transparentBox->SetValue(false);

        if (flags & PolyNoCollision) m_nocollisionBox->SetValue(true);
        else m_nocollisionBox->SetValue(false);

        if (flags & PolyTwoSide) m_twosideBox->SetValue(true);
        else m_twosideBox->SetValue(false);

        wxString smgroopStr = wxString::FromDouble(m_editor.getPolygonSmoothGroop());
        m_smgroopEdit->SetValue(smgroopStr);

        m_umoveBtn->Enable();
        m_umoveEdit->Enable();
        m_uscaleBtn->Enable();
        m_uscaleEdit->Enable();
        m_vmoveBtn->Enable();
        m_vmoveEdit->Enable();
        m_vscaleBtn->Enable();
        m_vscaleEdit->Enable();
        m_rotateBtn->Enable();
        m_rotateEdit->Enable();

        m_smgroopEdit->Enable();
        m_subdivideBtn->Enable();
        m_resEdit->Enable();

        EditSurface* surface = m_editor.getPolygonSurface();

        if (surface)
        {
            const std::vector<SurfaceLayer>& layers = surface->layers();
            for (const SurfaceLayer& layer : layers) m_layerList->Append(layer.material->name);

            m_mappingCombo->SetSelection(static_cast<uint32_t>(surface->baseLayerType()));

            m_tesselateBox->SetValue(surface->tesselationEnabled());
        }
        else
        {
            m_tesselateBox->SetValue(false);
        }
    }
    break;
    case EditorSelectionType::Multiple:
        m_portalBox->Disable();
        m_skyBox->Disable();
        m_invisibleBox->Disable();
        m_transparentBox->Disable();
        m_nocollisionBox->Disable();
        m_twosideBox->Disable();

        m_umoveBtn->Enable();
        m_umoveEdit->Enable();
        m_uscaleBtn->Enable();
        m_uscaleEdit->Enable();
        m_vmoveBtn->Enable();
        m_vmoveEdit->Enable();
        m_vscaleBtn->Enable();
        m_vscaleEdit->Enable();
        m_rotateBtn->Enable();
        m_rotateEdit->Enable();

        wxString smgroopStr = wxString::FromDouble(m_editor.getPolygonSmoothGroop());
        m_smgroopEdit->SetValue(smgroopStr);

        m_smgroopEdit->Enable();
        m_subdivideBtn->Enable();
        m_resEdit->Enable();

        m_tesselateBox->Disable();
        m_mappingCaption->Disable();
        m_mappingCombo->Disable();

        m_tesselateBox->SetValue(false);
    break;
    }

    if (polyNum != EditorSelectionType::None)
    {
        double val;

        if ((m_umoveEdit->GetValue()).ToDouble(&val)) m_umoveRate = val; else m_umoveRate = 0.1;
        if ((m_vmoveEdit->GetValue()).ToDouble(&val)) m_vmoveRate = val; else m_vmoveRate = 0.1;

        if ((m_uscaleEdit->GetValue()).ToDouble(&val)) m_uscaleRate = val; else m_uscaleRate = 2;
        if ((m_vscaleEdit->GetValue()).ToDouble(&val)) m_vscaleRate = val; else m_vscaleRate = 2;

        if ((m_rotateEdit->GetValue()).ToDouble(&val)) m_rotateRate = val; else m_uscaleRate = 5;

        EditSurface* surface = m_editor.getPolygonSurface();

        bool editLayers = polyNum == EditorSelectionType::One && surface != nullptr;

        m_layersCaption->Enable(surface != nullptr);
        m_layerList->Enable(editLayers);
        m_addLayerBtn->Enable(surface != nullptr);
        m_delLayerBtn->Enable(editLayers);
        m_layerUpBtn->Enable(editLayers);
        m_layerDownBtn->Enable(editLayers);

        m_tesselateBox->Enable(editLayers);
        m_mappingCaption->Enable(editLayers);
        m_mappingCombo->Enable(editLayers);
    }
}

void PolyDlgImpl::onPortalBox(wxCommandEvent& event)
{
    m_editor.setPolygonFlag(PolyZonePortal, m_portalBox->IsChecked());
}

void PolyDlgImpl::onSkyBox(wxCommandEvent& event)
{
    m_editor.setPolygonFlag(PolySky, m_skyBox->IsChecked());
}

void PolyDlgImpl::onInvisibleBox(wxCommandEvent& event)
{
    m_editor.setPolygonFlag(PolyInvisible, m_invisibleBox->IsChecked());
}

void PolyDlgImpl::onTransparentBox(wxCommandEvent& event)
{
    m_editor.setPolygonFlag(PolyTransparent, m_transparentBox->IsChecked());
}

void PolyDlgImpl::onNocollisionBox(wxCommandEvent& event)
{
    m_editor.setPolygonFlag(PolyNoCollision, m_nocollisionBox->IsChecked());
}

void PolyDlgImpl::onTwosideBox(wxCommandEvent& event)
{
    m_editor.setPolygonFlag(PolyTwoSide, m_twosideBox->IsChecked());
}

void PolyDlgImpl::onUMoveDown(wxSpinEvent& event)
{
    m_editor.movePolyU(m_umoveRate);
}

void PolyDlgImpl::onUMoveUp(wxSpinEvent& event)
{
    m_editor.movePolyU(-m_umoveRate);
}

void PolyDlgImpl::onUScaleDown(wxSpinEvent& event)
{
    m_editor.scalePolyU(1.0f / m_uscaleRate);
}

void PolyDlgImpl::onUScaleUp(wxSpinEvent& event)
{
    m_editor.scalePolyU(m_uscaleRate);
}

void PolyDlgImpl::onVMoveDown(wxSpinEvent& event)
{
    m_editor.movePolyV(m_vmoveRate);
}

void PolyDlgImpl::onVMoveUp(wxSpinEvent& event)
{
    m_editor.movePolyV(-m_vmoveRate);
}

void PolyDlgImpl::onVScaleDown(wxSpinEvent& event)
{
    m_editor.scalePolyV(1.0f / m_vscaleRate);
}

void PolyDlgImpl::onVScaleUp(wxSpinEvent& event)
{
    m_editor.scalePolyV(m_vscaleRate);
}

void PolyDlgImpl::onRotateDown(wxSpinEvent& event)
{
    m_editor.rotatePolyUV(-m_rotateRate);
}

void PolyDlgImpl::onRotateUp(wxSpinEvent& event)
{
    m_editor.rotatePolyUV(m_rotateRate);
}

void PolyDlgImpl::onUMoveRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_umoveEdit->GetValue()).ToDouble(&val)) m_umoveRate = val;
}

void PolyDlgImpl::onVMoveRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_vmoveEdit->GetValue()).ToDouble(&val)) m_vmoveRate = val;
}

void PolyDlgImpl::onUScaleRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_uscaleEdit->GetValue()).ToDouble(&val)) m_uscaleRate = val;
}

void PolyDlgImpl::onVScaleRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_vscaleEdit->GetValue()).ToDouble(&val)) m_vscaleRate = val;
}

void PolyDlgImpl::onRotateRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_rotateEdit->GetValue()).ToDouble(&val)) m_rotateRate = val;
}

void PolyDlgImpl::onSmoothGroopEnter(wxCommandEvent& event)
{
    int val;
    m_smgroopEdit->GetValue().ToInt(&val);
    m_staticText6->SetFocus();  // dirty hack

    m_editor.setPolygonSmoothGroop(val);
}

void PolyDlgImpl::onSubdivide(wxCommandEvent& event)
{
    int factor = 4;
    m_resEdit->GetValue().ToInt(&factor);

    m_editor.subdividePolygons(factor);

    update();
}

void PolyDlgImpl::onAddLayer(wxCommandEvent& event)
{
    if (m_resourceDlg->open("Textures", ".mtl", false) == wxID_OK)
    {
        const wxString& matname = m_resourceDlg->getPath();
        m_editor.addSurfaceLayer(m_editor.loadMaterial(matname.ToStdString()));

        EditorSelectionType polyNum = m_editor.selectedPolygonsNum();

        if (polyNum == EditorSelectionType::One) m_layerList->Append(matname);
    }
}

void PolyDlgImpl::onDeleteLayer(wxCommandEvent& event)
{
    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();
    if (polyNum != EditorSelectionType::One) return;

    EditSurface* surface = m_editor.getPolygonSurface();

    if (!surface) return;

    int idx = m_layerList->GetSelection();

    if (idx != wxNOT_FOUND)
    {
        m_layerList->Delete(idx);
        surface->deleteLayer(idx);
    }

    m_editor.updateViews();
}

void PolyDlgImpl::onLayerUp(wxCommandEvent& event)
{
    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();
    if (polyNum != EditorSelectionType::One) return;

    EditSurface* surface = m_editor.getPolygonSurface();

    if (!surface) return;

    int idx = m_layerList->GetSelection();

    if (idx == 0) return;

    if (idx != wxNOT_FOUND)
    {
        const wxString& matname = m_layerList->GetString(idx);
        m_layerList->Delete(idx);
        m_layerList->Insert(matname, idx - 1);
        m_layerList->SetSelection(idx - 1);
        
        surface->moveLayerUp(idx);

        m_editor.updateViews();
    }
}

void PolyDlgImpl::onLayerDown(wxCommandEvent& event)
{
    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();
    if (polyNum != EditorSelectionType::One) return;

    EditSurface* surface = m_editor.getPolygonSurface();

    if (!surface) return;

    int idx = m_layerList->GetSelection();

    if (idx == m_layerList->GetCount() - 1) return;

    if (idx != wxNOT_FOUND)
    {
        const wxString& matname = m_layerList->GetString(idx);
        m_layerList->Delete(idx);
        m_layerList->Insert(matname, idx + 1);
        m_layerList->SetSelection(idx + 1);

        surface->moveLayerDown(idx);

        m_editor.updateViews();
    }
}

void PolyDlgImpl::onLayerDeselect(wxCommandEvent& event)
{
    m_layerList->Deselect(wxNOT_FOUND);

    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();
    if (polyNum != EditorSelectionType::One) return;

    EditSurface* surface = m_editor.getPolygonSurface();
    if (!surface) return;

    m_mappingCombo->SetSelection(static_cast<uint32_t>(surface->baseLayerType()));
}

void PolyDlgImpl::onLayer(wxCommandEvent& event)
{
    int idx = event.GetInt();

    if (idx == wxNOT_FOUND) return;

    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();
    if (polyNum != EditorSelectionType::One) return;

    EditSurface* surface = m_editor.getPolygonSurface();
    if (!surface) return;

    m_mappingCombo->SetSelection(static_cast<uint32_t>(surface->layer(idx).type));
}

void PolyDlgImpl::onLayerDClick(wxCommandEvent& event)
{
    int idx = event.GetInt();

    if (idx == wxNOT_FOUND) return;

    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();
    if (polyNum != EditorSelectionType::One) return;

    EditSurface* surface = m_editor.getPolygonSurface();
    if (!surface) return;

    if (m_resourceDlg->open("Textures", ".mtl", false) == wxID_OK)
    {
        const wxString& matname = m_resourceDlg->getPath();
        surface->setMaterial(m_editor.loadMaterial(matname.ToStdString()), idx);

        m_layerList->SetString(idx, matname);

        m_editor.updateViews();
    }
}

void PolyDlgImpl::onTesselationBox(wxCommandEvent& event)
{
    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();
    if (polyNum != EditorSelectionType::One) return;

    EditSurface* surface = m_editor.getPolygonSurface();
    if (!surface) return;

    surface->enableTesselation(m_tesselateBox->IsChecked());
}

void PolyDlgImpl::onMappingChange(wxCommandEvent& event)
{
    EditorSelectionType polyNum = m_editor.selectedPolygonsNum();
    if (polyNum != EditorSelectionType::One) return;

    EditSurface* surface = m_editor.getPolygonSurface();
    if (!surface) return;

    int layer = m_layerList->GetSelection();
    int idx = m_mappingCombo->GetSelection();

    if (layer == wxNOT_FOUND)
        surface->setLayerType(static_cast<LayerType>(idx));
    else
        surface->setLayerType(static_cast<LayerType>(idx), layer);
}