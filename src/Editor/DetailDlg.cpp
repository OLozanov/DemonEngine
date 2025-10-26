#include "DetailDlg.h"

#include "Editor.h"

DetailDlgImpl::DetailDlgImpl(wxWindow* parent, Editor& editor)
: DetailDlg(parent)
, m_editor(editor)
{
}

void DetailDlgImpl::onAddBtn(wxCommandEvent& event)
{
    size_t layer = m_layerCtrl->GetValue();
    std::string model = m_modelCtrl->GetValue();
    std::string material = m_materialCtrl->GetValue();
    float density = m_densityCtrl->GetValue();

    m_editor.addSurfaceDetails(layer, model, material, density);

    EndModal(0);
}

void DetailDlgImpl::onAddDetailBtn(wxCommandEvent& event)
{

}

void DetailDlgImpl::onRemoveDetailDlg(wxCommandEvent& event)
{

}