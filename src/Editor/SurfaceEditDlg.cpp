#include "SurfaceEditDlg.h"
#include "Editor.h"

#include <wx/msgdlg.h>

SurfaceEditDlgImpl::SurfaceEditDlgImpl(wxWindow* parent, Editor& editor)
: SurfaceEditDlg(parent)
, m_editor(editor)
, m_umoveRate(0.1f)
, m_vmoveRate(0.1f)
, m_uscaleRate(2.0f)
, m_vscaleRate(2.0f)
, m_rotateRate(5.0f)
{
}

void SurfaceEditDlgImpl::update()
{
    EditorSelectionType surfaceNum = m_editor.selectedSurfacesNum();
    Surface* selectedSurface = m_editor.selectedSurface();

    if (surfaceNum == EditorSelectionType::One)
    {
        m_xresEdit->Enable(true);
        m_yresEdit->Enable(true);

        m_mappingCombo->Enable(true);

        m_collisionBox->Enable(true);
        m_normalsBox->Enable(true);

        if (selectedSurface->type() == SurfaceType::BSpline)
        {
            m_xcycleBox->Show(true);
            m_ycycleBox->Show(true);

            m_xendBox->Show(true);
            m_yendBox->Show(true);
        }
        else
        {
            m_xcycleBox->Show(false);
            m_ycycleBox->Show(false);

            m_xendBox->Show(false);
            m_yendBox->Show(false);
        }

        uint32_t xres = selectedSurface->getXResolution();
        uint32_t yres = selectedSurface->getYResolution();

        m_xresEdit->SetValue(wxString::FromDouble(xres));
        m_yresEdit->SetValue(wxString::FromDouble(yres));

        TextureMapping mapMode = selectedSurface->getMapMode();

        if (mapMode == TextureMapping::TCoords) m_mappingCombo->SetSelection(0);
        else m_mappingCombo->SetSelection(1);

        m_collisionBox->SetValue(selectedSurface->getFlags(Surface::surf_collision));
        m_normalsBox->SetValue(selectedSurface->getFlags(Surface::surf_accurate_normals));

        m_xcycleBox->SetValue(selectedSurface->getFlags(Surface::surf_cyclic_x));
        m_ycycleBox->SetValue(selectedSurface->getFlags(Surface::surf_cyclic_y));

        m_xendBox->SetValue(selectedSurface->getFlags(Surface::surf_endpoint_x));
        m_yendBox->SetValue(selectedSurface->getFlags(Surface::surf_endpoint_y));

        m_applyBtn->Enable(true);
    }
    else
    {
        m_xresEdit->Enable(false);
        m_yresEdit->Enable(false);

        m_mappingCombo->Enable(false);

        m_collisionBox->Enable(false);
        m_normalsBox->Enable(false);

        m_xcycleBox->Show(false);
        m_ycycleBox->Show(false);

        m_xendBox->Show(false);
        m_yendBox->Show(false);

        m_applyBtn->Enable(false);
    }

    if (surfaceNum == EditorSelectionType::None)
    {
        m_umoveBtn->Enable(false);
        m_vmoveBtn->Enable(false);

        m_uscaleBtn->Enable(false);
        m_vscaleBtn->Enable(false);

        m_rotateBtn->Enable(false);
    }
    else
    {
        m_umoveBtn->Enable(true);
        m_vmoveBtn->Enable(true);

        m_uscaleBtn->Enable(true);
        m_vscaleBtn->Enable(true);

        m_rotateBtn->Enable(true);
    }
}

void SurfaceEditDlgImpl::onUMoveDown(wxSpinEvent& event)
{
    m_editor.moveSurfaceTexS(-m_umoveRate);
}

void SurfaceEditDlgImpl::onUMoveUp(wxSpinEvent& event)
{
    m_editor.moveSurfaceTexS(m_umoveRate);
}

void SurfaceEditDlgImpl::onUScaleDown(wxSpinEvent& event)
{
    m_editor.scaleSurfaceTexS(1.0 / m_uscaleRate);
}

void SurfaceEditDlgImpl::onUScaleUp(wxSpinEvent& event)
{
    m_editor.scaleSurfaceTexS(m_uscaleRate);
}

void SurfaceEditDlgImpl::onVMoveDown(wxSpinEvent& event)
{
    m_editor.moveSurfaceTexT(-m_vmoveRate);
}

void SurfaceEditDlgImpl::onVMoveUp(wxSpinEvent& event)
{
    m_editor.moveSurfaceTexT(m_vmoveRate);
}

void SurfaceEditDlgImpl::onVScaleDown(wxSpinEvent& event)
{
    m_editor.scaleSurfaceTexT(1.0 / m_vscaleRate);
}

void SurfaceEditDlgImpl::onVScaleUp(wxSpinEvent& event)
{
    m_editor.scaleSurfaceTexT(m_vscaleRate);
}

void SurfaceEditDlgImpl::onRotateDown(wxSpinEvent& event)
{
    m_editor.rotateSurfaceTex(-m_rotateRate);
}

void SurfaceEditDlgImpl::onRotateUp(wxSpinEvent& event)
{
    m_editor.rotateSurfaceTex(m_rotateRate);
}

void SurfaceEditDlgImpl::onUMoveRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_umoveEdit->GetValue()).ToDouble(&val)) m_umoveRate = val;
}

void SurfaceEditDlgImpl::onVMoveRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_vmoveEdit->GetValue()).ToDouble(&val)) m_vmoveRate = val;
}

void SurfaceEditDlgImpl::onUScaleRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_uscaleEdit->GetValue()).ToDouble(&val)) m_uscaleRate = val;
}

void SurfaceEditDlgImpl::onVScaleRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_vscaleEdit->GetValue()).ToDouble(&val)) m_vscaleRate = val;
}

void SurfaceEditDlgImpl::onRotateRateChanged(wxCommandEvent& event)
{
    double val;
    if ((m_rotateEdit->GetValue()).ToDouble(&val)) m_rotateRate = val;
}

void SurfaceEditDlgImpl::onApply(wxCommandEvent& event)
{
    Surface* selectedSurface = m_editor.selectedSurface();

    if (!selectedSurface)
    {
        Close();
        return;
    }

    int xres, yres;

    bool resize = true;

    if (!m_xresEdit->GetValue().ToInt(&xres))
    {
        wxMessageBox("X resolution. Incorrect numeric value.");
        resize = false;
    }

    if (!m_yresEdit->GetValue().ToInt(&yres))
    {
        wxMessageBox("Y resolution. Incorrect numeric value.");
        resize = false;
    }

    if (resize) selectedSurface->setResolution(xres, yres);

    TextureMapping mapping = m_mappingCombo->GetSelection() == 0 ? TextureMapping::TCoords : TextureMapping::TSpace;
    selectedSurface->setMapMode(mapping);

    selectedSurface->setFlag(Surface::surf_collision, m_collisionBox->GetValue());
    selectedSurface->setFlag(Surface::surf_accurate_normals, m_normalsBox->GetValue());

    selectedSurface->setFlag(Surface::surf_cyclic_x, m_xcycleBox->GetValue());
    selectedSurface->setFlag(Surface::surf_cyclic_y, m_ycycleBox->GetValue());

    selectedSurface->setFlag(Surface::surf_endpoint_x, m_xendBox->GetValue());
    selectedSurface->setFlag(Surface::surf_endpoint_y, m_yendBox->GetValue());

    selectedSurface->reconfig();

    Close();
    onSurfaceUpdate();
}

void SurfaceEditDlgImpl::onClose(wxCommandEvent& event)
{
    Close();
}