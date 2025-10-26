#include "SurfaceCpDlg.h"
#include <wx/msgdlg.h>

#include "Editor.h"

SurfaceCpDlgImpl::SurfaceCpDlgImpl(wxWindow* parent, Editor& editor)
: SurfaceCpDlg(parent)
, m_editor(editor)
{
}

void SurfaceCpDlgImpl::onSetWeight(wxCommandEvent& event)
{
    double weight;

    if (!m_weightEdit->GetValue().ToDouble(&weight))
    {
        wxMessageBox("Incorrect numeric value");
        return;
    }
    
    m_editor.setCpWeight(weight);

    Close();
}

void SurfaceCpDlgImpl::onClose(wxCommandEvent& event)
{
    Close();
}

void SurfaceCpDlgImpl::update()
{
    EditorSelectionType cpnum = m_editor.selectedCpNum();

    if (cpnum != EditorSelectionType::None)
    {
        m_setWeightBtn->Enable(true);

        float weight = m_editor.getCpWeight();
        m_weightEdit->SetValue(wxString::FromDouble(weight));
    }
    else
        m_setWeightBtn->Enable(false);
}