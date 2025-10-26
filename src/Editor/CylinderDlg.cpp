#include "CylinderDlg.h"
#include "Editor.h"

CylinderDlgImpl::CylinderDlgImpl(wxWindow* parent, Editor& editor)
: CylinderDlg(parent)
, m_editor(editor)
{
}

void CylinderDlgImpl::onOkBtn(wxCommandEvent& event)
{
    double height = 2;
    double radius = 2;
    int sides = 6;

    m_heightEdit->GetValue().ToDouble(&height);
    m_radiusEdit->GetValue().ToDouble(&radius);
    m_sidesEdit->GetValue().ToInt(&sides);

    bool smooth = m_smoothBox->GetValue();
    bool half = m_halfBox->GetValue();

    m_editor.createCylinder(height, radius, sides, smooth, half);

    Close();
}

void CylinderDlgImpl::onCancelBtn(wxCommandEvent& event)
{
    Close();
}